/*
   cole - A free C OLE library.
   cole_isfs_fast - Fast guess if a file is a valid filesystem.
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


#define PRGNAME "cole_isfs_fast"


int
main (int argc, char **argv)
{
	FILE *file;
	unsigned char sign[8];


	if (argc != 2) {
		fprintf (stderr, PRGNAME". Prints `yes' if the file is a "
				 "valid filesystem, otherwise it prints `no'. "
				 "The test is not much accurate.\n"
				 "Usage: "PRGNAME" <FILE>\n");
		exit (1);
	}


	file = fopen (argv[1], "rb");
	if (file == NULL) {
		perror (PRGNAME);
		exit (1);
	}

	fread (sign, 1, 8, file);
	if (   sign[0] != 0xd0
	    || sign[1] != 0xcf
	    || sign[2] != 0x11
	    || sign[3] != 0xe0
	    || sign[4] != 0xa1
	    || sign[5] != 0xb1
	    || sign[6] != 0x1a
	    || sign[7] != 0xe1) {
		printf ("no\n");
		fclose (file);
		exit (1);
	}

	fclose (file);

	printf ("yes\n");

	exit (0);
}

