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


static char *xguess_osf1_dummy = "warning prevention";

#if defined(__alpha) || defined(alpha)	/* whole file */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>


#define BUFFER    255


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


#ifdef DEBUG

void main (void) {
  printf ("%s\n", local_guess());
  exit(0);
}

#endif /* DEBUG */


#endif /* __alpha */


/***************************************************************/
/* end of FILE_C */

