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

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif


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


#endif /* __alpha */


/***************************************************************/
/* end of FILE_C */

