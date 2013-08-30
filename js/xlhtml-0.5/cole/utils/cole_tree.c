/*
   cole - A free C OLE library.
   cole_tree - Prints the tree of a filesystem.
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

/* To compile this file outside cole source tree,
   you must include here <cole/cole.h> instead <cole.h> */
#include <cole.h>


#define PRGNAME "cole_tree"


int
main (int argc, char **argv)
{
	COLEFS * cfs;
	COLERRNO colerrno;


	if (argc != 2) {
		fprintf (stderr, "cole_tree. Prints the tree of a filesystem.\n"
			"Usage: "PRGNAME" <FILE>\n");
		exit (1);
	}


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

	if (cole_umount (cfs, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}

	exit (0);
}

