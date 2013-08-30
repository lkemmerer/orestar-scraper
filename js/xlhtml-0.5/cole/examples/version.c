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


int
main (int argc, char **argv)
{
	printf ("cole version (defines): %d.%d.%d\n",
		COLE_MAJOR_VERSION, COLE_MINOR_VERSION, COLE_MICRO_VERSION);
	printf ("cole version (variables): %d.%d.%d\n",
		cole_major_version, cole_minor_version, cole_micro_version);
	printf ("cole version (cole_version): %s\n", cole_version);
	printf ("cole host: %s\n", cole_host_info);

	exit (0);
}

