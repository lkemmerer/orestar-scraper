/*
   cole - A free C OLE library.
   cole using sample.
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

/* To compile this file outside cole source tree,
   you must include here <cole/cole.h> instead <cole.h> */
#include <cole.h>


#define PRGNAME "demo"
#define BUFFER_SIZE 128


int
main (int argc, char **argv)
{
	COLEFS * cfs;
	COLEDIR * cd;
	COLEDIRENT * cde;
	COLEFILE * cf;
	COLERRNO colerrno;
	char * entry_name;
	char buffer[BUFFER_SIZE];
	size_t char_read;
	size_t char_read_total;


	if (argc != 2) {
		fprintf (stderr, "cole demo. cole is a free C OLE library.\n"
			"Usage: demo <FILE>\n");
		return 1;
	}


	/*
	 * version info
	 */
	printf ("Version info: (%d.%d.%d) (%d.%d.%d) (%s)\n",
		COLE_MAJOR_VERSION, COLE_MINOR_VERSION, COLE_MICRO_VERSION,
		cole_major_version, cole_minor_version, cole_micro_version,
		cole_version);
	printf ("Host info: (%s)\n", cole_host_info);


	cfs = cole_mount (argv[1], &colerrno);
	if (cfs == NULL) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}



	if (cole_print_tree (cfs, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
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
			printf ("DIR  ");
		else if (cole_direntry_isfile (cde)) {
			printf ("FILE ");
			/* open the file using their direntry */
			cf = cole_fopen_direntry (cde, &colerrno);
			if (cf == NULL) {
				cole_perror ("travel", colerrno);
				cole_closedir (cd, NULL);
				cole_umount (cfs, NULL);
				exit (1);
			}
			if (cole_fclose (cf, &colerrno)) {
				cole_perror (PRGNAME, colerrno);
				cole_closedir (cd, NULL);
				cole_umount (cfs, NULL);
				exit (1);
			}
		} else
			printf ("???? ");
		printf ("%7u ", cole_direntry_getsize (cde));
		entry_name = cole_direntry_getname (cde);
		printf ("%08lx-%08lx %08lx-%08lx\t",
			cole_direntry_getdays1 (cde),
			cole_direntry_getsec1 (cde),
			cole_direntry_getdays2 (cde),
			cole_direntry_getsec2 (cde));
		if (!isprint ((int)entry_name[0]))
			printf ("'\\x%02x%s'\n", entry_name[0], entry_name+1);
		else
			printf ("'%s'\n", entry_name);
	}
	if (cole_closedir (cd, &colerrno)) {
		cole_umount (cfs, NULL);
		exit (1);
	}


	cf = cole_fopen (cfs, "\005SummaryInformation", &colerrno);
	if (cf == NULL) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}
	char_read_total = 0;
	printf ("Reading: ");
	while ((char_read = cole_fread (cf, buffer, BUFFER_SIZE, &colerrno))) {
		printf ("[%d]", char_read);
		char_read_total += char_read;
	}
	cole_perror (PRGNAME, colerrno);
	printf ("\nRead %d bytes from the stream '\\005SummaryInformation'\n",
		char_read_total);
	if (cole_fclose (cf, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}



	if (cole_umount (cfs, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}

	exit (0);
}

