#############################################################################
#
#              xguess
#
# File:        $Source$
# Version:     $RCSfile$ $Revision$
# Copyright:   (C) 1998-2000, David Arnold.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#############################################################################
# Process this file with automake to produce Makefile.in

bin_PROGRAMS = xguess

# xcoffeebiff has many sources
xguess_SOURCES = \
	xguess-dummy.c \
	xguess-osf1.c \
	xguess-sunos.c \
	xguess-ultrix.c \
	xguess.c

man_MANS = xguess.1

EXTRA_DIST = $(man_MANS)


#############################################################################
