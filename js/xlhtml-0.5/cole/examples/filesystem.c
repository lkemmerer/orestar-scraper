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

#include <cole.h>
#include <topsrcdir.h>


#define PRGNAME "filesystem"


/* To test cole_locate_filename() */
struct str_info {
	char *name;
	size_t size;
};
COLE_LOCATE_ACTION_FUNC check_size;


int
main (int argc, char **argv)
{
	COLEFS * cfs;
	COLERRNO colerrno;

	/* To test cole_locate_filename() */
#define FILENAME "/ObjectPool/_994231763/\001Ole10Native"
	struct str_info info = { FILENAME, 108004 };


	cfs = cole_mount (COLE_TOPSRCDIR"/examples/text.doc", &colerrno);
	if (cfs == NULL) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}


	if (cole_print_tree (cfs, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


	if (cole_locate_filename (cfs, FILENAME, &info, check_size,
				  &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


	if (cole_recurse_tree (cfs, NULL, NULL, NULL, NULL, NULL, NULL,
			       &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


	if (cole_umount (cfs, &colerrno)) {
		cole_perror ("travel", colerrno);
		exit (1);
	}

	exit (0);
}


/* To test cole_locate_filename() */
void
check_size (COLEDIRENT *cde, void *_info) {
	struct str_info *info = (struct str_info *)_info;

	if (info->size != cole_direntry_getsize (cde))
		printf ("SIZE OF `%s' DOESN'T CHECK!\n", info->name);
	else
		printf ("Size of `%s' is ok\n", info->name);
}

