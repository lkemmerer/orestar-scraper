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


/* cole_fopen_direntry is tested in `directory.c' */


#define PRGNAME "file"
#define BUFFER_SIZE 128
#define FILENAME2_SIZE 108004


int
main (int argc, char **argv)
{
	COLEFS * cfs;
	COLEFILE * cf;
	COLERRNO colerrno;
	char buffer[BUFFER_SIZE];
	size_t char_read;
	size_t char_read_total;
	char *filename2 = "/ObjectPool/_994231763/\001Ole10Native";
	char *filename2_pretty = "/ObjectPool/_994231763/\\001Ole10Native";

	cfs = cole_mount (COLE_TOPSRCDIR"/examples/text.doc", &colerrno);
	if (cfs == NULL) {
		cole_perror (PRGNAME, colerrno);
		exit (1);
	}

	cf = cole_fopen (cfs, filename2, &colerrno);
	if (cf == NULL) {
		cole_perror (PRGNAME, colerrno);
		cole_umount (cfs, NULL);
		exit (1);
	}



	printf ("File size: %d\n", cole_fsize (cf));
	if (cole_fsize (cf) != FILENAME2_SIZE) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	printf ("File position: %d\n", cole_ftell (cf));
	char_read_total = 0;
	while ((char_read = cole_fread (cf, buffer, BUFFER_SIZE, &colerrno)))
		char_read_total += char_read;
	cole_perror (PRGNAME, colerrno);
	printf ("File position: %d\n", cole_ftell (cf));
	printf ("Read %d bytes from the stream `%s'\n", char_read_total,
		filename2_pretty);


	if (cole_frewind (cf, &colerrno)) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	printf ("File position: %d\n", cole_ftell (cf));
	if (cole_ftell (cf) != 0) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	if (cole_fseek (cf, 0, COLE_SEEK_END, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));
	if (cole_ftell (cf) != FILENAME2_SIZE) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	if (cole_fseek (cf, 200, COLE_SEEK_END, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));
	if (cole_ftell (cf) != FILENAME2_SIZE - 200) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	if (cole_fseek (cf, 100, COLE_SEEK_FORWARD, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));
	if (cole_ftell (cf) != FILENAME2_SIZE - 200 + 100) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	if (cole_fseek (cf, 200, COLE_SEEK_BACKWARD, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));
	if (cole_ftell (cf) != FILENAME2_SIZE - 200 + 100 - 200) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}

	if (!cole_fseek (cf, FILENAME2_SIZE + 1, COLE_SEEK_SET, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));

	if (cole_fseek (cf, FILENAME2_SIZE, COLE_SEEK_END, &colerrno)) {
		cole_perror (PRGNAME, colerrno);
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	if (cole_ftell (cf) != 0) {
		fprintf (stderr, "Test failed\n");
		cole_fclose (cf, NULL);
		cole_umount (cfs, NULL);
		exit (1);
	}
	printf ("File position: %d\n", cole_ftell (cf));



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

