/*
   pptHtml - Format a PowerPoint Presentation into Html
   Copyright 2002 Charles N Wyble <jackshck@yahoo.com>	

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

#if !(defined( __BORLANDC__ ) || defined( __WIN32__ ))
#include "config.h"			/* Created by ./configure script */
#include "support.h"		/* Needs to be before internal.h */
#include "internal.h"		/* Needs to be before cole */
#include "cole.h"
#else
#include "config.h.in"		/* Created by ./configure script */
#include "support.h"		/* Needs to be before internal.h */
#include "internal.h"		/* Needs to be before cole */
#include "cole.h.in"
#include <dir.h>
#endif


#include <stdio.h>
#include <string.h>	/* for strcpy() */
#include <ctype.h>	/* For isprint */
#include <stdlib.h> /* For exitt() */


#define PRGNAME	"pptHtml"
#if !(defined( __WIN32__ ) || defined( __BORLANDC__ ))
#define PRGVER		VERSION
#else
#define PRGVER		"0.4"
#endif
#define WORK_SIZE	8192
static char FileName[2][32] =			/* The section of the PowerPoint File we read */
{
	"/PowerPoint Document",	/* Power Point 97 & 2000 */
	"/PP40"				/* Everything else ? */
};


/* Function Prototypes */
COLE_LOCATE_ACTION_FUNC dump_file;
static void container_processor(int);
static void atom_processor(int, int, int, unsigned char);
static void print_unicode(unsigned char *, int);
static void print_utf8(unsigned short c);
static void put_utf8(unsigned short c);

/* Global data */
static char filename[128];
static unsigned char working_buffer[WORK_SIZE];
static int buf_idx=0;
static int output_this_container = 0;
static int past_first_slide = 0;
static int last_container = 0;

int main (int argc, char **argv)
{
	int f_ptr = 0;
	COLEFS * cfs;
	COLERRNO colerrno;

	if (argc < 2)
	{
		fprintf (stderr, "pptHtml - Outputs Power Point files as Html.\n"
			"Usage: "PRGNAME" <FILE>\n");
		exit (1);
	}
	else
	{
		strncpy(filename, argv[1], 124);
#if defined( __WIN32__ ) || defined( __BORLANDC__ )
		{
			char *ptr = strchr(filename, ':');
			if (ptr)
			{
				int len;
				char new_drive[MAXPATH];
				fnsplit(filename, new_drive, 0, 0, 0);
				if (new_drive[0] >= 'a')
					setdisk(new_drive[0] - 'a');
				else
					setdisk(new_drive[0] - 'A');
				ptr++;	/* Get past the colon */
				len = strlen(ptr);
				memmove(filename, ptr, len);
				filename[len] = 0;
			}
	}
#endif
		cfs = cole_mount (filename, &colerrno);
		if (cfs == NULL)
		{
			cole_perror (PRGNAME, colerrno);
			exit (1);
		}
	}

	while (cole_locate_filename (cfs, FileName[f_ptr], NULL, dump_file, &colerrno))
	{
		if (f_ptr)
		{	/* Two strikes...we're out! */
			cole_perror (PRGNAME, colerrno);
			if (colerrno == COLE_EFILENOTFOUND)
				fprintf(stderr, "Section: PowerPoint Document\n");
			break;
		}
		else	/* Don't do this... */
			f_ptr++;
	}

	if (cole_umount (cfs, &colerrno))
	{
		cole_perror ("travel", colerrno);
		exit (1);
	}

	return 0;
}


void dump_file(COLEDIRENT *cde, void *_info)
{
	unsigned long version=0, instance=0, type=0, length=0, target=0, count=0;
	unsigned char buf[16];
	COLEFILE *cf;
	COLERRNO err;

	cf = cole_fopen_direntry(cde, &err);

/* Ouput Header */
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\">\n");
	printf("<HTML><HEAD><TITLE>%s", filename);
	printf("</TITLE></HEAD><BODY>\n");

/* Output body */
	while (cole_fread(cf, buf, 1, &err))
	{
		if (count == 0)
		{
			instance = buf[0];
			type = 0;
			length = 0;
			target = 80;	/* ficticious number */
		}
		else if (count == 1)
		{
			instance |= (buf[0]<<8);
			version = instance &0x000F;
			instance = (instance>>4);
		}
		else if (count == 2)
			type = (unsigned)buf[0];
		else if (count == 3)
			type |= (buf[0]<<8)&0x00000FFFL;
		else if (count == 4)
			length = (unsigned)buf[0];
		else if (count == 5)
			length |= (buf[0]<<8);
		else if (count == 6)
			length |= (buf[0]<<16);
		else if (count == 7)
		{
			length |= (buf[0]<<24);
			target = length;
			if (version == 0x0F)
			{	/* Do container level Processing */
				container_processor(type);
				count = -1;
			}
		}
		if (count > 7)
		{	/* Here is where we want to process the data
			   based on the Atom type... */
			atom_processor(type, count-8, target-1, buf[0]);
		}
		if (count == (target+7))
			count = 0;
		else
			count++;
	}

	if (past_first_slide)
		printf("<HR>");

	printf("&nbsp;<br>\n");

	/* Output Credit */
	printf("<hr><FONT SIZE=-1>Created with <a href=\"http://chicago.sf.net/xlhtml\">pptHtml</a></FONT><br>\n" );

	/* Output Tail */
	printf("</BODY></HTML>\n");
	cole_fclose(cf, &err);
}

static void container_processor(int type)
{
	if (type == 0x03EE)
	{
		if (past_first_slide)
			printf("<BR><HR><BR>\n");
		else
			past_first_slide = 1;
	}
	switch (type)
	{
		case 0x000D:
			if (last_container == 0x11)		/* suppress notes info */
				output_this_container = 0;
			else
				output_this_container = 1;
			break;
		case 0x0FF0:
			output_this_container = 1;
			break;
		default:
/*			printf("Cont:%x|\n", type);	*/
			output_this_container = 0;
			break;
	}
	last_container = type;
}

static void atom_processor(int type, int count, int buf_last, unsigned char data)
{
	if ((buf_idx >= WORK_SIZE)||(output_this_container == 0))
		return;

	if (count == 0)
	{
		memset(working_buffer, 0, WORK_SIZE);
		buf_idx = 0;
	}
	
	switch (type)
	{
		case 0x0FA0:	/* Text String in unicode */
			working_buffer[buf_idx++] = data;
			if (count == buf_last)
			{
/*				printf("Atom:%x|\n", type);	*/
/*				working_buffer[buf_idx++] = 0;       */
/*				printf("%s<BR>\n", working_buffer); */
				print_unicode(working_buffer, buf_idx);
				printf("<BR>\n");
			}
			break;
		case 0x0FA8:	/* Text String in ASCII */
			working_buffer[buf_idx++] = data;
			if (count == buf_last)
			{
				int i;
/*				working_buffer[buf_idx++] = 0;	*/
/*				printf("Atom:%x|\n", type);	*/
				for (i=0;i<buf_idx; i++)
				{
/*					printf("%02X &nbsp;", (int)working_buffer[i]); */	/* Debug */
					if (working_buffer[i] == 0x0D)
						printf("<BR>\n");
					else
						putchar(working_buffer[i]);
				}
				printf("<BR>\n");
			}
			break;
		case 0x0FBA:	/* CString - unicode... */
			working_buffer[buf_idx++] = data;
			if (count == buf_last)
			{
/*				working_buffer[buf_idx++] = 0;	*/
/*				printf("%s<BR>\n", working_buffer); */
/*				printf("Atom:%x|\n", type);	*/
				print_unicode(working_buffer, buf_idx);
				printf("<BR>\n");
			}
			break;
		default:
			break;
	}
}

static void print_unicode(unsigned char *ucs, int len)
{
	int i;
	for (i = 0; i < len; i += 2)
		print_utf8(ucs[i] | (ucs[i+1] << 8));
}

static void OutputCharCorrected(unsigned char c)
{
	switch (c)
	{	/* Special char handlers here... */
		case '\r':
			printf("<BR>\n");
			break;
		case 0x3C:
			printf("&lt;");
			break;
		case 0x3E:
			printf("&gt;");
			break;
		case 0x26:
			printf("&amp;");
			break;
		case 0x22:
			printf("&quot;");
			break;
		/* Also need to cover 128-159 since MS uses this area... */
		case 0x80:		/* Euro Symbol */
			printf("&#8364;");
			break;
		case 0x82:		/* baseline single quote */
			printf("&#8218;");
			break;
		case 0x83:		/* florin */
			printf("&#402;");
			break;
		case 0x84:		/* baseline double quote */
			printf("&#8222;");
			break;
		case 0x85:		/* ellipsis */
			printf("&#8230;");
			break;
		case 0x86:		/* dagger */
		    printf("&#8224;");
		    break;
		case 0x87:		/* double dagger */
		    printf("&#8225;");
		    break;
		case 0x88:		/* circumflex accent */
		    printf("&#710;");
		    break;
		case 0x89:		/* permile */
		    printf("&#8240;");
		    break;
		case 0x8A:		/* S Hacek */
		    printf("&#352;");
		    break;
		case 0x8B:		/* left single guillemet */
		    printf("&#8249;");
		    break;
		case 0x8C:		/* OE ligature */
		    printf("&#338;");
		    break;
		case 0x8E:		/*  #LATIN CAPITAL LETTER Z WITH CARON */
			printf("&#381;");
			break;
		case 0x91:		/* left single quote ? */
		    printf("&#8216;");
		    break;
		case 0x92:		/* right single quote ? */
		    printf("&#8217;");
		    break;
		case 0x93:		/* left double quote */
		    printf("&#8220;");
		    break;
		case 0x94:		/* right double quote */
		    printf("&#8221;");
		    break;
		case 0x95:		/* bullet */
		    printf("&#8226;");
		    break;
		case 0x96:		/* endash */
		    printf("&#8211;");
		    break;
		case 0x97:		/* emdash */
		    printf("&#8212;");
		    break;
		case 0x98:		/* tilde accent */
		    printf("&#732;");
		    break;
		case 0x99:		/* trademark ligature */
		    printf("&#8482;");
		    break;
		case 0x9A:		/* s Haceks Hacek */
		    printf("&#353;");
		    break;
		case 0x9B:		/* right single guillemet */
		    printf("&#8250;");
		    break;
		case 0x9C:		/* oe ligature */
		    printf("&#339;");
		    break;
		case 0x9F:		/* Y Dieresis */
		    printf("&#376;");
		    break;
		default:
			putchar(c);
			break;
	}
}

static void print_utf8(unsigned short c)
{
	if (c == 0)
		return;
		
	if (c < 0x80)
		OutputCharCorrected(c);
	else if (c < 0x800)
	{
		putchar(0xC0 | (c >>  6));
		put_utf8(c);
	}
	else
	{
		putchar(0xE0 | (c >> 12));
		put_utf8(c >>  6);
		put_utf8(c);
	}
}

static void put_utf8(unsigned short c)
{
	putchar(0x0080 | ((short)c & 0x003F));
}

