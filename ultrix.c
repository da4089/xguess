/***************************************************************
 *
 *             xguess
 *             X implementation attribute testing
 *
 * File:        $Source$
 * Version:     $RCSfile$ $Revision$
 * Copyright:   (C) 1995, 1997, 1999-2000 David Arnold.
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


static char *xguess_ultrix_dummy = "warning prevention";

#if defined(__ultrix) || defined(ultrix)	/* whole file */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/devio.h>


#define BUFFER    255


char *local_guess(void) {

  struct devget  dev_info;
  int error, fd;

  fd = open("/dev/console", "r");

  error = ioctl(fd, DEVIOGET, &dev_info);

  printf("Category %d\n", dev_info.category);
  printf("Interface %s\n", dev_info.interface);
  printf("Device %s\n", dev_info.device);
  printf("Name %s\n", dev_info.dev_name);

  return NULL;
}


#ifdef DEBUG

void main (void) {
  printf ("%s\n", local_guess());
  exit(0);
}

#endif /* DEBUG */


#endif /* __ultrix || ultrix */


/***************************************************************/
/* end of ultrix.c */

