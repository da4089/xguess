/*
 *
 */

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif

#if defined(__linux) || defined(linux)	/* whole file */
#define XGUESS_KEYBOARD     (1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

static error_handler(Display *dpy, XErrorEvent err) {
  printf("got an error\n");
  printf("code = %d\n", err.error_code);
  return 1;
}


char *local_guess(void) {
  int major = XkbMajorVersion;
  int minor = XkbMinorVersion;
  int event, error, status;
  char *display_name = ":0.0";
  Display *display = NULL;
  XkbDescPtr kbd;
  char *keycodes, *geometry, *types, *symbols, *s;
  char buf[1023];

  printf("hello kitty\n");

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

  XSetErrorHandler(error_handler);

  kbd = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
  if (kbd == NULL) {
    fprintf(stderr, "get kbd died.\n");
    return NULL;
  }

  keycodes = XGetAtomName(display, kbd->names->keycodes);
  printf("keycodes: %s\n", keycodes);

  geometry = XGetAtomName(display, kbd->names->geometry);
  printf("geometry: %s\n", geometry);

  types = XGetAtomName(display, kbd->names->types);
  printf("types: %s\n", types);

  s = XGetAtomName(display, kbd->names->phys_symbols);
  printf("phys_symbols: %s\n", s);

  symbols = XGetAtomName(display, kbd->names->symbols);
  printf("symbols: %s\n", symbols);

  sprintf(buf, "%s-%s-%s", symbols, keycodes, symbols);
  return strdup(buf);
}


#ifdef DEBUG

int main (void) {
  printf ("%s\n", local_guess());
  exit(0);
}

#endif /* DEBUG */


#endif /* __linux || linux */


/***************************************************************/
/* end of xguess-linux.c */

