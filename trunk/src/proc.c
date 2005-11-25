/*
proc.c -- The gameboy CPU
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

int iFlag(sState *sCPU, int Z, int N, int H, int C) {
	if(Z!=2) 
		sCPU->F = (sCPU->F & 0x70) ^ (Z << 7);
	
	if(N!=2) 
		sCPU->F = (sCPU->F & 0xB0) ^ (N << 6);
	
	if(H!=2) 
		sCPU->F = (sCPU->F & 0xD0) ^ (H << 5);
	
	if(C!=2) 
		sCPU->F = (sCPU->F & 0xE0) ^ (C << 4);

	sCPU->F &= 0xF0;

	return 1;
}

int iADC  (sState *sCPU, uchar *cRAM); 


int iADD  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ = 2, iH = 0, iC = 0;
	int iOldA;
	
	switch(cRAM[iPC]) {
		case 0x81: // ADD	A, C
			iOldA = sCPU->A;
			sCPU->A += sCPU->C;
			iZ = 0;
			
			if(sCPU->A == 0)
				iZ = 1;
			if((sCPU->A >= 0x10) && (iOldA < 0x10))
				iH = 1;
			if(sCPU->A < iOldA)
				iC = 1;
					
			printf("ADD\tA, C\t(A->%02x C=%02x)\n",sCPU->A,sCPU->C);
	}

	iFlag(sCPU, iZ, 0, iH, iC);
	return 1;	
}

int iAND  (sState *sCPU, uchar *cRAM);
int iBIT  (sState *sCPU, uchar *cRAM);

int iCALL (sState *sCPU, uchar *cRAM){
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cHB=0, cLB=0;

	switch(cRAM[iPC]) {
		case 0xCD: //CALL	nnnn
			cHB = cRAM[iPC+2];
			cLB = cRAM[iPC+1];
			
			printf("CALL\t%02x%02x\n", cRAM[iPC+2], cRAM[iPC+1]);
			break;
	}

	sCPU->iPC=(cHB<<8)^cLB;

	// Push the calling address onto the stack
	cRAM[iSP-2] = (iPC>>8) & 255;
	cRAM[iSP-1] = iPC & 255;
	sCPU->iSP-=2;

	return 0;
}

int iCCF  (sState *sCPU, uchar *cRAM);
int iCP   (sState *sCPU, uchar *cRAM);
int iCPL  (sState *sCPU, uchar *cRAM);
int iDAA  (sState *sCPU, uchar *cRAM);
int iDEC  (sState *sCPU, uchar *cRAM);
int iDI   (sState *sCPU, uchar *cRAM);
int iEI   (sState *sCPU, uchar *cRAM);
int iHALT (sState *sCPU, uchar *cRAM);

int iINC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iH=0, iZ=0, iN =2, iBC;

	switch(cRAM[iPC]) {
		case 0x03: // INC	BC
			iBC = (sCPU->B << 8) ^ sCPU->C;
			iBC++;
			sCPU->B = (iBC >> 8) & 255;
			sCPU->C = iBC & 255;
			printf("INC\tBC\t(->%04x)\n", iBC);
			break;
		
		case 0x04: // INC	B
			sCPU->B++;
			
			if(sCPU->B == 0)
				iZ = 1;
			if(sCPU->B == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tB\t(B->%02x)\n", sCPU->B);
			break;
		
		case 0x2c: // INC	L
			sCPU->L++;
			
			if(sCPU->L == 0)
				iZ = 1;
			if(sCPU->L == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tL\t(L->%02x)\n", sCPU->L);
			break;
	}
	
	iFlag(sCPU, iZ, iN, iH, 2);
	return iRet;
	
}

int iJP   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet= 0;
	
	switch(cRAM[iPC]) {
		case 0xC3:
			sCPU->iPC = (cRAM[iPC+2] << 8) ^ cRAM[iPC+1];
			printf("JP\t%04x\n", sCPU->iPC);
			break;

		case 0xE9:
			sCPU->iPC = (sCPU->H << 8) ^ sCPU->L;
			printf("JP\t%04x\n", sCPU->iPC);
			break;
		
		default:
			iRet = 1;
	}
	return iRet;	
}

int iJR   (sState *sCPU, uchar *cRAM);

int iLD   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet= 1;
	
	switch(cRAM[iPC]) {
		case 0x01: // LD	BC, nnnn
			sCPU->B = cRAM[iPC+2];
			sCPU->C = cRAM[iPC+1];
			iRet = 3;
			printf("LD\tBC, %02x%02x\n", sCPU->B, sCPU->C);
			break;

		case 0x02: // LD	(BC), A
			cRAM[(sCPU->B) << 8 ^ sCPU->C] = sCPU->A;
			printf("LD\t(BC), A\t(BC->%02x)\n", sCPU->A);
			break;

		case 0x06: // LD	B, nn
			sCPU->B = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tB, %02x\n", sCPU->B);
			break;

		case 0x31: // LD	SP, nnnn
			sCPU->iSP = (cRAM[iPC+2] << 8) ^ cRAM[iPC+1];
			iRet = 3;
			printf("LD\tSP, %04x\n", sCPU->iSP);
			break;
		
		case 0x7e: // LD	A, (HL)
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LD\tA, (HL)\t(A->%02x)\n", sCPU->A);
			break;

	}

	return iRet;
}

int iLDD  (sState *sCPU, uchar *cRAM);

int iLDI  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 3;
	
	switch(cRAM[iPC]) {
		case 0x22: // LDI	(HL), A
			cRAM[(sCPU->H << 8) ^ sCPU->L] = sCPU->A;
			sCPU->A++;
			printf("LDI\t(HL), A\n");
	}

	return iRet;
};

int iNOP  (sState *sCPU, uchar *cRAM) {
	printf("NOP\n");
	return 1;
}
int iOR   (sState *sCPU, uchar *cRAM);

int iPOP  (sState *sCPU, uchar *cRAM) {
	return 1;
}

/*
int iPUSH (sState *sCPU, uchar *cRAM) {
	int iSP = sCPU->iSP;
	int iPC = sCPU->iPC;

	cRAM[iSP-2] = cRAM[iPC+2];
	cRAM[iSP-1] = cRAM[iPC+1];

	sCPU->iSP-=2;

	return 1;
}
*/

int iRES  (sState *sCPU, uchar *cRAM);

int iRET  (sState *sCPU, uchar *cRAM) {
	int iSP = sCPU->iSP;

	sCPU->iPC=(cRAM[iSP] << 8) ^ cRAM[iSP+1];
	sCPU->iSP+=2;

	printf("RET\n");
	return 1;
}

int iRETI (sState *sCPU, uchar *cRAM);

int iRST  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cJP=0;

	switch(cRAM[iPC]) {
		case 0xC7:
			cJP = 0x00;
			printf("RST\t00\n");
			break;

		case 0xCF:
			cJP = 0x08;
			printf("RST\t08\n");
			break;
		
		case 0xD7:
			cJP = 0x10;
			printf("RST\t10\n");
			break;
		
		case 0xDF:
			cJP = 0x18;
			printf("RST\t18\n");
			break;
		
		case 0xE7:
			cJP = 0x20;
			printf("RST\t20\n");
			break;
		
		case 0xEF:		
			cJP = 0x28;
			printf("RST\t28\n");
			break;
		
		case 0xF7:
			cJP = 0x30;
			printf("RST\t30\n");
			break;

		case 0xFF:
			cJP = 0x38;
			printf("RST\t38\n");
			break;
	}
	
	// Push the calling address onto the stack
	cRAM[iSP-2] = (iPC >> 8) & 255;
	cRAM[iSP-1] = iPC & 255;
			
	sCPU->iPC = cJP;
	sCPU->iSP-=2;
	
	return 0;
}

int iSBC  (sState *sCPU, uchar *cRAM);
int iSCF  (sState *sCPU, uchar *cRAM);
int iSET  (sState *sCPU, uchar *cRAM);
int iSTOP (sState *sCPU, uchar *cRAM);
int iSUB  (sState *sCPU, uchar *cRAM);
int iXOR  (sState *sCPU, uchar *cRAM);

int iClock(sState *sCPU, uchar *cRAM) {
	int iAdd=1, iPC=sCPU->iPC;
	
	printf("%04x\t",iPC);

	switch(cRAM[iPC]) {
		// ADD
		case 0x81: iAdd = iADD(sCPU, cRAM);
			break;

		// CALL
		case 0xCD: iAdd = iCALL(sCPU, cRAM);
			break;

		// DEC
		case 0x05: // DEC	B
			sCPU->B--;
			printf("DEC\tB\t(B->%02x)\n", sCPU->B);
			break;
	
		// INC
		case 0x03:
		case 0x04: 
		case 0x2c: iAdd = iINC(sCPU, cRAM);
			break;
		
		// JP
		case 0xC3: 
		case 0xE9: iAdd = iJP(sCPU, cRAM);
			break;

		// LD
		case 0x01:
		case 0x02:
		case 0x06:
		case 0x31:
		case 0x7e: iAdd = iLD(sCPU, cRAM);
			break;

		// LDI
		case 0x22: iAdd = iLDI(sCPU, cRAM);
			break;
		
		// NOP
		case 0x00: iAdd = iNOP(sCPU, cRAM);
			break;

		// RET
		case 0xC9: iAdd = iRET(sCPU, cRAM);
			break;

		// RST
		case 0xC7:
		case 0xCF:
		case 0xD7:
		case 0xDF:
		case 0xE7:
		case 0xEF:
		case 0xF7:		
		case 0xFF: iAdd = iRST(sCPU, cRAM);
			break;

		default: printf("???\t(%02x)\n", cRAM[iPC]);
			break;
	}			
	sCPU->iPC+=iAdd;

	return iAdd;
}

/*)\
\(*/
