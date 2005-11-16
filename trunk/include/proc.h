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

int iADC  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iADD  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iAND  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iBIT  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iCALL (sState *sCPU, char *cRAM, uchar *cOpcode);
int iCCF  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iCP   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iCPL  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iDAA  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iDEC  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iDI   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iEI   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iHALT (sState *sCPU, char *cRAM, uchar *cOpcode);
int iINC  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iJP   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iJR   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iLD   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iLDD  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iLDI  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iNOP  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iOR   (sState *sCPU, char *cRAM, uchar *cOpcode);
int iPOP  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iPUSH (sState *sCPU, char *cRAM, uchar *cOpcode); 
int iRES  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iRET  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iRETI (sState *sCPU, char *cRAM, uchar *cOpcode);
int iRST  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iSBC  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iSCF  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iSET  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iSTOP (sState *sCPU, char *cRAM, uchar *cOpcode);
int iSUB  (sState *sCPU, char *cRAM, uchar *cOpcode);
int iXOR  (sState *sCPU, char *cRAM, uchar *cOpcode);

int iClock(sState *sCPU, char *cRAM);

#endif

/*)\
\(*/
