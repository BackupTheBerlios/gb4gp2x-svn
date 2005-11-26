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
					
			printf("ADD\tA, C\t");
	}

	iFlag(sCPU, iZ, 0, iH, iC);
	return 1;	
}

int iAND  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;

	switch(cRAM[iPC]) {
		case 0xE6:
			sCPU->A &= cRAM[iPC+1];
			printf("AND\t%02x\t", cRAM[iPC+1]);
			break;
	}

	return 2;
}
int iBIT  (sState *sCPU, uchar *cRAM);

int iCALL (sState *sCPU, uchar *cRAM){
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;

	uchar cHB=0, cLB=0;

	switch(cRAM[iPC]) {
		case 0xCD: //CALL	nnnn
			cHB = cRAM[iPC+2];
			cLB = cRAM[iPC+1];
			
			printf("CALL\t%02x%02x\t", cRAM[iPC+2], cRAM[iPC+1]);
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

int iCP   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iZ=0, iH=0, iC=0;
	uchar uCP = 0;

	switch(cRAM[iPC]) {
		case 0xB8: 
			uCP = sCPU->B;
			printf("CP\tB\t");
			break;

		case 0xB9: 
			uCP = sCPU->C;
			printf("CP\tC\t");
			break;

		case 0xBA: 
			uCP = sCPU->D;
			printf("CP\tD\t");
			break;

		case 0xBB: 
			uCP = sCPU->E;
			printf("CP\tE\t");
			break;

		case 0xBC: 
			uCP = sCPU->H;
			printf("CP\tH\t");
			break;

		case 0xBD: 
			uCP = sCPU->L;
			printf("CP\tL\t");
			break;

		case 0xBF: 
			uCP = sCPU->A;
			printf("CP\tA\t");
			break;

	}
	
	if(sCPU->A == uCP) 
		iZ = 1;
	if(sCPU->A < uCP)
		iC = 1;

	iFlag(sCPU, iZ, 1, iH, iC);
	return 1;
}

int iCPL  (sState *sCPU, uchar *cRAM);
int iDAA  (sState *sCPU, uchar *cRAM);

int iDEC  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet = 1;
	int iZ=0, iN =1, iH=0;

	switch(cRAM[iPC]) {
		case 0x05: // DEC	B
			sCPU->B--;
			if(sCPU->B == 0)
				iZ = 1;
			if(sCPU->B == 0x0F)
				iH = 1;
			
			printf("DEC\tB\t");
			break;
		
		case 0x3D: // DEC	A
			sCPU->A--;
			if(sCPU->A == 0)
				iZ = 1;
			if(sCPU->A == 0x0F)
				iH = 1;
			
			printf("DEC\tA\t");
			break;
	}

	iFlag(sCPU, iZ, iN, iH, 2);
	return iRet;
}

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
			printf("INC\tBC\t");
			break;
		
		case 0x04: // INC	B
			sCPU->B++;
			
			if(sCPU->B == 0)
				iZ = 1;
			if(sCPU->B == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tB\t");
			break;
		
		case 0x2c: // INC	L
			sCPU->L++;
			
			if(sCPU->L == 0)
				iZ = 1;
			if(sCPU->L == 0x10)
				iH = 1;
			iN = 0;
			
			printf("INC\tL\t");
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
			printf("JP\t%04x\t", sCPU->iPC);
			break;

		case 0xE9:
			sCPU->iPC = (sCPU->H << 8) ^ sCPU->L;
			printf("JP\t%04x\t", sCPU->iPC);
			break;
		
		default:
			iRet = 1;
	}
	return iRet;	
}

int iJR   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iAdd = 1;

	switch(cRAM[iPC]) {
		case 0x20:
			if((sCPU->F & 0x80) == 0) {
				sCPU->iPC += cRAM[iPC+1];
				iAdd = 0;
			}
			printf("JR\tNZ, %02x\t", cRAM[iPC+1]);
			break;
	}

	return iAdd;
}

int iLD   (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iRet= 1;
	
	switch(cRAM[iPC]) {
		case 0x01: // LD	BC, nnnn
			sCPU->B = cRAM[iPC+2];
			sCPU->C = cRAM[iPC+1];
			iRet = 3;
			printf("LD\tBC, %02x%02x", sCPU->B, sCPU->C);
			break;

		case 0x02: // LD	(BC), A
			cRAM[(sCPU->B) << 8 ^ sCPU->C] = sCPU->A;
			printf("LD\t$BC, A\t");
			break;

		case 0x06: // LD	B, nn
			sCPU->B = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tB, %02x\t", sCPU->B);
			break;
		
		case 0x21: // LD	HL, nnnn
			sCPU->H = cRAM[iPC+2];
			sCPU->L = cRAM[iPC+1];
			iRet = 3;
			printf("LD\tHL, %02x%02x", sCPU->H, sCPU->L);
			break;

		case 0x31: // LD	SP, nnnn
			sCPU->iSP = (cRAM[iPC+2] << 8) ^ cRAM[iPC+1];
			iRet = 3;
			printf("LD\tSP, %04x", sCPU->iSP);
			break;
		
		case 0x3E: // LD	A, nn
			sCPU->A = cRAM[iPC+1];
			iRet = 2;
			printf("LD\tA, %02x\t", sCPU->A);
			break;
			
		case 0x40: // LD	B, B
			sCPU->B = sCPU->B;
			printf("LD\tB, B\t");
			break;

		case 0x5B: // LD	E, E
			sCPU->E = sCPU->E;
			printf("LD\tE, E\t");
			break;

		case 0x7E: // LD	A, (HL)
			sCPU->A = cRAM[(sCPU->H << 8) ^ sCPU->L];
			printf("LD\tA, $HL\t");
			break;

		case 0xE0: // LD	(FF00+nn), A
			cRAM[0xFF00 + cRAM[iPC + 1]] = sCPU->A;
			printf("LD\t$ff%02x, A", cRAM[iPC+1]);
			iRet = 2;
			break;

		case 0xEA: // LD	(nnnn), A
			cRAM[cRAM[iPC+2] << 8 ^ cRAM[iPC + 1]] = sCPU->A;
			printf("LD\t$%02x%02x, A", cRAM[iPC+2], cRAM[iPC+1]);
			iRet=3;
			break;

		case 0xF0: // LD	A, (FF00+nn)
			sCPU->A = cRAM[0xFF00 + cRAM[iPC + 1]];
			printf("LD\tA, $ff%02x", cRAM[iPC+1]);
			iRet = 2;
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
			printf("LDI\t$HL, A\t");
	}

	return iRet;
};

int iNOP  (sState *sCPU, uchar *cRAM) {
	printf("NOP\t\t");
	return 1;
}
int iOR   (sState *sCPU, uchar *cRAM);

int iPOP  (sState *sCPU, uchar *cRAM) {
	int iPC = sCPU->iPC;
	int iSP = sCPU->iSP;
	
	int iHVal = cRAM[iSP];
	int iLVal = cRAM[iSP+1];
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
	
	
	cRAM[iSP-2] = iHVal;
	cRAM[iSP-1] = iLVal;

	sCPU->iSP-=2;

	return 1;
}

int iRES  (sState *sCPU, uchar *cRAM);

int iRET  (sState *sCPU, uchar *cRAM) {
	int iSP = sCPU->iSP;
	int iPC = sCPU->iPC;

	switch(cRAM[iPC]) {
		case 0xC8:
			if((sCPU->A & 0x80) == 0x80) {
				sCPU->iPC=(cRAM[iSP] << 8) ^ cRAM[iSP+1];
				sCPU->iSP+=2;
			}
			printf("RET\tZ\t");
			break;
		
		case 0xC9:
			sCPU->iPC=(cRAM[iSP] << 8) ^ cRAM[iSP+1];
			sCPU->iSP+=2;
			printf("RET\t\t");
			break;

	}
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

int iDebugMsg(sState *sCPU) {
	printf(" | R: %02x%02x %02x%02x %02x%02x %02x%02x PC: %04x SP: %04x %01x\n",
			sCPU->A, sCPU->F, sCPU->B, sCPU->C, sCPU->D, sCPU->E,
			sCPU->H, sCPU->L, sCPU->iPC, sCPU->iSP, sCPU->iEI);
					
	return 0;
}

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
		case 0x3D: iAdd = iDEC (sCPU, cRAM); break;
		case 0xF3: iAdd = iDI  (sCPU, cRAM); break;
		case 0xFB: iAdd = iEI  (sCPU, cRAM); break;
		case 0x03:
		case 0x04: 
		case 0x2C: iAdd = iINC (sCPU, cRAM); break;
		case 0xC3: 
		case 0xE9: iAdd = iJP  (sCPU, cRAM); break;
		case 0x20: iAdd = iJR  (sCPU, cRAM); break;
		case 0x01:
		case 0x02:
		case 0x06:
		case 0x21:
		case 0x31:
		case 0x3E:
		case 0x40:
		case 0x5B:
		case 0x7E:
		case 0xE0:
		case 0xEA:
		case 0xF0: iAdd = iLD  (sCPU, cRAM); break;
		case 0x22: iAdd = iLDI (sCPU, cRAM); break;
		case 0x00: iAdd = iNOP (sCPU, cRAM); break;
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

		default: printf("\t[%02x]\t", cRAM[iPC]);
			break;
	}			
	iDebugMsg(sCPU);
	
	sCPU->iPC+=iAdd;

	return iAdd;
}

/*)\
\(*/
