#ifndef XGUESS_OSF1_H
#define XGUESS_OSF1_H
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

#if defined(__osf1) || defined(osf1)

/* 
 * if running on OSF/1, we know how to test a local keyboard
 */
#define XGUESS_LOCAL     (1)


/*
 * our local_guess function uses /dev/kbd and the layout ioctl
 */

extern char *local_guess(void);


#endif /* __osf1 || osf */

/***************************************************************/
#endif  /* XGUESS_OSF1_H */

