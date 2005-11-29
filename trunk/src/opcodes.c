/*
opcodes.c -- The gameboy CPU opcodes
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

#include <stdio.h>
#include "gb.h"

int iDI   (sState *sCPU, uchar *cRAM) {
	sCPU->iEI = 0;
	printf("DI\t\t");

	return 1;
}

int iEI   (sState *sCPU, uchar *cRAM) {
	sCPU->iEI = 1;
	printf("EI\t\t");

	return 1;
}

int iLDD  (sState *sCPU, uchar *cRAM){
	int iPC = sCPU->iPC;
	int iRet = 1;
	
	switch(cRAM[iPC]) {
		case 0x32:
			cRAM[(sCPU->H << 8) ^ sCPU->L] = sCPU->A;
			printf("LDD\t$HL, A\t");
			break;
			
		case 0x3A:
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LDD\tA, $HL\t");
			break;
			
	}
	sCPU->L--;
	if(sCPU->L == 0xFF)
		sCPU->H--;

	return iRet;
}

int iLDI  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	
	switch(cRAM[iPC]) {
		case 0x22: // LDI	(HL), A
			cRAM[(sCPU->H << 8) ^ sCPU->L] = sCPU->A;
			printf("LDI\t$HL, A\t");
			break;
			
		case 0x2A: // LDI	A, (HL)
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LDI\tA, $HL\t");
			break;
			
	}
	sCPU->L++;
	if(sCPU->L == 0)
		sCPU->H++;

	return iRet;
};

int iNOP  (sState *sCPU, uchar *cRAM) {
	printf("NOP\t\t");
	return 1;
}

int iPOP  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;
	
	int iLVal = cRAM[iSP];
	int iHVal = cRAM[iSP+1];
	sCPU->iSP+=2;

	switch(cRAM[iPC]) {
		case 0xC1:
			sCPU->B = iHVal;
			sCPU->C = iLVal;
			printf("POP\tBC\t");
			break;
			
		case 0xD1:
			sCPU->D = iHVal;
			sCPU->E = iLVal;
			printf("POP\tDE\t");
			break;
			
		case 0xE1:
			sCPU->H = iHVal;
			sCPU->L = iLVal;
			printf("POP\tHL\t");
			break;
			
		case 0xF1:
			sCPU->A = iHVal;
			sCPU->F = iLVal;
			printf("POP\tAF\t");
			break;
	}
	
	return 1;
}

int iPUSH (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;
	uchar iHVal = 0, iLVal = 0;

	switch(cRAM[iPC]) {
		case 0xC5:
			iHVal = sCPU->B;
			iLVal = sCPU->C;
			printf("PUSH\tBC\t");
			break;
			
		case 0xD5:
			iHVal = sCPU->D;
			iLVal = sCPU->E;
			printf("PUSH\tDE\t");
			break;
			
		case 0xE5:
			iHVal = sCPU->H;
			iLVal = sCPU->L;
			printf("PUSH\tHL\t");
			break;
			
		case 0xF5:
			iHVal = sCPU->A;
			iLVal = sCPU->F;
			printf("PUSH\tAF\t");
			break;
			
	}
	
	
	cRAM[iSP-1] = iHVal;
	cRAM[iSP-2] = iLVal;

	sCPU->iSP-=2;

	return 1;
}

int iRST  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cJP=0;

	switch(cRAM[iPC]) {
		case 0xC7:
			cJP = 0x00;
			printf("RST\t00\t");
			break;

		case 0xCF:
			cJP = 0x08;
			printf("RST\t08\t");
			break;
		
		case 0xD7:
			cJP = 0x10;
			printf("RST\t10\t");
			break;
		
		case 0xDF:
			cJP = 0x18;
			printf("RST\t18\t");
			break;
		
		case 0xE7:
			cJP = 0x20;
			printf("RST\t20\t");
			break;
		
		case 0xEF:		
			cJP = 0x28;
			printf("RST\t28\t");
			break;
		
		case 0xF7:
			cJP = 0x30;
			printf("RST\t30\t");
			break;

		case 0xFF:
			cJP = 0x38;
			printf("RST\t38\t");
			break;
	}
	
	// Push the calling address onto the stack
	iPC+=1;
	cRAM[iSP-1] = (iPC >> 8) & 255;
	cRAM[iSP-2] = iPC & 255;
			
	sCPU->iPC = cJP;
	sCPU->iSP-=2;
	
	return 0;
}

/*)\
\(*/
