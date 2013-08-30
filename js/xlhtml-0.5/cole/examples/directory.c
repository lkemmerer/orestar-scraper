/*
   cole - A free C OLE library.
   cole test.
   Copyright 1998, 1999  Roberto Arturo Tena Sanchez

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published  by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */
/*
   Arturo Tena <arturo@directmail.org>
 */


#include <stdio.h>
#include <ctype.h>

#include <cole.h>
#include <topsrcdir.h>


/*
MISSING TO TEST:
cole_opendir_direntry
cole_dir_getname
cole_dir_getsize
cole_dir_getdays1
cole_dir_getsec1
cole_dir_getdays2
cole_dir_getsec2
*/


#define PRGNAME "directory"


int
main (int argc, char **argv)
{
	COLEFS * cfs;
	COLEDIR * cd;
	COLEDIRENT * cde;
	COLEFILE * cf;
	char * entry_name;
	COLERRNO colerrno;


	cfs = cole_mount (COLE_TOPSRCDIR"/examples/text.doc", &colerrno);
	if (cfs == NULL) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}


	cd = cole_opendir_rootdir (cfs, &colerrno);
	if (cd == NULL) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}
	for (cde = cole_visiteddirentry (cd); cde != NULL;
	    cde = cole_nextdirentry (cd)) {
		if (cole_direntry_isdir (cde))
			printf ("DIR ");
		else if (cole_direntry_isfile (cde)) {
			printf ("FILE");
			/* open the file using their direntry */
			cf = cole_fopen_direntry (cde, &colerrno);
			if (cf == NULL) {
				cole_perror ("travel", colerrno);
				cole_closedir (cd, NULL);
				cole_umount (cfs, NULL);
				exit (1);
			}
			/* Here we process cf */
			if (cole_fclose (cf, &colerrno)) {
				cole_perror ("travel", colerrno);
				cole_closedir (cd, NULL);
				cole_umount (cfs, NULL);
				exit (1);
			}
		} else
			printf ("????");
		printf (" %7u", cole_direntry_getsize (cde));
		entry_name = cole_direntry_getname (cde);
		if (!isprint ((int)entry_name[0]))
			printf ("' \\x%02x%s'", entry_name[0], entry_name+1);
		else
			printf ("'%s'", entry_name);
		printf ("\t%08lx-%08lx %08lx-%08lx",
			cole_direntry_getdays1 (cde),
			cole_direntry_getsec1 (cde),
			cole_direntry_getdays2 (cde),
			cole_direntry_getsec2 (cde));
		printf ("\n");
	}
	if (cole_closedir (cd, &colerrno)) {
		cole_perror ("travel", colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


	if (cole_umount (cfs, &colerrno)) {
		cole_perror ("travel", colerrno);
		exit (1);
	}

	exit (0);
}

