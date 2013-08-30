/*
   pptdump - dumps individual records for analysis
   Copyright 2002  Charles N Wyble <jackshck@yahoo.com>

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
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/* for strcpy() */
#include <ctype.h>	/* For isprint */


#define MODE 1

#define PRGNAME "pptdump"
#define MAX_COLS 64
#define MAX_ROWS 512

static char FileName[2][32] =			/* The section of the PowerPoint File we read */
{
	"/PowerPoint Document",	/* Power Point 97 & 2000 */
	"/PP40"				/* Everything else ? */
};

/* Function Prototypes */
COLE_LOCATE_ACTION_FUNC dump_file;
static void output_type_string(int);

/* Global data */
static char filename[128];


int main (int argc, char **argv)
{
	int f_ptr = 0;
	COLEFS * cfs;
	COLERRNO colerrno;

	if (argc < 2)
	{
		fprintf (stderr, "pptdump - Outputs PowerPoint file records for analysis.\n"
			"Usage: "PRGNAME" <FILE>\n");
		exit (1);
	}
	else
	{
		strncpy(filename, argv[1], 124);
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
		{
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
	unsigned long cversion=0, cinstance=0, ctype=0, clength=0, ctarget=0, ccount = 0;
	unsigned char buf[16];
	COLEFILE *cf;
	COLERRNO err;
	
	cf = cole_fopen_direntry(cde, &err);	

/* Ouput Header */
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\">\n");
	printf("<HTML><HEAD><TITLE>%s", filename);	
	printf("</TITLE></HEAD><BODY>\n");	

/* Output body */
#if (MODE == 1)
	while (cole_fread(cf, buf, 8, &err)) /* For mode >= 1 */
#else
	while (cole_fread(cf, buf, 1, &err))
#endif	
	{
		if (MODE == 0)
		{
			if (ccount == 0)
			{
				cinstance = buf[0];
				ctype = 0;
				clength = 0;
				ctarget = 80;	/* ficticious number */
				printf("\n<br>\n");
			}
			else if (ccount == 1)
			{
				cinstance |= (buf[0]<<8);
				cversion = cinstance &0x000F;
				cinstance = (cinstance>>4);
			}
			else if (ccount == 2)
				ctype = (unsigned)buf[0];	
			else if (ccount == 3)
				ctype |= (buf[0]<<8)&0x00000FFFL;	
			else if (ccount == 4)
				clength = (unsigned)buf[0];	
			else if (ccount == 5)
				clength |= (buf[0]<<8);	
			else if (ccount == 6)
				clength |= (buf[0]<<16);	
			else if (ccount == 7)
			{
				clength |= (buf[0]<<24);
				ctarget = clength;
				if (cversion == 0x0F)
				{
					printf("Container");
					ccount = -1;
				}
				else
					printf("Atom");
				printf(" - Version:%02X Instance:%02X, Type:%03X, Length:%08X - ", 
								(int)cversion, (int)cinstance, (int)ctype, (int)clength);
				output_type_string(ctype);
				puts("<br>\n");
			}
			if (ccount > 7)
			{	/* Here is where we want to process the data */
				/* based on the opcode... */
				/* process_atom(buf[0]); */
				if (isprint(buf[0]))
					putc(buf[0], stdout);	
			}		
			if (ccount == (ctarget+7))
				ccount = 0;
			else		
				ccount++;	
		}
		else	/* mode >= 1 */
		{
			printf("%02x %02x %02x %02x %02x %02x %02x %02x &nbsp; &nbsp; &nbsp; &nbsp; ",
			  (unsigned)buf[0], (unsigned)buf[1], (unsigned)buf[2], (unsigned)buf[3], 
			  (unsigned)buf[4], (unsigned)buf[5], (unsigned)buf[6], (unsigned)buf[7]);
			  putchar(buf[0]); putchar(buf[1]); 
			  putchar(buf[2]); putchar(buf[3]); 
			  putchar(buf[4]); putchar(buf[5]); 
			  putchar(buf[6]); putchar(buf[7]);
			  printf("<br>");
		}
	}

/* Output Tail */
	printf("</BODY></HTML>\n");	
	cole_fclose(cf, &err);
}

static void output_type_string(int type)
{
	switch (type)
	{
		case 0x0001:
			puts("SubContainerCompleted");
			break;
		case 0x0002:
			puts("IRRAtom");
			break;
		case 0x0003:
			puts("PSS");
			break;
		case 0x0004:
			puts("SubContainerException");
			break;
		case 0x0006:
			puts("ClientSignal1");
			break;
		case 0x0007:
			puts("ClientSignal2");
			break;
		case 0x000A:
			puts("PowerPointStateAtom");
			break;
		case 0x03E8:
			puts("Document");
			break;
		case 0x03E9:
			puts("Document Atom");
			break;
		case 0x03EA:
			puts("End Document");
			break;
		case 0x03EB:
			puts("Slide Persist");
			break;
		case 0x03EC:
			puts("SlideBase");
			break;
		case 0x03ED:
			puts("SlideBase Atom");
			break;
		case 0x03EE:
			puts("Slide");
			break;
		case 0x03EF:
			puts("Slide Atom");
			break;
		case 0x03F0:
			puts("Notes");
			break;
		case 0x03F1:
			puts("Notes Atom");
			break;
		case 0x03F2:
			puts("Environment");
			break;
		case 0x03F3:
			puts("SlidePersistAtom");
			break;
		case 0x03F4:
			puts("Scheme");
			break;
		case 0x03F5:
			puts("Scheme Atom");
			break;
		case 0x03F6:
			puts("DocViewInfo");
			break;
		case 0x03F7:
			puts("SlideLayout Atom");
			break;
		case 0x03F8:
			puts("MainMaster");
			break;
		case 0x03F9:
			puts("SlideInfo Atom");
			break;
		case 0x03FA:
			puts("SlideViewInfo");
			break;
		case 0x03FB:
			puts("Guide Atom");
			break;
		case 0x03FC:
			puts("ViewInfo");
			break;
		case 0x03FD:
			puts("ViewInfo Atom");
			break;
		case 0x03FE:
			puts("SlideViewInfo Atom");
			break;
		case 0x03FF:
			puts("VBAInfo");
			break;
		case 0x0400:
			puts("VBAInfo Atom");
			break;
		case 0x0401:
			puts("SSDocInfo Atom");
			break;
		case 0x0402:
			puts("Summary");
			break;
		case 0x0403:
			puts("Texture");
			break;
		case 0x0404:
			puts("VBASlideInfo");
			break;
		case 0x0405:
			puts("VBASlideInfo Atom");
			break;
		case 0x0406:
			puts("DocRoutingSlip");
			break;
		case 0x0407:
			puts("OutlineViewInfo");
			break;
		case 0x0408:
			puts("SorterViewInfo");
			break;
		case 0x0409:
			puts("ExObjList");
			break;
		case 0x040A:
			puts("ExObjList Atom");
			break;
		case 0x040B:
			puts("PPDrawingGroup");
			break;
		case 0x040C:
			puts("PPDrawing");
			break;
		case 0x0410:
			puts("NamedShows - Several");
			break;
		case 0x0411:
			puts("NamedShow - One");
			break;
		case 0x0412:
			puts("NamedShowSlides");
			break;
		case 0x07D0:
			puts("List");
			break;
		case 0x07D5:
			puts("Font Collection");
			break;
		case 0x07E1:
			puts("ListPlaceHolder");
			break;
		case 0x07E3:
			puts("BookMarkCollection");
			break;
		case 0x07E4:
			puts("SoundCollection");
			break;
		case 0x07E5:
			puts("SoundColl Atom");
			break;
		case 0x07E6:
			puts("Sound");
			break;
		case 0x07E7:
			puts("SoundData");
			break;
		case 0x07E9:
			puts("BookMarkSeed  Atom");
			break;
		case 0x07EA:
			puts("GuideList");
			break;
		case 0x07EB:
			puts("RunArray");
			break;
		case 0x07EC:
			puts("RunArray Atom");
			break;
		case 0x07ED:
			puts("ArrayElement Atom");
			break;
		case 0x07EE:
			puts("Int4Array Atom");
			break;
		case 0x07EF:
			puts("ColorScheme Atom");
			break;
		case 0x07F0:
			puts("ColorSchemeAtom");
			break;
		case 0x0BC0:
			puts("OEShape");
			break;
		case 0x0BC1:
			puts("ExObjRef Atom");
			break;
		case 0x0BC3:
			puts("OEPlaceholder Atom");
			break;
		case 0x0BCC:
			puts("GrColor");
			break;
		case 0x0BD1:
			puts("Grect Atom");
			break;
		case 0x0BD7:
			puts("GRatio Atom");
			break;
		case 0x0BD8:
			puts("GScaling");
			break;
		case 0x0BDA:
			puts("GPoint Atom");
			break;
		case 0x0BDB:
			puts("OEShape Atom");
			break;
		case 0x0F9E:
			puts("OutlineTextRef Atom");
			break;
		case 0x0F9F:
			puts("TextHeader Atom");
			break;
		case 0x0FA0:
			puts("TextChars Atom");
			break;
		case 0x0FA1:
			puts("StyleTextProp Atom");
			break;
		case 0x0FA2:
			puts("BaseTextProp Atom");
			break;
		case 0x0FA3:
			puts("TxMasterStyle Atom");
			break;
		case 0x0FA4:
			puts("TxCFStyle Atom");
			break;
		case 0x0FA5:
			puts("TxPFStyle Atom");
			break;
		case 0x0FA6:
			puts("TextRuler Atom");
			break;
		case 0x0FA7:
			puts("TextBookMark Atom");
			break;
		case 0x0FA8:
			puts("TextBytes Atom");
			break;
		case 0x0FA9:
			puts("TxSiStyle Atom");
			break;
		case 0x0FAA:
			puts("TextSpecInfo");
			break;
		case 0x0FAB:
			puts("DefaultRuler Atom");
			break;
		case 0x0FB7:
			puts("FontEntity Atom");
			break;
		case 0x0FB8:
			puts("FontEmbedData");
			break;
		case 0x0FB9:
			puts("TypeFace");
			break;
		case 0x0FBA:
			puts("CString");
			break;
		case 0x0FBB:
			puts("ExternalObject");
			break;
		case 0x0FC1:
			puts("MetaFile");
			break;
		case 0x0FC2:
			puts("ExOleObj");
			break;
		case 0x0FC3:
			puts("ExOleObj Atom");
			break;
		case 0x0FE9:
			puts("ExQuickTime");
			break;
		case 0x0FD9:
			puts("HeadersFooters");
			break;
		case 0x0FDA:
			puts("HeadersFooters Atom");
			break;
		case 0x0FF0:
			puts("SlideListWithText");
			break;
		case 0x0FF1:
			puts("AnimationInfoAtom");
			break;
		case 0x0FF2:
			puts("InteractiveInfo");
			break;
		case 0x0FF5:
			puts("UserEditAtom");
			break;
		case 0x0FF6:
			puts("CurrentUserAtom");
			break;
		case 0x0FF7:
			puts("DateTimeMCAtom ");
			break;
		default:
			puts("Unknown Type");
			break;
	}
}

