/***************************************************************
 *
 *              xguess
 *              X implementation attribute testing
 *
 * File:        $Source$
 * Version:     $RCSfile$ $Revision$
 * Copyright:   (C) 2000-2003 David Arnold.
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


#include "config.h"

#if defined(HAVE_X11_XKBLIB_H) && defined(HAVE_X11_EXTENSIONS_XKBGEOM_H)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBgeom.h>

static error_handler(Display *dpy, XErrorEvent err) {
  printf("got an error\n");
  printf("code = %d\n", err.error_code);
  return 1;
}


char *xguess_from_xkb(void) {
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  int event, error, status;
  char *display_name = ":0.0";
  Display *display = NULL;
  XkbDescPtr kbd;
  char *keycodes, *geometry, *types, *symbols, *s, *c, *b;
  char buf[1023];

  /* check version, extension and open display */
  display = XkbOpenDisplay(display_name, &event, &error, &major,  &minor, &status);
  if (display == NULL) {
    switch (status) {
    case XkbOD_BadLibraryVersion:
      fprintf(stderr, "The version of the linked X11 library (%d.%d) is different to that used to compile this program (%d.%d).  Aborting.\n", major, minor, XkbMajorVersion, XkbMinorVersion);
      break;
    case XkbOD_ConnectionRefused:
      fprintf(stderr, "Cannot open display \"%s\".  Aborting.\n", display_name);
      break;
    case XkbOD_NonXkbServer:
      fprintf(stderr, "specified server does not support XKB extension.\n");
      break;
    case XkbOD_BadServerVersion:
      fprintf(stderr, "bad server version.\n");
      break;
    default:
      fprintf(stderr, "unknown error opening display, %d\n", status);
    }
    return NULL;
  }

  /* trap errors */
  XSetErrorHandler(error_handler);

  /* get keyboard description from server */
  kbd = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
  if (kbd == NULL) {
    fprintf(stderr, "XkbGetKeyboard failed..\n");
    return NULL;
  }

  /* look up attributes */
  geometry = XGetAtomName(display, kbd->geom->name);
  keycodes = XGetAtomName(display, kbd->names->keycodes);
  symbols = XGetAtomName(display, kbd->names->symbols);

  /* strips parens from geometry */
  c = geometry;
  b = &buf[0];
  while (*c != '(' && *c != '\0') {
    c++;
  }

  if (*c == '(') {
    c++;
    while (*c != ')' && *c != '\0') {
      *b++ = *c++;
    }
  }

  sprintf(b, "-%s-%s", keycodes, symbols);
  return strdup(buf);
}

#endif /* XKBlib && XKBgeom */

/***************************************************************/
/* end of xkb.c */

