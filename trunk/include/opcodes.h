/*
opcodes.h -- opcodes.c prototypes
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

#ifndef _OPCODES_H_
#define _OPCODES_H_
#include "gb.h"

//int iADC  (sState *sCPU, uchar *cRAM);
//int iADD  (sState *sCPU, uchar *cRAM);
//int iAND  (sState *sCPU, uchar *cRAM);
//int iBIT  (sState *sCPU, uchar *cRAM);
//int iCALL (sState *sCPU, uchar *cRAM);
//int iCCF  (sState *sCPU, uchar *cRAM);
//int iCP   (sState *sCPU, uchar *cRAM);
//int iCPL  (sState *sCPU, uchar *cRAM);
//int iDAA  (sState *sCPU, uchar *cRAM);
//int iDEC  (sState *sCPU, uchar *cRAM);
int iDI   (sState *sCPU, uchar *cRAM);
int iEI   (sState *sCPU, uchar *cRAM);
//int iHALT (sState *sCPU, uchar *cRAM);
//int iINC  (sState *sCPU, uchar *cRAM);
//int iJP   (sState *sCPU, uchar *cRAM);
//int iJR   (sState *sCPU, uchar *cRAM);
//int iLD   (sState *sCPU, uchar *cRAM);
int iLDD  (sState *sCPU, uchar *cRAM);
int iLDI  (sState *sCPU, uchar *cRAM);
int iNOP  (sState *sCPU, uchar *cRAM);
int iOR   (sState *sCPU, uchar *cRAM);
int iPOP  (sState *sCPU, uchar *cRAM);
int iPUSH (sState *sCPU, uchar *cRAM); 
//int iRES  (sState *sCPU, uchar *cRAM);
//int iRET  (sState *sCPU, uchar *cRAM);
//int iRETI (sState *sCPU, uchar *cRAM);
int iRST  (sState *sCPU, uchar *cRAM);
//int iSBC  (sState *sCPU, uchar *cRAM);
//int iSCF  (sState *sCPU, uchar *cRAM);
//int iSET  (sState *sCPU, uchar *cRAM);
//int iSTOP (sState *sCPU, uchar *cRAM);
//int iSUB  (sState *sCPU, uchar *cRAM);
int iXOR  (sState *sCPU, uchar *cRAM);

#endif

/*)\
\(*/
