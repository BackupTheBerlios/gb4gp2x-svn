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

// Set the Flags inside the F-Register
// An argument <2 means "Set this Value",
// An argument ==2 means "Don't touch this Value"
int iFlag(sState *sCPU, int Z, int N, int H, int C) {
	if(Z<2) 
		sCPU->F = (sCPU->F & 0x70) ^ (Z << 7);
	
	if(N<2) 
		sCPU->F = (sCPU->F & 0xB0) ^ (N << 6);
	
	if(H<2) 
		sCPU->F = (sCPU->F & 0xD0) ^ (H << 5);
	
	if(C<2) 
		sCPU->F = (sCPU->F & 0xE0) ^ (C << 4);

	sCPU->F &= 0xF0;

	return 1;
}

// These opcodes are not yet fully implemented.
// Will be moved to opcodes.c when done.
int iADC  (sState *sCPU, uchar *cRAM); 

// Arithmetic addition operation
int iADD  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ = 2, iH = 0, iC = 0;
	int iOldA;
	
	switch(cRAM[iPC]) {
		// ADD	A, C
		case 0x81:
			iOldA = sCPU->A;
			sCPU->A += sCPU->C;
			iZ = 0;
			
			if(sCPU->A == 0)
				iZ = 1;
			if((sCPU->A >= 0x10) && (iOldA < 0x10))
				iH = 1;
			if(sCPU->A < iOldA)
				iC = 1;
					
			printf("ADD\tA, C\t");
	}

	iFlag(sCPU, iZ, 0, iH, iC);
	return 1;	
}

// Bitwise AND operation
int iAND  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;

	switch(cRAM[iPC]) {

		// AND 	nn
		case 0xE6:
			sCPU->A &= cRAM[iPC+1];
			printf("AND\t%02x\t", cRAM[iPC+1]);
			break;
	}

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
	int iZ=0, iH=0, iC=0;
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

	}

	// Determine whether Z and C
	// Flags should be set
	if(sCPU->A == uCP) 
		iZ = 1;
	if(sCPU->A < uCP)
		iC = 1;

	// Set Flags
	iFlag(sCPU, iZ, 1, iH, iC);
	return 1;
}

int iCPL  (sState *sCPU, uchar *cRAM);
int iDAA  (sState *sCPU, uchar *cRAM);

// Decrease value
int iDEC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iZ=0, iN =1, iH=0;

	switch(cRAM[iPC]) {
		// DEC	B
		case 0x05:
			sCPU->B--;
			if(sCPU->B == 0)
				iZ = 1;
			if(sCPU->B == 0x0F)
				iH = 1;
			
			printf("DEC\tB\t");
			break;
	
		// DEC	BC
		case 0x0B:
			sCPU->C--;
			if(sCPU->C == 0xFF)
				sCPU->B--;
			iZ = iN = iH = 2;
			printf("DEC\tBC\t");
			break;
		
		// DEC	C
		case 0x0D:
			sCPU->C--;
			if(sCPU->C == 0)
				iZ = 1;
			if(sCPU->C == 0x0F)
				iH = 1;
			
			printf("DEC\tC\t");
			break;
	
		// DEC	A
		case 0x3D:
			sCPU->A--;
			if(sCPU->A == 0)
				iZ = 1;
			if(sCPU->A == 0x0F)
				iH = 1;
			
			printf("DEC\tA\t");
			break;
	}

	// Set Flags
	iFlag(sCPU, iZ, iN, iH, 2);
	return iRet;
}

int iHALT (sState *sCPU, uchar *cRAM);

// Increase value
int iINC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iH=0, iZ=0, iN =2, iBC;

	switch(cRAM[iPC]) {
		// INC	BC
		case 0x03:
			iBC = (sCPU->B << 8) ^ sCPU->C;
			iBC++;
			sCPU->B = (iBC >> 8) & 255;
			sCPU->C = iBC & 255;
			printf("INC\tBC\t");
			break;
		
		// INC	B
		case 0x04:
			sCPU->B++;
			
			if(sCPU->B == 0)
				iZ = 1;
			if(sCPU->B == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tB\t");
			break;
		
		// INC	C
		case 0x0C:
			sCPU->C++;
			
			if(sCPU->C == 0)
				iZ = 1;
			if(sCPU->C == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tC\t");
			break;
		
		// INC	L
		case 0x2c:
			sCPU->L++;
			
			if(sCPU->L == 0)
				iZ = 1;
			if(sCPU->L == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tL\t");
			break;
	}
	
	// Set Flags
	iFlag(sCPU, iZ, iN, iH, 2);
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

		// LD	E, E (dto.)
		case 0x5B: 
			sCPU->E = sCPU->E;
			printf("LD\tE, E\t");
			break;

		// LD	A, B
		case 0x78: 
			sCPU->A = sCPU->B;
			printf("LD\tA, B\t");
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

// Bitwise OR operation
int iOR   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ=0;

	switch(cRAM[iPC]) {
		// OR	B
		case 0xB0:
			sCPU->A |= sCPU->B;
			printf("OR\tB\t");
			break;
		
		// OR	C
		case 0xB1:
			sCPU->A |= sCPU->C;
			printf("OR\tC\t");
			break;
			
		// OR	D
		case 0xB2:
			sCPU->A |= sCPU->D;
			printf("OR\tD\t");
			break;
			
		// OR	E
		case 0xB3:
			sCPU->A |= sCPU->E;
			printf("OR\tE\t");
			break;
		
		// OR	H
		case 0xB4:
			sCPU->A |= sCPU->H;
			printf("OR\tH\t");
			break;
			
		// OR	L
		case 0xB5:
			sCPU->A |= sCPU->L;
			printf("OR\tL\t");
			break;
			
		// OR	A
		case 0xB7:
			sCPU->A |= sCPU->A;
			printf("OR\tA\t");
			break;
			
	}

	// Determine whether the 
	// Zero-Flag should be set
	if (sCPU->A == 0)
		iZ = 1;

	// Set Flags
	iFlag(sCPU, iZ, 0,0,0);
	return 1;
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

// Bitwise XOR operation
int iXOR  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ=0;

	switch(cRAM[iPC]) {
		// XOR	B
		case 0xA8:
			sCPU->A ^= sCPU->B;
			printf("XOR\tB\t");
			break;
			
		// XOR	C
		case 0xA9:
			sCPU->A ^= sCPU->C;
			printf("XOR\tC\t");
			break;
		
		// XOR	D
		case 0xAA:
			sCPU->A ^= sCPU->D;
			printf("XOR\tD\t");
			break;
			
		// XOR	E
		case 0xAB:
			sCPU->A ^= sCPU->E;
			printf("XOR\tE\t");
			break;
		
		// XOR	H
		case 0xAC:
			sCPU->A ^= sCPU->H;
			printf("XOR\tH\t");
			break;
		
		// XOR	L
		case 0xAD:
			sCPU->A ^= sCPU->L;
			printf("XOR\tL\t");
			break;
			
		// XOR	A
		case 0xAF:
			sCPU->A ^= sCPU->A;
			printf("XOR\tA\t");
			break;
			
	}

	// Determine whether the
	// Zero-Flag should be set
	if (sCPU->A == 0)
		iZ = 1;

	// Set Flags
	iFlag(sCPU, iZ, 0,0,0);
	return 1;
}

// Dump the values of the registers
// onto the screen
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

	switch(cRAM[iPC]) {
		case 0x81: iAdd = iADD (sCPU, cRAM); break;
		case 0xE6: iAdd = iAND (sCPU, cRAM); break;
		case 0xCD: iAdd = iCALL(sCPU, cRAM); break;
		case 0xB8:
		case 0xB9:
		case 0xBA:
		case 0xBB:
		case 0xBC:
		case 0xBD:
		case 0xBF: iAdd = iCP  (sCPU, cRAM); break;
		case 0x05: 
		case 0x0B:
		case 0x0D:
		case 0x3D: iAdd = iDEC (sCPU, cRAM); break;
		case 0xF3: iAdd = iDI  (sCPU, cRAM); break;
		case 0xFB: iAdd = iEI  (sCPU, cRAM); break;
		case 0x03:
		case 0x04: 
		case 0x0C: 
		case 0x2C: iAdd = iINC (sCPU, cRAM); break;
		case 0xC3: 
		case 0xE9: iAdd = iJP  (sCPU, cRAM); break;
		case 0x18: 
		case 0x20: iAdd = iJR  (sCPU, cRAM); break;
		case 0x01:
		case 0x02:
		case 0x06:
		case 0x0E:
		case 0x16:
		case 0x1E:
		case 0x21:
		case 0x26:
		case 0x2E:
		case 0x31:
		case 0x3E:
		case 0x40:
		case 0x5B:
		case 0x78:
		case 0x7E:
		case 0xE0:
		case 0xE2:
		case 0xEA:
		case 0xF0: iAdd = iLD  (sCPU, cRAM); break;
		case 0x32:
		case 0x3A: iAdd = iLDD (sCPU, cRAM); break;
		case 0x22:
		case 0x2A: iAdd = iLDI (sCPU, cRAM); break;
		case 0x00: iAdd = iNOP (sCPU, cRAM); break;
		case 0xB0:
		case 0xB1:
		case 0xB2:
		case 0xB3:
		case 0xB4:
		case 0xB5:
		case 0xB7: iAdd = iOR  (sCPU, cRAM); break;
		case 0xC1:
		case 0xD1:
		case 0xE1:
		case 0xF1: iAdd = iPOP (sCPU, cRAM); break;
		case 0xC5:
		case 0xD5:
		case 0xE5:
		case 0xF5: iAdd = iPUSH(sCPU, cRAM); break;
		case 0xC8:
		case 0xC9: iAdd = iRET (sCPU, cRAM); break;
		case 0xC7:
		case 0xCF:
		case 0xD7:
		case 0xDF:
		case 0xE7:
		case 0xEF:
		case 0xF7:		
		case 0xFF: iAdd = iRST (sCPU, cRAM); break;
		case 0xA8:
		case 0xA9:
		case 0xAA:
		case 0xAB:
		case 0xAC:
		case 0xAD:
		case 0xAF: iAdd = iXOR (sCPU, cRAM); break;

		default: printf("\t[%02x]\t", cRAM[iPC]);
			break;
	}

	// Only the 4 most significant
	// bits of F should hold information
	sCPU->F &= 0xF0;

	// Dump registers
	iDebugMsg(sCPU, cRAM);

	// Increase Program Counter
	sCPU->iPC+=iAdd;

	return iAdd;
}

/*)\
\(*/
