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
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


#if defined(__sun)
extern int gethostname(char *name, int namelen);
#endif


/****************************************************************/

/*-- DISPLAY environment variable name */
#define DISPLAY "DISPLAY"

/*-- vendor ID strings */
#define DECWINDOWS "DECWINDOWS"
#define NCD        "Network Computing Devices Inc."
#define OSF1_3_2C  "DECWINDOWS Digital Equipment Corporation Digital UNIX V3.2C"
#define OSF1_4_0   "DECWINDOWS Digital Equipment Corporation Digital UNIX V4.0"
#define SUNOS_5_5  "X11/NeWS - Sun Microsystems Inc."
#define ULTRIX_4_4 "DECWINDOWS Digital Equipment Corporation UWS V4.4"
#define VXT2000    "DECWINDOWS DigitalEquipmentCorp. / VXT 2000"
#define X_R4       "MIT X Consortium"
#define X_R5       "MIT X Consortium"
#define X_R6       "X Consortium"

/*-- root window property names */
#define NCD_KB_PROPERTY "_NCD_KEYBOARD_TYPE"


/****************************************************************/

#if !defined(__sun) && !defined(__alpha)

char *local_guess(void) {
  return (char *)NULL;
}

#else

extern char  *local_guess(void);

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

  This function returns a string with the following format:

  layout-vendor-model

  where layout is a general indication of the keyboard style and has
  one of these values

  lk201   - a DEC LK201 (single Control, single Compose, no Alt or Meta)
  lk401   - a DEC LK401 (single Control, two Compose, two Alt)
  pc101   - generic PC layout
  type4   - a Sun Type4 layout
  type5   - a Sun Type5 layout
  unknown - oops!  no idea, sorry.

  the vendor string is not really that important, except where very
  minor differences are present.

  dec     - Digital Equipment Corporation
  ibm     - Internation Business Machines
  ncd     - Network Computing Devices
  sun     - Sun Microsystems
  unknown - no way to tell.  this is normally the case with Linux, FreeBSD
            and other PC-based Unices.

  finally, there is a model number.  quite often this is just a repeat
  of the general type layout, although it can have a national language
  type suffix which is very important!

  these aren't itemised, since there are lots of them and i'm too lazy.

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
  k.window = (Window)NULL;
  k.root = (Window)NULL;
  k.subwindow = (Window)NULL;
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

       NCD, helpful people that they are (thanks Ken Johnson!) put the
       keyboard type as a property on the root window.
    */
    Atom ncd_atom, actual_type_return;
    int  res, actual_format_return;
    unsigned long nitems_return, bytes_after_return;
    unsigned char *buf;

    /* get the atom number for the NCD keyboard property */
    ncd_atom = XInternAtom(display, NCD_KB_PROPERTY, 1); /* don't create */

    if (ncd_atom != None) {
      res = XGetWindowProperty(display,
			       DefaultRootWindow(display),
			       ncd_atom,
			       0L,    /* offset 0 is start */
			       10L,   /* number of 32bit words to return */
			       False,    /* don't delete it! */
			       XA_STRING,
			       &actual_type_return,
			       &actual_format_return,
			       &nitems_return,
			       &bytes_after_return,
			       &buf); /* the actual value! */

      if (res == Success) {
	if (strcmp(buf, "N-108 US") == 0) {
	  printf("lk401-ncd-n108us\n");
	  return;

	} else if (strcmp(buf, "N-108 DE") == 0) {
	  printf("lk401-ncd-n108de\n");
	  return;

	} else if (strcmp(buf, "N-101") == 0) {
	  printf("pc101-ncd-n101\n");
	  return;

	} else if (strcmp(buf, "N-97") == 0) {
	  printf("type4-ncd-n97\n");
	  return;

	} else if (strcmp(buf, "N-102 US") == 0) {
	  printf("pc101-ncd-n102us\n");
	  return;

	} else if (strcmp(buf, "N-107") == 0) {
	  printf("type5-ncd-n107\n");
	  return;

	} else if (strcmp(buf, "VT-220") == 0) {
	  printf("lk401-ncd-vt220\n");
	  return;

	} else if (strcmp(buf, "123UX") == 0) {
	  printf("unknown-ncd-123ux\n");
	  return;

	} else if (strcmp(buf, "PC-Xview") == 0) {
	  printf("pc101-ncd-pcxview\n");
	  return;
	}
      }
    }

    /* no idea, other than NCD ... */
    printf("unknown-ncd-unknown\n");
    return;

    
  } else if (strncmp(vendor, "DECWINDOWS", 10) == 0) {
    /* something with a Digital X Server */

    k.keycode = 88;  /* Control_R on a PC-style keyboard */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffe4) {
      /* standard PC layout, various models ... */
  
      k.keycode = 9;  /* F1 on a DEC PCXAL*/
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        printf("pc101-dec-pcxal\n");  /* eg. DEC AlphaStation 250 4/266 */
        return;
      }
  
      k.keycode = 16;  /* F1 on a standard PC */
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        printf("pc101-unknown-pc101\n");  /* Honeywell,DEC PC7XL */
        return;
      }
    }
  
    k.keycode = 173;  /* Compose_R on a DEC LK401 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xff20) {
      printf("lk401-dec-lk401\n");          /* eg. DEC 3000/400 */
      return;
    }
  
    k.keycode = 194;  /* 'a' on a DEC LK201 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0x61) {
      printf("lk201-dec-lk201\n");          /* eg. DECstation 3100 */
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
    printf("pc101-ibm-5168572M\n");        /* IBM RS/6000 C10 */
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
  char     *optarg;
  int      optind;
  int      opterr;
  int      optopt;
  int      c = 0;

  /*-- assign to vars to prevent 'unused variable' errors */
  optarg = NULL;
  optind = 0;
  opterr = 0;
  optopt = 0;

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

