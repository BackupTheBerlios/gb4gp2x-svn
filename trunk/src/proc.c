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
#include "opcodes.h"
#include "gb.h"

// These opcodes are not yet fully implemented.
// Will be moved to opcodes.c when ready.
int iADC  (sState *sCPU, uchar *cRAM); 

// Arithmetic addition operation
int iADD  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iOldA, iOldB;
	int iBuf, iZ = 0, iC = 0, iH = 0;
	
	switch(cRAM[iPC]) {
		// ADD	HL, DE
		case 0x19:
			iOldA = sCPU->H;
			iOldB = sCPU->L;
			
			iBuf = ((sCPU->H << 8) ^ sCPU->L) + ((sCPU->D << 8) ^ sCPU->E);
			if(iBuf > 0xFFFF)
				iC = FLAG_C;
			if((iBuf & 0xFFFF) == 0)
				iZ = FLAG_Z;
			
			sCPU->H = (iBuf >> 8) & 255;
			sCPU->L = iBuf & 255;

			printf("ADD\tHL, DE\t");
			break;
			
		// ADD	A, C
		case 0x81:
			iOldA = sCPU->A;
			sCPU->A += sCPU->C;
		
			if(sCPU->A == 0) {
				iZ = FLAG_Z;
			} else {
				if(sCPU->A < iOldA)
					iC = FLAG_C;
				if((sCPU->A >= 0x10) && (iOldA < 0x10))
						iH = FLAG_H;
			}
			
			printf("ADD\tA, C\t");
			break;

		// ADD	A, A
		case 0x87:
			iOldA = sCPU->A;
			sCPU->A *= 2;
			
			if(sCPU->A == 0) {
				iZ = FLAG_Z;
			} else {
				if(sCPU->A < iOldA)
					iC = FLAG_C;
				if((sCPU->A >= 0x10) && (iOldA < 0x10))
						iH = FLAG_H;
			}
			
			printf("ADD\tA, A\t");
			break;
	}
	
	// Edit Flags
	DELFLAG(FLAG_Z | FLAG_N | FLAG_H | FLAG_C);
	SETFLAG(iZ | iC | iH);
					
	return 1;	
}

// Bitwise AND operation
int iAND  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ = 0;

	switch(cRAM[iPC]) {
		case 0xA1:
			sCPU->A &= sCPU->C;
			if(sCPU->A == 0)
				iZ=FLAG_Z;
			
			printf("AND\tC\t");
			break;

		// AND 	nn
		case 0xE6:
			sCPU->A &= cRAM[iPC+1];
			if(sCPU->A == 0)
				iZ=FLAG_Z;
			
			printf("AND\t%02x\t", cRAM[iPC+1]);
			break;
	}

	DELFLAG(FLAG_Z | FLAG_N | FLAG_C);
	SETFLAG(iZ | FLAG_H);
	return 2;
}
int iBIT  (sState *sCPU, uchar *cRAM);

// Call a subroutine
int iCALL (sState *sCPU, uchar *cRAM){
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cHB=0, cLB=0;

	switch(cRAM[iPC]) {
		// CALL	nnnn
		case 0xCD: 
			cHB = cRAM[iPC+2];
			cLB = cRAM[iPC+1];
			
			printf("CALL\t%02x%02x\t", cRAM[iPC+2], cRAM[iPC+1]);
			break;
	}

	sCPU->iPC=(cHB<<8)^cLB;
	iPC+=3;

	// Push the calling address onto the stack
	cRAM[iSP-1] = (iPC>>8) & 255;
	cRAM[iSP-2] = iPC & 255;
	sCPU->iSP-=2;

	return 0;
}

int iCCF  (sState *sCPU, uchar *cRAM);

// compare value with register A
int iCP   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	uchar uCP = 0;

	switch(cRAM[iPC]) {
		// CP	B
		case 0xB8: 
			uCP = sCPU->B;
			printf("CP\tB\t");
			break;

		// CP 	C
		case 0xB9: 
			uCP = sCPU->C;
			printf("CP\tC\t");
			break;

		// CP	D
		case 0xBA: 
			uCP = sCPU->D;
			printf("CP\tD\t");
			break;

		// CP	E
		case 0xBB: 
			uCP = sCPU->E;
			printf("CP\tE\t");
			break;

		// CP	H
		case 0xBC: 
			uCP = sCPU->H;
			printf("CP\tH\t");
			break;

		// CP	L
		case 0xBD: 
			uCP = sCPU->L;
			printf("CP\tL\t");
			break;

		// CP	A
		case 0xBF: 
			uCP = sCPU->A;
			printf("CP\tA\t");
			break;

		// CP	nn
		case 0xFE:
			uCP = cRAM[iPC+1];;
			printf("CP\t%02x\t", uCP);
			iRet=2;
			break;
	}

	// Edit Flags
	DELFLAG(FLAG_Z | FLAG_H | FLAG_C);
	SETFLAG(FLAG_N);
	
	if(sCPU->A == uCP) 
		SETFLAG(FLAG_Z);

	if(sCPU->A < uCP)
		SETFLAG(FLAG_C);

	return iRet;
}

int iCPL  (sState *sCPU, uchar *cRAM) {
	sCPU->A ^= 255;
	printf("CPL\t\t");
	
	SETFLAG(FLAG_N | FLAG_H);

	return 1;
}

int iDAA  (sState *sCPU, uchar *cRAM);

// Decrease value
int iDEC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iZ=0, iN = FLAG_N, iH=0;

	switch(cRAM[iPC]) {
		// DEC	B
		case 0x05:
			sCPU->B--;
			if(sCPU->B == 0)
				iZ = FLAG_Z;
			if((sCPU->B & 0xF ) == 0xF)
				iH = FLAG_H;
			
			printf("DEC\tB\t");
			break;
	
		// DEC	BC
		case 0x0B:
			sCPU->C--;
			if(sCPU->C == 0xFF)
				sCPU->B--;
			iZ = sCPU->F & FLAG_Z;
			iN = sCPU->F & FLAG_N;
			iH = sCPU->F & FLAG_H;
			printf("DEC\tBC\t");
			break;
		
		// DEC	C
		case 0x0D:
			sCPU->C--;
			if(sCPU->C == 0)
				iZ = FLAG_Z;
			if((sCPU->C & 0xF) == 0xF)
				iH = FLAG_H;
			
			printf("DEC\tC\t");
			break;
	
		// DEC	A
		case 0x3D:
			sCPU->A--;
			if(sCPU->A == 0)
				iZ = FLAG_Z;
			if((sCPU->A & 0xF) == 0xF)
				iH = FLAG_H;
			
			printf("DEC\tA\t");
			break;
	}

	// Edit Flags
	DELFLAG(FLAG_Z | FLAG_N | FLAG_H);
	SETFLAG(iZ | iN | iH);
	
	return iRet;
}

int iHALT (sState *sCPU, uchar *cRAM);

// Increase value
int iINC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iH=0, iZ=0, iN =0;

	switch(cRAM[iPC]) {
		// INC	BC
		case 0x03:
			sCPU->C++;
			if(sCPU->C == 0)
				sCPU->B++;
			printf("INC\tBC\t");
			break;
	/*	
		// INC	BC
		case 0x03:
			iBC = (sCPU->B << 8) ^ sCPU->C;
			iBC++;
			sCPU->B = (iBC >> 8) & 255;
			sCPU->C = iBC & 255;
			printf("INC\tBC\t");
			break;
	*/	
		// INC	B
		case 0x04:
			sCPU->B++;
			
			if(sCPU->B == 0)
				iZ = FLAG_Z;
			if(sCPU->B == 0x10)
				iH = FLAG_H;
			iN = 0;
			
			printf("INC\tB\t");
			break;
		
		// INC	C
		case 0x0C:
			sCPU->C++;
			
			if(sCPU->C == 0)
				iZ = FLAG_Z;
			if(sCPU->C == 0x10)
				iH = FLAG_H;
			iN = 0;
			
			printf("INC\tC\t");
			break;
		
		// INC	HL
		case 0x23:
			sCPU->L++;
			if(sCPU->L == 0)
				sCPU->H++;
			printf("INC\tHL\t");
			break;
		
		// INC	L
		case 0x2c:
			sCPU->L++;
			
			if(sCPU->L == 0)
				iZ = FLAG_Z;
			if(sCPU->L == 0x10)
				iH = FLAG_H;
			iN = 0;
			
			printf("INC\tL\t");
			break;
	}
	
	// Edit Flags
	DELFLAG(FLAG_Z | FLAG_N | FLAG_H);
	SETFLAG(iZ | iN | iH);
	
	return iRet;
}

// Jump to a new address
int iJP   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet= 0;
	
	switch(cRAM[iPC]) {
		// JP	nnnn
		case 0xC3:
			sCPU->iPC = (cRAM[iPC+2] << 8) ^ cRAM[iPC+1];
			printf("JP\t%04x\t", sCPU->iPC);
			break;

		// JP	HL
		case 0xE9:
			sCPU->iPC = (sCPU->H << 8) ^ sCPU->L;
			printf("JP\t%04x\t", sCPU->iPC);
			break;
		
		default:
			iRet = 1;
	}
	return iRet;	
}

// Jump relative from current address
int iJR   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRel = cRAM[iPC+1];
	int iAdd = 2;

	switch(cRAM[iPC]) {
		// JR	nn
		case 0x18:
			iRel += 1; // opcode is 1 byte long! 
			if((cRAM[iPC+1] & 0x80) == 0x80)
				iRel -= 256;
				
			sCPU->iPC += iRel; 
			iAdd = 0;
			printf("JR\t%02x\t", cRAM[iPC+1]);
			break;
		
		// JR	NZ, nn
		case 0x20:
			iRel +=2; // opcode is 2 bytes long!
			if((sCPU->F & 0x80) == 0) {
				if((cRAM[iPC+1] & 0x80) == 0x80)
					iRel -= 256;
				
				sCPU->iPC += iRel; 
				iAdd = 0;
			}
			printf("JR\tNZ, %02x\t", cRAM[iPC+1]);
			break;
	}

	return iAdd;
}

// Load a value into a register
int iLD   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet= 1;
	
	switch(cRAM[iPC]) {
		// LD	BC, nnnn
		case 0x01:
			sCPU->B = cRAM[iPC+2];
			sCPU->C = cRAM[iPC+1];
			iRet = 3;
			printf("LD\tBC, %02x%02x", sCPU->B, sCPU->C);
			break;

		// LD	(BC), A
		case 0x02:
			cRAM[(sCPU->B) << 8 ^ sCPU->C] = sCPU->A;
			printf("LD\t$BC, A\t");
			break;

		// LD	B, nn
		case 0x06:
			sCPU->B = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tB, %02x\t", sCPU->B);
			break;
		
		// LD	C, nn
		case 0x0e:
			sCPU->C = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tC, %02x\t", sCPU->C);
			break;
			
		// LD	D, nn	
		case 0x16:
			sCPU->D = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tD, %02x\t", sCPU->D);
			break;

		// LD	E, nn	
		case 0x1E:
			sCPU->E = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tE, %02x\t", sCPU->E);
			break;

		// LD	HL, nnnn
		case 0x21: 
			sCPU->H = cRAM[iPC+2];
			sCPU->L = cRAM[iPC+1];
			iRet = 3;
			printf("LD\tHL, %02x%02x", sCPU->H, sCPU->L);
			break;

		// LD	H, nn
		case 0x26:
			sCPU->H = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tH, %02x\t", sCPU->H);
			break;

		// LD	L, nn
		case 0x2E: 
			sCPU->L = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tL, %02x\t", sCPU->L);
			break;

		// LD	SP, nnnn
		case 0x31: 
			sCPU->iSP = (cRAM[iPC+2] << 8) ^ cRAM[iPC+1];
			iRet = 3;
			printf("LD\tSP, %04x", sCPU->iSP);
			break;
		
		// LD	A, nn
		case 0x3E: 
			sCPU->A = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tA, %02x\t", sCPU->A);
			break;
			
		// LD	B, B (What's this good for anyway?)
		case 0x40:
			sCPU->B = sCPU->B;
			printf("LD\tB, B\t");
			break;

		// LD	B, A
		case 0x47:
			sCPU->B = sCPU->A;
			printf("LD\tB, A\t");
			break;

		// LD	C, A
		case 0x4F:
			sCPU->C = sCPU->A;
			printf("LD\tC, A\t");
			break;
			
		// LD	E, E (dto.)
		case 0x5B: 
			sCPU->E = sCPU->E;
			printf("LD\tE, E\t");
			break;

		// LD	D, (HL)
		case 0x56: 
			sCPU->D = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LD\tD, $HL\t");
			break;

		// LD	E, (HL)
		case 0x5E: 
			sCPU->E = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LD\tE, $HL\t");
			break;

		// LD	E, A
		case 0x5F: 
			sCPU->E = sCPU->A;
			printf("LD\tE, A\t");
			break;
			
		// LD	A, B
		case 0x78: 
			sCPU->A = sCPU->B;
			printf("LD\tA, B\t");
			break;
			
		// LD	A, C
		case 0x79: 
			sCPU->A = sCPU->C;
			printf("LD\tA, C\t");
			break;
			
		// LD	A, (HL)
		case 0x7E: 
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LD\tA, $HL\t");
			break;

		// LD	(FFnn), A
		case 0xE0: 
			cRAM[0xFF00 + cRAM[iPC + 1]] = sCPU->A;
			printf("LD\t$ff%02x, A", cRAM[iPC+1]);
			iRet = 2;
			break;

		// LD	(FF00+C), A
		case 0xE2: 
			sCPU->A= 0xFF00 + sCPU->C;
			printf("LD\t$C, A\t");
			break;

		// LD	(nnnn), A
		case 0xEA: 
			cRAM[cRAM[iPC+2] << 8 ^ cRAM[iPC + 1]] = sCPU->A;
			printf("LD\t$%02x%02x, A", cRAM[iPC+2], cRAM[iPC+1]);
			iRet=3;
			break;

		// LD	A, (FFnn)
		case 0xF0: 
			sCPU->A = cRAM[0xFF00 + cRAM[iPC + 1]];
			printf("LD\tA, $ff%02x", cRAM[iPC+1]);
			iRet = 2;
			break;

	}

	return iRet;
}

int iRES  (sState *sCPU, uchar *cRAM);

// Return from subroutine
int iRET  (sState *sCPU, uchar *cRAM) {
	int iSP = sCPU->iSP;
	int iPC = sCPU->iPC;

	switch(cRAM[iPC]) {
		// RET	Z
		case 0xC8:
			if((sCPU->A & 0x80) == 0x80) {
				sCPU->iPC=(cRAM[iSP+1] << 8) ^ cRAM[iSP];
				sCPU->iSP+=2;
			}
			printf("RET\tZ\t");
			break;
		
		// RET
		case 0xC9:
			sCPU->iPC=(cRAM[iSP+1] << 8) ^ cRAM[iSP];
			sCPU->iSP+=2;
			printf("RET\t\t");
			break;

	}
	return 0;
}

int iRETI (sState *sCPU, uchar *cRAM);
int iSBC  (sState *sCPU, uchar *cRAM);
int iSCF  (sState *sCPU, uchar *cRAM);
int iSET  (sState *sCPU, uchar *cRAM);
int iSTOP (sState *sCPU, uchar *cRAM);
int iSUB  (sState *sCPU, uchar *cRAM);

int iSPEC(sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	
	switch(cRAM[iPC+1]) {
		case 0x37:
			sCPU->A = (sCPU->A << 4) ^ ((sCPU->A >> 4) & 15);
	
			DELFLAG(FLAG_Z | FLAG_N | FLAG_H | FLAG_C);
			if(sCPU->A == 0)
				SETFLAG(FLAG_Z);
			
			printf("SWAP\tA\t");
			break;

		case 0x87:
			sCPU->A &= 127;
			printf("RES\t0, A\t");
			break;
	}

	return 2;
}

int iUNK(sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	printf("\t[%02x]\t", cRAM[iPC]);
	return 0;
}

int (*iExec[])(sState *, uchar *) = {
	iNOP,	iLD,	iLD,	iINC,	iINC,	iDEC,	iLD,	iUNK,	// 00
	iUNK,	iUNK,	iUNK,	iDEC,	iINC,	iDEC,	iLD,	iUNK,	// 08
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	iUNK,	// 10
	iJR,	iADD,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	iUNK,	// 18
	iJR,	iLD,	iLDI,	iINC,	iUNK,	iUNK,	iLD,	iUNK,	// 20
	iUNK,	iUNK,	iLDI,	iINC,	iUNK,	iUNK,	iLD,	iCPL,	// 28
	iUNK,	iLD,	iLDD,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 30
	iUNK,	iUNK,	iLDD,	iUNK,	iUNK,	iDEC,	iLD,	iUNK,	// 38
	iLD,	iLD,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	// 40
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	// 48
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	iUNK,	// 50
	iUNK,	iUNK,	iUNK,	iLD,	iUNK,	iUNK,	iLD,	iLD,	// 58
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 60
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 68
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 70
	iLD,	iLD,	iUNK,	iUNK,	iUNK,	iUNK,	iLD,	iUNK,	// 78
	iUNK,	iADD,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iADD,	// 80
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 88
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 90
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// 98
	iUNK,	iAND,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	// A0
	iXOR,	iXOR,	iXOR,	iXOR,	iXOR,	iXOR,	iXOR,	iXOR,	// A8
	iOR,	iOR,	iOR,	iOR,	iOR,	iOR,	iOR,	iOR,	// B0
	iCP,	iCP,	iCP,	iCP,	iCP,	iCP,	iUNK,	iCP,	// B8
	iUNK,	iPOP,	iUNK,	iJP,	iUNK,	iPUSH,	iUNK,	iRST,	// C0
	iRET,	iRET,	iUNK,	iUNK,	iUNK,	iCALL,	iUNK,	iRST,	// C8
	iUNK,	iPOP,	iUNK,	iUNK,	iUNK,	iPUSH,	iUNK,	iRST,	// D0
	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iUNK,	iRST,	// D8
	iLD,	iPOP,	iLD,	iUNK,	iUNK,	iPUSH,	iAND,	iRST,	// E0
	iUNK,	iJP,	iLD,	iUNK,	iUNK,	iUNK,	iXOR,	iRST,	// E8
	iLD,	iPOP,	iUNK,	iDI,	iUNK,	iPUSH,	iOR,	iRST,	// F0
	iUNK,	iUNK,	iUNK,	iEI,	iUNK,	iUNK,	iCP,	iRST,	// F8
};

// Display the values of the
// registers on the screen
int iDebugMsg(sState *sCPU, uchar *cRAM) {
	printf(" | R: %02x%02x %02x%02x %02x%02x %02x%02x PC: %04x SP: %02x%02x %01x\n",
			sCPU->A, sCPU->F, sCPU->B, sCPU->C, sCPU->D, sCPU->E, sCPU->H, sCPU->L,
			sCPU->iPC, cRAM[sCPU->iSP+1], cRAM[sCPU->iSP], sCPU->iEI);
					
	return 0;
}

// Execute one CPU clock-cycle
int iClock(sState *sCPU, uchar *cRAM) {
	int iAdd=1, iPC=sCPU->iPC;
	
	printf("%04x\t%02x%02x%02x\t",iPC, cRAM[iPC], cRAM[iPC+1], cRAM[iPC+2]);
	
	iAdd = (*iExec[cRAM[iPC]])(sCPU, cRAM);
	// Only the 4 most significant
	// bits of F should hold information
	sCPU->F &= 0xF0;

	// Display registers
	iDebugMsg(sCPU, cRAM);

	// Increase Program Counter
	sCPU->iPC+=iAdd;

	return iAdd;
}

/*)\
\(*/
