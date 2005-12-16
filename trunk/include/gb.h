/*
gb.h -- Data structure definitions
Copyright (C) 2005  Martin Wolters

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the 

Free Software Foundation, Inc.
51 Franklin Street, Fifth Floor
Boston, MA  02110-1301
USA

You can contact the author via electronic mail by the address
'nc-wolterma4@netcologne.de'.
*/

#ifndef _GB_H_
#define _GB_H_

// Macros for flag operations
#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

#define SETFLAG(a) sCPU->F |= (a)
#define DELFLAG(a) sCPU->F &= ~(a)

typedef unsigned char uchar;

typedef struct {
	// Registers
	uchar A,F,B,C,D,E,H,L;

	// Program counter; stack pointer
	int iPC, iSP;

	// "Enable interrupt" register
	int iEI;
} sState;

#endif

/*)\
\(*/
