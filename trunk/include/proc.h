/*
proc.h -- proc.c prototypes
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

#ifndef _PROC_H_
#define _PROC_H_

int iFlag (sState *sCPU, int Z, int N, int H, int C);

// These will be moved to opcodes.h "when it's done"
int iADC  (sState *sCPU, char *cRAM);
int iADD  (sState *sCPU, char *cRAM);
int iAND  (sState *sCPU, char *cRAM);
int iBIT  (sState *sCPU, char *cRAM);
int iCALL (sState *sCPU, char *cRAM);
int iCCF  (sState *sCPU, char *cRAM);
int iCP   (sState *sCPU, char *cRAM);
int iCPL  (sState *sCPU, char *cRAM);
int iDAA  (sState *sCPU, char *cRAM);
int iDEC  (sState *sCPU, char *cRAM);
int iHALT (sState *sCPU, char *cRAM);
int iINC  (sState *sCPU, char *cRAM);
int iJP   (sState *sCPU, char *cRAM);
int iJR   (sState *sCPU, char *cRAM);
int iLD   (sState *sCPU, char *cRAM);
int iOR   (sState *sCPU, char *cRAM);
int iRES  (sState *sCPU, char *cRAM);
int iRET  (sState *sCPU, char *cRAM);
int iRETI (sState *sCPU, char *cRAM);
int iSBC  (sState *sCPU, char *cRAM);
int iSCF  (sState *sCPU, char *cRAM);
int iSET  (sState *sCPU, char *cRAM);
int iSTOP (sState *sCPU, char *cRAM);
int iSUB  (sState *sCPU, char *cRAM);
int iXOR  (sState *sCPU, char *cRAM);

int iClock(sState *sCPU, char *cRAM);

#endif

/*)\
\(*/
