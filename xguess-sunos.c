/***************************************************************
 Copyright (C) DSTC Pty Ltd (ACN 052 372 577) 1997.
 Unpublished work.  All Rights Reserved.
 
 The software contained on this media is the property of the DSTC
 Pty Ltd.  Use of this software is strictly in accordance with the
 license agreement in the accompanying LICENSE.DOC file.  If your
 distribution of this software does not contain a LICENSE.DOC file
 then you have no rights to use this software in any manner and
 should contact DSTC at the address below to determine an
 appropriate licensing arrangement.
 
      DSTC Pty Ltd
      Level 7, Gehrmann Labs
      University of Queensland
      St Lucia, 4072
      Australia
      Tel: +61 7 3365 4310
      Fax: +61 7 3365 4311
      Email: enquiries@dstc.edu.au
 
 This software is being provided "AS IS" without warranty of any
 kind, and DSTC Pty Ltd disclaims all warranties.
 
 Project:  Hector
 File:     $Source$

****************************************************************/

/* xkeycaps, Copyright (c) 1991, 1992, 1993, 1994, 1996
 * by Jamie Zawinski <jwz@netscape.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/* SunOS-specific stuff: if we're on console, we can query the keyboard
   hardware directly to find out what kind it is.  I would have just put
   this code in guess.c, but vuid_event.h defines a `struct keyboard' 
   that conflicts with our own...
 */

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif


static char *xguess_sunos_dummy = "warning prevention";

#if defined(__sun) || defined(sun)	/* whole file */

#include <stdlib.h>
#include <unistd.h>
extern char *strdup (const char *);

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#if defined(__SVR4)                     /* SunOS >= 5.0.0 */
#include <sys/vuid_event.h>
#include <sys/kbio.h>
#include <sys/kbd.h>
#else                                   /* SunOS <= 4.1.4 */
#include <sundev/vuid_event.h>
#include <sundev/kbio.h>
#include <sundev/kbd.h>
#endif

char *local_guess(void) {
  int   type = -1, layout = 0;
  int   kbdfd;
  char  *unknown;

  unknown = strdup("unknown-sun-unknown");

  if ((kbdfd = open ("/dev/kbd", O_WRONLY)) <= 0) {
    return unknown;
  }

  /* See /usr/man/man4/kb.4m on SunOS 4.1.3 */

  if (ioctl (kbdfd, KIOCTYPE, &type))
    {
      close (kbdfd);
      return unknown;
    }
  ioctl (kbdfd, KIOCLAYOUT, &layout);
  close (kbdfd);

  switch (type) {
  case -1:
    return unknown;
  case KB_ASCII:	  /* Ascii terminal */
    return strdup("ascii-sun-ascii");
  case KB_KLUNK:          /* Micro Switch 103SD32-2 */
    return strdup("ascii-sun-ms103sd32-2");
  case KB_VT100:          /* Keytronics VT100 */
    return strdup("vt100-sun-vt100");
  case KB_VT220:          /* vt220 Emulation */
    return strdup("vt220-sun-vt220");
  case KB_VT220I:    	  /* International vt220 */
    return strdup("vt220-sun-vt220i");
  case KB_SUN2:
    return strdup("sun2-sun-sun2");
  case KB_SUN3:
    return strdup("sun3-sun-sun3");

  case KB_SUN4:
    switch (layout) {
    case  0: return strdup("type4-sun-sun4");     /* Part 320-1005-02 REV A. */
    case  1: return strdup("type4-sun-sun4");     /* Part 320-1005-01 REV B. */

    case  2: return strdup("type4-sun-sun4fr");   /* French/Belgian */
    case  3: return strdup("type4-sun-sun4ca");	  /* Canadian	*/
    case  4: return strdup("type4-sun-sun4dk");	  /* Danish	*/
    case  5: return strdup("type4-sun-sun4de");	  /* German	*/
    case  6: return strdup("type4-sun-sun4it");	  /* Italian	*/
    case  7: return strdup("type4-sun-sun4du");	  /* Dutch	*/
    case  8: return strdup("type4-sun-sun4no");	  /* Norwegian	*/
    case  9: return strdup("type4-sun-sun4po");	  /* Portuguese	*/
    case 10: return strdup("type4-sun-sun4ed");	  /* Spanish	*/
    case 11: return strdup("type4-sun-sun4sw");	  /* Swedish/Finnish */
    case 12: return strdup("type4-sun-sun4swfr"); /* Swiss/French */
    case 13: return strdup("type4-sun-sun4swde"); /* Swiss/German */
    case 14: return strdup("type4-sun-sun4uk");   /* UK		*/
      /* 15 unknown */
    case 16: return strdup("type4-sun-sun4ko");	  /* Korean	*/
    case 17: return strdup("type4-sun-sun4ta");	  /* Taiwanese	*/
      /* 18 unknown */
    case 19: return strdup("pc101-sun-sun101a");  /* US		*/
      /* The Sun101A was apparently an early version
	 of the Sun5 kbd: it didn't last very long. */
      /* 20-31 unknown */
    case 32: return strdup("type4-sun-sun4ja");	  /* Japanese	*/

      /* It appears that there is no such keyboard as (for example) Sun5PCDE,
	 or any non-US version of the sun5PC keyboard.  I guess non-US versions
	 only exist for the Sun5 keyboard.  This is odd, but it looks like the
	 stuff in /usr/share/lib/keytables/ doesn't provide for that
	 possibility either. */

    case 33: return strdup("type5-sun-sun5pc");   /* US		*/
    case 34: return strdup("type5-sun-sun5");     /* US		*/
    case 35: return strdup("type5-sun-sun5fr");   /* French	*/
    case 36: return strdup("type5-sun-sun5da");   /* Danish	*/
    case 37: return strdup("type5-sun-sun5de");	  /* German	*/
    case 38: return strdup("type5-sun-sun5it");   /* Italian	*/
    case 39: return strdup("type5-sun-sun5du");   /* Dutch	*/
    case 40: return strdup("type5-sun-sun5no");   /* Norwegian	*/
    case 41: return strdup("type5-sun-sun5po");   /* Portuguese	*/
    case 42: return strdup("type5-sun-sun5es");   /* Spanish	*/
    case 43: return strdup("type5-sun-sun5sw");   /* Swedish	*/
    case 44: return strdup("type5-sun-sun5swfr"); /* Swiss/French	*/
    case 45: return strdup("type5-sun-sun5swde"); /* Swiss/German	*/
    case 46: return strdup("type5-sun-sun5uk");   /* UK		*/
    case 47: return strdup("type5-sun-sun5ko");   /* Korean	*/
    case 48: return strdup("type5-sun-sun5ta");   /* Taiwanese	*/
    case 49: return strdup("type5-sun-sun5ja");   /* Japanese	*/

    default:
      {
	char buf [255];
	sprintf (buf, "unknown-sun-sun4%d", layout);
	return strdup (buf);
      }
    }

  default:
    {
      char buf [255];
      if (layout)
	sprintf (buf, "unknown-sun-%d_%d", type, layout);
      else
	sprintf (buf, "unknown-sun-%d", type);
      return strdup (buf);
    }
  }
}


#ifdef DEBUG
void
main (void)
{
  printf ("%s\n", local_guess());
  exit (0);
}
#endif /* DEBUG */


#endif /* __sun	*/
