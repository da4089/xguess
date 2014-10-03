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


#if defined(__alpha) || defined(alpha)	/* whole file */
#if defined(__osf1) || defined(osf)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>


#define BUFFER    (255)


char *local_guess(void) {

  char buf_sysinfo[BUFFER], buf_output[BUFFER], *layout, *model;
  int error;

  error = getsysinfo(GSI_KEYBOARD, (caddr_t)buf_sysinfo, 255, 0, 0);

  if (error == -1) {
    switch (errno) {
    case EFAULT:
      fprintf(stderr, "Error EFAULT when testing keyboard hardware.\n");
      return NULL;

    case EINVAL:
      fprintf(stderr, "Error EINVAL when testing keyboard hardware.\n");
      return NULL;

    case EPERM:
      fprintf(stderr, "Error EPERM when testing keyboard hardware.\n");
      return NULL;
    }
  } else {
    if (strcmp(buf_sysinfo, "PCXAL") == 0) {
      layout = "pc101";
      model  = "pcxal";
    } else if (strcmp(buf_sysinfo, "PC7XL") == 0) {
      layout = "pc101";
      model  = "pc7xl";
    } else  if (strcmp(buf_sysinfo, "LK401") == 0) {
      layout = "lk401";
      model  = "lk401";
    } else {
      layout = "unknown";
      model  = "unknown";
    }

    sprintf(buf_output, "%s-dec-%s", layout, model);
    return strdup(buf_output);
  }
}


#endif /* __osf || osf */
#endif /* __alpha || alpha*/


/***************************************************************/
/* end of osf1.c */

