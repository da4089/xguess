/***************************************************************
 Copyright (C) DSTC Pty Ltd (ACN 052 372 577) 1995, 1997.
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

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


/****************************************************************/

#define DISPLAY "DISPLAY"


/*-- vendor ID strings */

#define DECWINDOWS "DECWINDOWS"
#define NCD        "Network Computing Devices Inc."
#define OSF1_3_2C  "DECWINDOWS Digital Equipment Corporation Digital UNIX V3.2C"
#define SUNOS_5_5  "X11/NeWS - Sun Microsystems Inc."
#define ULTRIX_4_4 "DECWINDOWS Digital Equipment Corporation UWS V4.4"
#define VXT2000    "DECWINDOWS DigitalEquipmentCorp. / VXT 2000"
#define X_R4       "MIT X Consortium"
#define X_R5       "MIT X Consortium"
#define X_R6       "X Consortium"


/****************************************************************/

#if !defined(__sun) && !defined(__alpha)

#include <stdio.h>

char *local_guess(void) {
  return (char *)NULL;
}

#endif


/****************************************************************/

int is_local_server(Display *display) {
  int   is_local = 1;
  char  displayname[255], localname[255], tmpname[255];
  struct hostent *he_display, *he_local;

  char  *dpy  = DisplayString(display);
  char  *tail = (char *) strchr(dpy, ':');
  
  if (tail == NULL || strncmp(tail, ":0", 2) != 0) {
    is_local = 0;     /*-- no display number or number isn't ":0" */

  } else {
    /*-- get hostname from display */
    strncpy(displayname, dpy, tail - dpy);
    displayname[tail - dpy] = 0;

    if (displayname[0] == '\000' ||
	strcmp(displayname, "unix") == 0 ||
	strcmp(displayname, "localhost") == 0) {
      is_local = 1;   /*-- the display host is empty, or local! */

    } else if (gethostname(localname, sizeof(localname)) == 1) {
      is_local = 0;   /*-- can't find hostname!? */

    } else {
      /*
	call gethostbyname() on the result of gethostname() because
	on some systems one is a FQDN and the other is not.
       */

      he_display = gethostbyname(displayname);
      if (he_display == NULL) {
	is_local = 0; /*-- gethostbyname failed */

      } else {
	strcpy (tmpname, he_display->h_name);
	he_local = gethostbyname (localname);
	
	if (he_local == NULL || strcmp(tmpname, he_local->h_name) != 0) {
	  is_local = 0;
	}
      }
    }
  }

  return is_local;
}


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

/*
  Attempt to determine what sort of keyboard is attached to the Display.

  We use a combination of methods:
  1) check whether the display is local.  If so, we can possibly
     probe the keyboard hardware directly to determine what type
     it is.

     We use conditional compilation to replace a
     platform-specific function which tests the keyboard
     hardware.  A default routine returns no result.

  2) check the manufacturer string of the X server.  Known values are

     This gives us some idea of what keyboards might be attached,
     and in particular, what keymaps might be in use so that we
     can test the default KeyCode->KeySym mappings.

  3) If no other info is about, then start testing for KeySyms
     and KeyCode mappings.  This is flawed, since any mapping
     could be set by default, but it often works ...

  Finally we try to pick a generic type of keyboard (PC, LK401,
  Type5) by the present of certain KeySyms.  This will at least
  let you set up some default mappings.
 */

void keyboard_guess(Display *display) {
  XKeyEvent k;
  KeySym    key;
  char      *res, *vendor;

  /*-- is hostname same as display host? */
  if (is_local_server(display)) {
    res = (char *)local_guess();
    if (res != NULL) {
      printf("%s\n", res);
      return;
    }
  }

  /*-- dummy KeyEvent to test KeyCode->KeySym mappings */

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

  /*-- get X Server manufacturer string */
  vendor = ServerVendor(display);

  if (strcmp(vendor, "MIT X Consortium") == 0) {
    /* vanilla R4 or R5 */
  } else if (strcmp(vendor, "X Consortium") == 0) {
    /* vanilla R6 */

  } else if (strcmp(vendor, NCD) == 0) {
    /* an NCD XTerminal

       NCD has a range of keyboard types, which are reported by their
       setup utility.  They fall into general categories ...

       n97 n101 n102 pc-xview - PC layout
       n107 123ux             - Sun Type5 layout
       n108us or n108de       - Digital LK401 layout (US and German models)
       vt220                  - Digital LK201 layout

       Detection is hard, since the keycode mapping is normally
       supplied by the X client which can ignore the fact that it's
       not one of their displays on the end, and map things all over
       the place.  For example, hosting a NCD off a Digital Unix box
       will give you the impression that there is an LK401 atttached,
       regardless of what is really there ...

       I think the solution (here it comes ...) is to figure out a way
       to talk to the NCD-SETUP extension that is loaded into the X
       server on all NCDs.  I imagine that this has the ability to
       extract the same keyboard info that their setup app does, and
       *it* can tell you what keyboard is attached ...
    */

    /* no idea, other than NCD ... */
    printf("ncd-unknown-unknown\n");
    return;

    /* old stuff that doesn't work ... */
    k.keycode = 120;  /* Compose_R on an NCD DEC-style keyboard */
                      /* 'F1' on an IBM PC-style */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xff20) {
      printf("ncd-dec-n108\n");
      return;
    }

    k.keycode = 16;
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffbe) {
      printf("ncd-pc-n101\n");
      return;
    }

    
  } else if (strncmp(vendor, "DECWINDOWS", 10) == 0) {
    /* something with a Digital X Server */

    k.keycode = 88;  /* Control_R on a PC-style keyboard */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffe4) {
      /* standard PC layout, various models ... */
  
      k.keycode = 9;  /* F1 on a DEC PCXAL*/
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        printf("dec-pc-pcxal\n");  /* eg. DEC AlphaStation 250 4/266 */
        return;
      }
  
      k.keycode = 16;  /* F1 on a standard PC */
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        printf("unknown-pc-101\n");  /* Honeywell, DEC PC7XL, NCD N-101M */
        return;
      }
    }
  
    k.keycode = 173;  /* Compose_R on a DEC LK401 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xff20) {
      printf("dec-dec-lk401\n");          /* eg. DEC 3000/400 */
      return;
    }
  
    k.keycode = 194;  /* 'a' on a DEC LK201 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0x61) {
      printf("dec-dec-lk201\n");          /* eg. DECstation 3100 */
      return;
    }
  
  }

  /*-- no idea? try to decide generic type at least ...  */

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

  /*-- give up and go home ... */

  printf("unknown-unknown-unknown\n");
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


/***************************************************************

  Some sundry information about various X servers ...

  DEC VXT200 XTerminal
  --------------------
  version number:    11.0
  vendor string:    DECWINDOWS DigitalEquipmentCorp. / VXT 2000
  vendor release number:    2001
  maximum request size:  262140 bytes
  motion buffer size:  0
  bitmap unit, bit order, padding:    32, LSBFirst, 32
  image byte order:    LSBFirst
  keycode range:    minimum 85, maximum 252
  number of extensions:    6
      DEC-Server-Mgmt-Extension
      ServerManagementExtension
      D2DX Extensions
      DEC-XTRAP
      SHAPE
      MIT-SUNDRY-NONSTANDARD

  i don't know what the server management extensions, the D2DX
  extension, or the DEC-XTRAP extension do, but i assume that one of
  them has keyboard hardware query info in it ...


  NCD 88K 8-bit Colour XTerminal
  ------------------------------
  version number:    11.0
  vendor string:    Network Computing Devices Inc.
  vendor release number:    3002
  maximum request size:  65536 bytes
  motion buffer size:  0
  bitmap unit, bit order, padding:    32, MSBFirst, 32
  image byte order:    MSBFirst
  keycode range:    minimum 8, maximum 254
  focus:  window 0x380000d, revert to PointerRoot
  number of extensions:    6
      SHAPE
      XTEST
      MIT-SUNDRY-NONSTANDARD
      XIdle
      ServerManagementExtension
      NCD-SETUP



****************************************************************/
/* end of xguess.c */

