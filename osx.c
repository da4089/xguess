/***************************************************************
 * xguess
 * X implementation attribute testing
 *
 * Copyright (C) 2007-2014, David Arnold.
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
static const char rcsid[] = "@(#)$RCSfile: osf1.c,v $ $Revision: 1.2 $";
#endif


#if defined(__APPLE__)	/* whole file */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *xguess_from_apple(void) {

    /* FIXME */

    fprintf(stderr, "+ server has Apple-WM extension\n");
    return "apple-apple-unknown";
}


#endif /* __APPLE__ */


/***************************************************************/
/* end of osx.c */

