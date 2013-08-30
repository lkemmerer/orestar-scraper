/*
   cole - A free C OLE library.
   cole_extract - Extract a file from a filesystem.
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


#define PRGNAME "cole_extract"
#define BUFFER_SIZE 128


int
main (int argc, char **argv)
{
	COLEFS *cfs;
	COLEFILE *cf;
	COLERRNO colerrno;
	char buffer[BUFFER_SIZE];
	size_t char_read;


	if (argc != 3) {
		fprintf (stderr, "cole_extract. Extract a file from a "
			 "filesystem to the standard output.\n"
			 "Usage: "PRGNAME" <FILE> <INFILE>\n"
			 "       FILE - File with the filesystem.\n"
			 "       INFILE - Filename of the file to extract.\n");
		exit (1);
	}


	cfs = cole_mount (argv[1], &colerrno);
	if (cfs == NULL) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}

	cf = cole_fopen (cfs, argv[2], &colerrno);
	if (cf == NULL) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


	while ((char_read = cole_fread (cf, buffer, BUFFER_SIZE, &colerrno))) {
		if (fwrite (buffer, 1, char_read, stdout) != char_read) {
			break;
		}
	}
	if (!cole_feof (cf)) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}


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

