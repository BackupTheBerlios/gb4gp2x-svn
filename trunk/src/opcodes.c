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

// Disable interrupts
int iDI   (sState *sCPU, uchar *cRAM) {
	sCPU->iEI = 0;
	printf("DI\t\t");

	return 1;
}

// Enable interrupts
int iEI   (sState *sCPU, uchar *cRAM) {
	sCPU->iEI = 1;
	printf("EI\t\t");

	return 1;
}

// Load and decrease
int iLDD  (sState *sCPU, uchar *cRAM){
	int iPC = sCPU->iPC;
	int iRet = 1;
	
	switch(cRAM[iPC]) {
		// LDD	(HL), A
		case 0x32:
			cRAM[(sCPU->H << 8) ^ sCPU->L] = sCPU->A;
			printf("LDD\t$HL, A\t");
			break;
		
		// LDD	A, (HL)
		case 0x3A:
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LDD\tA, $HL\t");
			break;
			
	}

	// Decrease HL
	sCPU->L--;
	if(sCPU->L == 0xFF)
		sCPU->H--;

	return iRet;
}

// Load and increase
int iLDI  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	
	switch(cRAM[iPC]) {
		// LDI	(HL), A
		case 0x22: 
			cRAM[(sCPU->H << 8) ^ sCPU->L] = sCPU->A;
			printf("LDI\t$HL, A\t");
			break;
		
		// LDI	A, (HL)
		case 0x2A: 
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LDI\tA, $HL\t");
			break;
			
	}

	// Increase HL
	sCPU->L++;
	if(sCPU->L == 0)
		sCPU->H++;

	return iRet;
};

// The most important opcode
int iNOP  (sState *sCPU, uchar *cRAM) {
	printf("NOP\t\t");
	return 1;
}

// Bitwise OR operation
int iOR   (sState *sCPU, uchar *cRAM) {
    int iPC = sCPU->iPC; 
    int iHL, iRet = 1;
    
    switch(cRAM[iPC]) {
        // OR   B
        case 0xB0:
            sCPU->A |= sCPU->B;
            printf("OR\tB\t");
            break;

        // OR   C
        case 0xB1:
            sCPU->A |= sCPU->C;
            printf("OR\tC\t");
            break;

        // OR   D
        case 0xB2:
            sCPU->A |= sCPU->D;
            printf("OR\tD\t");
            break;

        // OR   E
        case 0xB3:
            sCPU->A |= sCPU->E;
            printf("OR\tE\t");
            break;

        // OR   H
        case 0xB4:
            sCPU->A |= sCPU->H;
            printf("OR\tH\t");
            break;

        // OR   L
        case 0xB5:
            sCPU->A |= sCPU->L;
            printf("OR\tL\t");
            break;

        // OR   (HL)
        case 0xB6:
            iHL = (sCPU->H << 8) ^ sCPU->L;
            sCPU->A |= cRAM[iHL];
            printf("OR\t(HL)\t");
            break;

        // OR   A
        case 0xB7:
            sCPU->A |= sCPU->A;
            printf("OR\tA\t");
            break;

        // OR   nn
        case 0xF6:
            sCPU->A |= cRAM[iPC+1];
            printf("OR\t%02x\t", cRAM[iPC+1]);
			iRet = 2;
            break;
    }

    // Edit Flags
    DELFLAG(FLAG_Z | FLAG_N | FLAG_H | FLAG_C);

    // Determine whether the 
    // Zero-Flag should be set
    if (sCPU->A == 0)
        SETFLAG(FLAG_Z);

    return iRet;
}

// Pop a value from the stack
int iPOP  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;
	
	int iLVal = cRAM[iSP];
	int iHVal = cRAM[iSP+1];

	// Increase the stack pointer
	sCPU->iSP+=2;

	switch(cRAM[iPC]) {
		// POP	BC
		case 0xC1:
			sCPU->B = iHVal;
			sCPU->C = iLVal;
			printf("POP\tBC\t");
			break;
		
		// POP	DE
		case 0xD1:
			sCPU->D = iHVal;
			sCPU->E = iLVal;
			printf("POP\tDE\t");
			break;
		
		// POP	HL
		case 0xE1:
			sCPU->H = iHVal;
			sCPU->L = iLVal;
			printf("POP\tHL\t");
			break;
		
		// POP	AF
		case 0xF1:
			sCPU->A = iHVal;
			sCPU->F = iLVal;
			printf("POP\tAF\t");
			break;
	}
	
	return 1;
}

// Push a value onto the stack
int iPUSH (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;
	uchar iHVal = 0, iLVal = 0;

	switch(cRAM[iPC]) {
		// PUSH	BC
		case 0xC5:
			iHVal = sCPU->B;
			iLVal = sCPU->C;
			printf("PUSH\tBC\t");
			break;
		
		// PUSH DE
		case 0xD5:
			iHVal = sCPU->D;
			iLVal = sCPU->E;
			printf("PUSH\tDE\t");
			break;
		
		// PUSH	HL
		case 0xE5:
			iHVal = sCPU->H;
			iLVal = sCPU->L;
			printf("PUSH\tHL\t");
			break;
			
		// PUSH AF
		case 0xF5:
			iHVal = sCPU->A;
			iLVal = sCPU->F;
			printf("PUSH\tAF\t");
			break;
			
	}
	
	// Put the value onto the stack
	cRAM[iSP-1] = iHVal;
	cRAM[iSP-2] = iLVal;

	// Decrease stack pointer
	sCPU->iSP-=2;

	return 1;
}

// Call a subroutine (one byte opcode)
int iRST  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cJP=0;

	switch(cRAM[iPC]) {
		// RST	00
		case 0xC7:
			cJP = 0x00;
			printf("RST\t00\t");
			break;

		// RST	08
		case 0xCF:
			cJP = 0x08;
			printf("RST\t08\t");
			break;
		
		// RST	10
		case 0xD7:
			cJP = 0x10;
			printf("RST\t10\t");
			break;
		
		// RST	18
		case 0xDF:
			cJP = 0x18;
			printf("RST\t18\t");
			break;
		
		// RST	20
		case 0xE7:
			cJP = 0x20;
			printf("RST\t20\t");
			break;
		
		// RST	28
		case 0xEF:		
			cJP = 0x28;
			printf("RST\t28\t");
			break;
		
		// RST	30
		case 0xF7:
			cJP = 0x30;
			printf("RST\t30\t");
			break;

		// RST	38
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

// Bitwise XOR operation
int iXOR  (sState *sCPU, uchar *cRAM) {
    int iPC = sCPU->iPC;
    int iHL, iRet = 1;
        
    switch(cRAM[iPC]) {
        // XOR  B
        case 0xA8:
            sCPU->A ^= sCPU->B;
            printf("XOR\tB\t");
            break;
            
        // XOR  C
        case 0xA9:
            sCPU->A ^= sCPU->C;
            printf("XOR\tC\t");
            break;

        // XOR  D
        case 0xAA:
            sCPU->A ^= sCPU->D;
            printf("XOR\tD\t");
            break;

        // XOR  E
        case 0xAB:
            sCPU->A ^= sCPU->E;
            printf("XOR\tE\t");
            break;

        // XOR  H
        case 0xAC:
            sCPU->A ^= sCPU->H;
            printf("XOR\tH\t");
            break;

        // XOR  L
        case 0xAD:
            sCPU->A ^= sCPU->L;
            printf("XOR\tL\t");
            break;

        // XOR  (HL)
        case 0xAE:
            iHL = (sCPU->H << 8) ^ sCPU->L;
            sCPU->A ^= cRAM[iHL];
            printf("XOR\t(HL)\t");
            break;

        // XOR  A
        case 0xAF:
            sCPU->A ^= sCPU->A;
            printf("XOR\tA\t");
            break;

        // XOR  nn
        case 0xEE:
            sCPU->A ^= cRAM[iPC+1];
            printf("XOR\t%02x\t", cRAM[iPC+1]);
			iRet = 2;
            break;

    }

    // Edit Flags
    DELFLAG(FLAG_Z | FLAG_N | FLAG_H | FLAG_C);

    // Determine whether the
    // Zero-Flag should be set
    if(sCPU->A == 0)
        SETFLAG(FLAG_Z);

    return iRet;
}

/*)\
\(*/
