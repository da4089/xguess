/***************************************************************

 Copyright (C) DSTC Pty Ltd (ACN 052 372 577) 1997.
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
     Australia
     Tel: +61 7 3365 4310
     Fax: +61 7 3365 4311
     Email: enquiries@dstc.edu.au

 This software is being provided "AS IS" without warranty of
 any kind.  In no event shall DSTC Pty Ltd be liable for
 damage of any kind arising out of or in connection with
 the use or performance of this software.

 Project:  Hector
 File:     $Source$

****************************************************************/

#if !defined(lint)
static const char rcsid[] = "@(#)$RCSfile$ $Revision$";
#endif


#if defined(__alpha) || defined(alpha)	/* whole file */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>


char *local_guess(void) {

  char buf1[255];
  int error;

  error = getsysinfo(GSI_KEYBOARD, (caddr_t)&buf1, 255, 0, 0);

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
    fprintf(stderr, ">%s<\n", buf1);
    return strdup(buf1);
  }
}


#ifdef DEBUG

void main (void) {
  printf ("%s", local_guess());
  exit(0);
}

#endif /* DEBUG */


#endif /* __alpha */


/***************************************************************/
/* end of FILE_C */

