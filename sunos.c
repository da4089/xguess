/***************************************************************
 *
 *              xguess
 *              X implementation attribute testing
 *
 * File:        $Source$
 * Version:     $RCSfile$ $Revision$
 * Copyright:   (C) 1997-2003 David Arnold.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ****************************************************************/

/*
 * xkeycaps, Copyright (c) 1991, 1992, 1993, 1994, 1996
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


/*
 * SunOS-specific stuff: if we're on console, we can query the keyboard
 * hardware directly to find out what kind it is.
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

  unknown = strdup("sun-unknown-unknown");

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
    return strdup("sun-ascii");
  case KB_KLUNK:          /* Micro Switch 103SD32-2 */
    return strdup("sun-ascii-ms103sd32-2");
  case KB_VT100:          /* Keytronics VT100 */
    return strdup("sun-vt100");
  case KB_VT220:          /* vt220 Emulation */
    return strdup("sun-vt220");
  case KB_VT220I:    	  /* International vt220 */
    return strdup("sun-vt220i");
  case KB_SUN2:
    return strdup("sun-type2");
  case KB_SUN3:
    return strdup("sun-type3");

  case KB_SUN4:
    switch (layout) {
    case  0: return strdup("sun-type4-sun4");     /* Part 320-1005-02 REV A. */
    case  1: return strdup("sun-type4-sun4");     /* Part 320-1005-01 REV B. */

    case  2: return strdup("sun-type4-sun4fr");   /* French/Belgian */
    case  3: return strdup("sun-type4-sun4ca");	  /* Canadian	*/
    case  4: return strdup("sun-type4-sun4dk");	  /* Danish	*/
    case  5: return strdup("sun-type4-sun4de");	  /* German	*/
    case  6: return strdup("sun-type4-sun4it");	  /* Italian	*/
    case  7: return strdup("sun-type4-sun4du");	  /* Dutch	*/
    case  8: return strdup("sun-type4-sun4no");	  /* Norwegian	*/
    case  9: return strdup("sun-type4-sun4po");	  /* Portuguese	*/
    case 10: return strdup("sun-type4-sun4ed");	  /* Spanish	*/
    case 11: return strdup("sun-type4-sun4sw");	  /* Swedish/Finnish */
    case 12: return strdup("sun-type4-sun4swfr"); /* Swiss/French */
    case 13: return strdup("sun-type4-sun4swde"); /* Swiss/German */
    case 14: return strdup("sun-type4-sun4uk");   /* UK		*/
      /* 15 unknown */
    case 16: return strdup("sun-type4-sun4ko");	  /* Korean	*/
    case 17: return strdup("sun-type4-sun4ta");	  /* Taiwanese	*/
      /* 18 unknown */
    case 19: return strdup("sun-pc101-sun101a");  /* US		*/
      /* The Sun101A was apparently an early version
	 of the Sun5 kbd: it didn't last very long. */
      /* 20-31 unknown */
    case 32: return strdup("sun-type4-sun4ja");	  /* Japanese	*/

      /* It appears that there is no such keyboard as (for example) Sun5PCDE,
	 or any non-US version of the sun5PC keyboard.  I guess non-US versions
	 only exist for the Sun5 keyboard.  This is odd, but it looks like the
	 stuff in /usr/share/lib/keytables/ doesn't provide for that
	 possibility either. */

    case 33: return strdup("sun-type5-sun5pc");   /* US		*/
    case 34: return strdup("sun-type5-sun5");     /* US		*/
    case 35: return strdup("sun-type5-sun5fr");   /* French	*/
    case 36: return strdup("sun-type5-sun5da");   /* Danish	*/
    case 37: return strdup("sun-type5-sun5de");	  /* German	*/
    case 38: return strdup("sun-type5-sun5it");   /* Italian	*/
    case 39: return strdup("sun-type5-sun5du");   /* Dutch	*/
    case 40: return strdup("sun-type5-sun5no");   /* Norwegian	*/
    case 41: return strdup("sun-type5-sun5po");   /* Portuguese	*/
    case 42: return strdup("sun-type5-sun5es");   /* Spanish	*/
    case 43: return strdup("sun-type5-sun5sw");   /* Swedish	*/
    case 44: return strdup("sun-type5-sun5swfr"); /* Swiss/French	*/
    case 45: return strdup("sun-type5-sun5swde"); /* Swiss/German	*/
    case 46: return strdup("sun-type5-sun5uk");   /* UK		*/
    case 47: return strdup("sun-type5-sun5ko");   /* Korean	*/
    case 48: return strdup("sun-type5-sun5ta");   /* Taiwanese	*/
    case 49: return strdup("sun-type5-sun5ja");   /* Japanese	*/

    default:
      {
	char buf [255];
	sprintf (buf, "sun-sun4%d", layout);
	return strdup (buf);
      }
    }

  default:
    {
      char buf [255];
      if (layout)
	sprintf (buf, "sun-%d_%d", type, layout);
      else
	sprintf (buf, "sun-%d", type);
      return strdup (buf);
    }
  }
}

#endif /* __sun || sun	*/

/***************************************************************/
/*  end of sunos.c */
