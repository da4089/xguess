/***************************************************************

  Copyright (C) DSTC Pty Ltd (ACN 052 372 577) 1995, 1997.
  Unpublished work.  All Rights Reserved.

  The software contained on this media is the property of the
  DSTC Pty Ltd.  Use of this software is strictly in accordance
  with the license agreement in the accompanying LICENSE.DOC
  file.  If your distribution of this software does not contain
  a LICENSE.DOC file then you have no rights to use this
  software in any manner and should contact DSTC at the address
  below to determine an appropriate licensing arrangement.

     DSTC Pty Ltd
     Level 7, Gehrmann Labs
     University of Queensland
     St Lucia, 4072
     Tel: +61 7 3365 4310
     Fax: +61 7 3365 4311
     Email: enquiries@dstc.edu.au

  This software is being provided "AS IS" without warranty of
  any kind, and DSTC Pty Ltd disclaims all warranties.


  Project:  Hector
  File:     $Source$

  Description:
            Report properties of X Server and Keyboard.

****************************************************************/

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#define DISPLAY "DISPLAY"



void usage(char *argv0) {
  fprintf(stderr, "Usage: %s -x|-y|-z|-m|-r|-v|-k|-h\n", argv0);
  fprintf(stderr, "where\n");
  fprintf(stderr, "       -x horizontal resolution\n");
  fprintf(stderr, "       -y vertical resolution\n");
  fprintf(stderr, "       -z colour depth (bits/pixel)\n");
  fprintf(stderr, "       -m X server manufacturer\n");
  fprintf(stderr, "       -r X server manufacturer release number\n");
  fprintf(stderr, "       -v version of X protocol\n");
  fprintf(stderr, "       -k keyboard type string. NOTE: must be run\n");
  fprintf(stderr, "          before altering mappings with xmodmap!\n");
  fprintf(stderr, "       -h display this message\n");
  fprintf(stderr, "\n");
  exit(1);
}

void keyboard_guess(Display *display) {
  XKeyEvent k;
  KeySym key;

  /*-- create a dummy KeyEvent to be used to test KeyCode -> KeySym mappings */

  k.type = KeyPress;
  k.serial = 1;
  k.send_event = 0;
  k.display = display;
  k.window = NULL;
  k.root = NULL;
  k.subwindow = NULL;
  k.time = 1;
  k.x = 1;
  k.y = 1;
  k.x_root = 1;
  k.y_root = 1;
  k.state = 0x0;
  k.same_screen = 1;

  /*-- these tests check the default mappings.  their order is significant. */

  k.keycode = 88;  /* Control_R on a PC-style keyboard */
  key = XLookupKeysym(&k, 0);
  if ((int)key == 0xffe4) {
    /* standard PC layout, various models ... */

    k.keycode = 9;  /* F1 on a DEC PCXAL*/
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffbe) {
      printf("dec-pc-pcxal\n");  /* DEC AlphaStation 250 4/266 */
      return;
    }

    k.keycode = 16;  /* F1 on a standard PC */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffbe) {
      printf("unknown-pc-101\n");  /* Honeywell, DEC PC7XL, NCD N-101M */
      return;
    }
  }

  k.keycode = 120;  /* Compose_R on an NCD DEC-style keyboard */
                    /* 'F1' on an IBM PC-style */
  key = XLookupKeysym(&k, 0);
  if ((int)key == 0xff20) {
    printf("ncd-dec-n108lk\n");         /* vindaloo */
    return;
  }
  if ((int)key == 0xffbe) {
    printf("ibm-pc-5168572M\n");        /* IBM RS/6000 C10 */
    return;
  }

  k.keycode = 173;  /* Compose_R on a DEC LK401 */
  key = XLookupKeysym(&k, 0);
  if ((int)key == 0xff20) {
    printf("dec-dec-lk401\n");          /* DEC 3000/400 */
    return;
  }

  k.keycode = 194;  /* 'a' on a DEC LK201 */
  key = XLookupKeysym(&k, 0);
  if ((int)key == 0x61) {
    printf("dec-dec-lk201\n");          /* DECstation 3100 */
    return;
  }

  /*-- give up and go home ... */

  printf("unknown\n");
  fprintf(stderr, "-k option unable to determine keyboard type.\n");
  exit(1);
}


void main(int argc, char *argv[]) {
  Display  *display;
  char     *display_name;
  char     *optarg = NULL;
  int      optind = 0;
  int      opterr = 0;
  int      optopt = 0;
  int      c = 0;

  if (argc != 2) {
    usage(argv[0]);
  }

  /*-- check that we have a DISPLAY environment variable */

  if (!(display_name = getenv(DISPLAY))) {
    fprintf(stderr, "Cannot locate display - DISPLAY variable not set.\n\n");
    exit(1);
  }

  /*-- check that we have access to the display */

  if (!(display = XOpenDisplay(display_name))) {
    fprintf(stderr, "Cannot open display from DISPLAY variable.\n\n");
    exit(1);
  }
  
  /*-- parse arguments and decide what to do */

  if ((c = getopt(argc, argv, "xyzmrvkh")) != -1) {
    switch (c) {
    case 'x':
      printf("%d\n", DisplayWidth(display,0));
      break;
    case 'y':
      printf("%d\n", DisplayHeight(display,0));
      break;
    case 'z':
      printf("%d\n", DefaultDepth(display,0));
      break;
    case 'm':
      printf("%s\n", ServerVendor(display));
      break;
    case 'r':
      printf("%d\n", VendorRelease(display));
      break;
    case 'v':
      printf("%d.%d\n", ProtocolVersion(display), ProtocolRevision(display));
      break;
    case 'k':
      keyboard_guess(display);
      break;
    default:
      usage(argv[0]);
    }
  } else {
    usage(argv[0]);
  }

  exit(0);
}


/***************************************************************/
/* end of xguess.c */

