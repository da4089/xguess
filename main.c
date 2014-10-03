/***************************************************************
 * xguess
 * X implementation attribute testing
 *
 * Copyright (C) 1995-2014, David Arnold.
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

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <getopt.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "config.h"

#include "osx.h"
#if defined(HAVE_X11_XKBLIB_H)
#  include "xkb.h"
#endif

#if defined(__sun)
extern int gethostname(char *name, int namelen);
#endif


/****************************************************************/

/*-- DISPLAY environment variable name */
#define DISPLAY "DISPLAY"

/*-- root window property names */
#define NCD_KB_PROPERTY "_NCD_KEYBOARD_TYPE"

/* X Keyboard Extension name string */
#define XKB_EXTENSION_NAME "XKEYBOARD"

/* Apple Window Manager extension name string */
#define APPLEWM_EXTENSION_NAME "Apple-WM"

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


static struct option long_options[] = 
{
  {"x-resolution", no_argument, NULL, 'x'},
  {"y-resolution", no_argument, NULL, 'y'},
  {"depth", no_argument, NULL, 'z'},
  {"manufacturer", no_argument, NULL, 'm'},
  {"num-screens", no_argument, NULL, 'n'},
  {"release", no_argument, NULL, 'r'},
  {"x-version", no_argument, NULL, 'v'},
  {"screen", required_argument, NULL, 's'},
  {"version", no_argument, NULL, 'V'},
  {"help", no_argument, NULL, 'h'},
  {0, 0, 0, 0}
};

static int screen_number = 0;


void usage(char *argv0) {
  fprintf(stderr, 
	  "Usage: %s [-s n] -x|-y|-z|-m|-n|-r|-v|-k|-h\n"
	  "where\n"
	  "       -x horizontal resolution\n"
	  "       -y vertical resolution\n"
	  "       -z colour depth (bits/pixel)\n"
	  "       -m X server manufacturer\n"
	  "       -n number of screens on X server\n"
	  "       -r X server manufacturer release number\n"
	  "       -s screen number to use for other operations\n"
	  "       -v version of X protocol\n"
	  "       -k keyboard type string. NOTE: must be run\n"
	  "          before altering mappings with xmodmap!\n"
	  "       -V version of this program\n"
	  "       -h display this message\n\n",
	  argv0);
  exit(1);
}


/****************************************************************
 *
 *  a local_guess() function is defined by these include files
 *  iff we know how to probe the console hardware for the 
 *  particular operating system.  otherwise, use the dummy
 *  function.
 */

#include "xkb.h"
#include "sunos.h"

#if ! defined(XGUESS_LOCAL)
/* define a dummy local_guess(), since we have no clue */
char *local_guess(void)
{
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


/*
 *  ncd_guess()
 *
 *  NCD has a range of keyboard types, which are reported by their
 *  setup utility.  They fall into general categories ...
 *  
 *  n97 n101 n102 pc-xview - PC layout
 *  n107 123ux             - Sun Type5 layout
 *  n108us or n108de       - Digital LK401 layout (US and German models)
 *  vt220                  - Digital LK201 layout
 *
 *  NCD, helpful people that they are (thanks Ken Johnson!) put the
 *  keyboard type as a property on the root window.
 */


char *ncd_guess(Display *display)
{
  Atom ncd_atom, actual_type_return;
  int  res, actual_format_return;
  unsigned long nitems_return, bytes_after_return;
  char *buf;
  char *type = NULL;

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
			     (unsigned char **)&buf); /* the actual value! */

    if (res == Success) {
      if (strcmp(buf, "N-108 US") == 0) {
	type = strdup("lk401-ncd-n108us");
      } else if (strcmp(buf, "N-108 DE") == 0) {
	type = strdup("lk401-ncd-n108de");
      } else if (strcmp(buf, "N-101") == 0) {
	type = strdup("pc101-ncd-n101");
      } else if (strcmp(buf, "N-102 US") == 0) {
	type = strdup("pc101-ncd-n102us");
      } else if (strcmp(buf, "N-97") == 0) {
	type = strdup("type4-ncd-n97");
      } else if (strcmp(buf, "N-107") == 0) {
	type = strdup("type5-ncd-n107");
      } else if (strcmp(buf, "VT-220") == 0) {
	type = strdup("lk401-ncd-vt220");
      } else if (strcmp(buf, "123UX") == 0) {
	type = strdup("unknown-ncd-123ux");
      } else if (strcmp(buf, "PC-Xview") == 0) {
	type = strdup("pc101-ncd-pcxview");
      } else if (strcmp(buf, "IBM PS/2") == 0) {
	type = strdup("unknown-ncd-ps2");
      } else {
  	type = strdup("unknown-ncd-");
  	strcat(type, buf);
      }
    }
  }
  return type;
}


/*
 *  keyboard_guess
 *
 *  Attempt to determine what sort of keyboard is attached to the Display.
 *
 *  We use a combination of methods:
 *  1) test whether the server supported the Keyboard Extension, and
 *     if so query its registered geometry information.  this works
 *     really well for XFree86, so long as you're using XKB.
 *
 *  2) check whether the display is local.  If so, we can possibly
 *     probe the keyboard hardware directly to determine what type
 *     it is.
 *
 *     We use conditional compilation to replace a
 *     platform-specific function which tests the keyboard
 *     hardware.  A default routine returns no result.
 *
 *  3) check the manufacturer string of the X server.
 *
 *     This gives us some idea of what keyboards might be attached,
 *     and in particular, what keymaps might be in use so that we
 *     can test the default KeyCode->KeySym mappings.
 *
 *  4) If no other info is about, then start testing for KeySyms
 *     and KeyCode mappings.  This is flawed, since any mapping
 *     could be set by default, but it often works ...
 *
 *  Finally we try to pick a generic type of keyboard (PC, LK401,
 *  Type5) by the presence of certain KeySyms.  This will at least
 *  let you set up some default mappings.
 *
 *  This function returns a string with the following format:
 *
 *  layout-vendor-model
 *
 *  where layout is a general indication of the keyboard style and has
 *  one of these values
 *
 *  lk201   - a DEC LK201 (single Control, single Compose, no Alt or Meta)
 *  lk401   - a DEC LK401 (single Control, two Compose, two Alt)
 *  pc101   - generic PC layout
 *  pc104   - later model PC layout (with menu and logo keys)
 *  type4   - a Sun Type4 layout
 *  type5   - a Sun Type5 layout
 *  unknown - oops!  no idea, sorry.
 *
 *  the vendor string is not really that important, except where very
 *  minor differences are present.
 *
 *  dec     - Digital Equipment Corporation
 *  ibm     - Internation Business Machines
 *  ncd     - Network Computing Devices
 *  sun     - Sun Microsystems
 *  xfree   - XFree86 
 *  unknown - no way to tell.  this is normally the case with Linux, FreeBSD
 *            and other PC-based Unices.
 *
 *  finally, there is a model number.  quite often this is just a repeat
 *  of the general type layout, although it can have a national language
 *  type suffix which is very important!
 *
 *  these aren't itemised, since there are lots of them and i'm too lazy.
 */

char *keyboard_guess(Display *display) {
  XKeyEvent k;
  KeySym    key;
  char      *res, *vendor;
  char      **extensions;
  char      **ext_name;
  int       n_extensions, i;

  extensions = XListExtensions(display, &n_extensions);
  if (extensions) {

    /* Does the server have the X Keyboard (XKB) extension */
    for (i=0, ext_name = extensions; i < n_extensions; i++) {
      if (strcmp(*ext_name, XKB_EXTENSION_NAME) == 0) {
	if ((res = xguess_from_xkb())) {
	  return res;
	}
      }
      ext_name++;
    }

    /* Does the server have the Apple Window Manager (Apple-WM) extension */
    for (i=0,ext_name = extensions; i < n_extensions; i++) {
      if (strcmp(*ext_name, APPLEWM_EXTENSION_NAME) == 0) {
        if ((res = xguess_from_apple())) {
          return res;
        }
      }
      ext_name++;
    }
  }

  /*-- is hostname same as display host? */
  if (is_local_server(display)) {
    res = (char *)local_guess();
    if (res != NULL) {
      return res;
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
    return ncd_guess(display);

  } else if (strncmp(vendor, "DECWINDOWS", 10) == 0) {
    /* something with a Digital X Server */

    k.keycode = 88;  /* Control_R on a PC-style keyboard */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xffe4) {
      /* standard PC layout, various models ... */
  
      k.keycode = 9;  /* F1 on a DEC PCXAL*/
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        return strdup("pc101-dec-pcxal");  /* eg. DEC AlphaStation 250 4/266 */
      }
  
      k.keycode = 16;  /* F1 on a standard PC */
      key = XLookupKeysym(&k, 0);
      if ((int)key == 0xffbe) {
        return strdup("pc101-unknown-pc101");  /* Honeywell,DEC PC7XL */
      }
    }
  
    k.keycode = 173;  /* Compose_R on a DEC LK401 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0xff20) {
      return strdup("lk401-dec-lk401");          /* eg. DEC 3000/400 */
    }
  
    k.keycode = 194;  /* 'a' on a DEC LK201 */
    key = XLookupKeysym(&k, 0);
    if ((int)key == 0x61) {
      return strdup("lk201-dec-lk201");          /* eg. DECstation 3100 */
    }
  }

  /*-- no idea? guesswork time ... */

  k.keycode = 120;  /* Compose_R on an NCD DEC-style keyboard */
                    /* 'F1' on an IBM PC-style */
  key = XLookupKeysym(&k, 0);
  if ((int)key == 0xff20) {
    return strdup("ncd-dec-n108lk");         /* vindaloo */
  }

  if ((int)key == 0xffbe) {
    return strdup("pc101-ibm-5168572M");        /* IBM RS/6000 C10 */
  }

  /*-- give up and go home ... */

  fprintf(stderr, "-k option unable to determine keyboard type.\n");
  return strdup("unknown-unknown-unknown");
}


/*
 *   main()
 */

int main(int argc, char *argv[]) {
  Display  *display;
  char     *display_name;
  int      c = 0;
  int      operation = 0;
  int      long_index = 0;

  /*-- parse arguments and decide what to do */
  while (operation == 0) {
    c = getopt_long(argc, argv, "xyzmnrs:vkhV", long_options, &long_index);
    switch (c) {
    case 's':
      screen_number = strtol(optarg, NULL, 10);
      break;
    case 'x':
    case 'y':
    case 'z':
    case 'm':
    case 'n':
    case 'r':
    case 'v':
    case 'k':
      operation = c;
      break;
    case 'h':
      usage(argv[0]);
      exit(0);
    case 'V':
      printf("%s\n", VERSION);
      exit(0);
    case -1:
      operation = -1;
      break;
    case '?':
      usage(argv[0]);
      exit(1);
    }
  }

  /* check whether operation is set yet */
  if (operation < 1) {
    usage(argv[0]);
    exit(1);
  }

  /* check that we have a DISPLAY environment variable */
  if (!(display_name = getenv(DISPLAY))) {
    fprintf(stderr, "Cannot locate display - DISPLAY variable not set.\n\n");
    exit(1);
  }

  /* check that we have access to the display */
  if (!(display = XOpenDisplay(display_name))) {
    fprintf(stderr, "Cannot open display from DISPLAY variable.\n\n");
    exit(1);
  }

  /* check screen number */
  if (screen_number >= ScreenCount(display)) {
    fprintf(stderr, "cannot use screen %d, only %d screen(s) available.\n", screen_number, ScreenCount(display));
    usage(argv[0]);
    exit(1);
  }

  switch (operation) {
  case 'x':
    printf("%d\n", DisplayWidth(display, screen_number));
    break;
  case 'y':
    printf("%d\n", DisplayHeight(display, screen_number));
    break;
  case 'z':
    printf("%d\n", DefaultDepth(display, screen_number));
    break;
  case 'm':
    printf("%s\n", ServerVendor(display));
    break;
  case 'n':
    printf("%d\n", ScreenCount(display));
    break;
  case 'r':
    printf("%d\n", VendorRelease(display));
    break;
  case 'v':
    printf("%d.%d\n", ProtocolVersion(display), ProtocolRevision(display));
    break;
  case 'k':
    printf("%s\n", keyboard_guess(display));
    break;
  }

  exit(0);
}


/***************************************************************/
/* end of main.c */

