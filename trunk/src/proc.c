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

int iPOP  (sState *sCPU, char *cRAM, uchar *cOpcode) {
	return 1;
}

int iPUSH (sState *sCPU, char *cRAM, uchar *cOpcode) {
	int iSP = sCPU->iSP;

	cRAM[iSP-2] = cOpcode[1];
	cRAM[iSP-1] = cOpcode[0];

	sCPU->iSP-=2;

	return 1;
}

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

int iClock(sState *sCPU, char *cRAM) {
	uchar cOpcode;
	uchar cOperand[2];
	uchar A, F, B, C, D, E, H, L;
	int iAF, iBC, iDE, iHL;
	
	int iAdd=1, iPC;

	A = sCPU->A;
	F = sCPU->F;
	B = sCPU->B;
	C = sCPU->C;
	D = sCPU->D;
	E = sCPU->E;
	H = sCPU->H;
	L = sCPU->L;

	iAF = (A<<8)^F;
	iBC = (B<<8)^C;
	iDE = (D<<8)^E;
	iHL = (H<<8)^L;

	iPC=sCPU->iPC;
	cOpcode=cRAM[iPC]&255;
	cOperand[0]=cRAM[iPC+1];
	cOperand[1]=cRAM[iPC+2];

	printf("%04x\t",iPC);

	switch(cOpcode) {
		case 0x00: // NOP
			printf("NOP\n");
			break;
			
		case 0x01: // LD	BC, nnnn
			sCPU->B = cOperand[1];
			sCPU->C = cOperand[0];
			iAdd = 3;
			printf("LD\tBC, %02x%02x\n", sCPU->B, sCPU->C);
			break;

		case 0x02: // LD	(BC), A
			cRAM[iBC] = sCPU->A;
			printf("LD\t(BC), A\t(BC=%04x)\n", iBC);
			break;

		case 0x03: // INC	BC
			iBC++;
			sCPU->B = (iBC >> 8) & 255;
			sCPU->C = iBC & 255;
			printf("INC\tBC\t(->%04x)\n", iBC);
			break;
			
		case 0x04: // INC	B
			sCPU->B++;
			printf("INC\tB\t(B->%02x)\n", sCPU->B);
			break;
		
		case 0x05: // DEC	B
			sCPU->B--;
			printf("DEC\tB\t(B->%02x)\n", sCPU->B);
			break;

		case 0x06: // LD	B, nn
			sCPU->B = cOperand[0];
			iAdd = 2;
			printf("LD\tB, %02x\n", sCPU->B);
			break;

		case 0x22: // LDI	(HL), A
			cRAM[iHL] = A;
			sCPU->A++;
			printf("LDI\t(HL), A\t(HL=%04x A->%02x)\n", iHL, A + 1);
			break;

		case 0x2c: // INC	L
			sCPU->L++;
			printf("INC\tL\t(L=%02x)\n",L+1);
			break;

		case 0x31: // LD	SP, nnnn
			sCPU->iSP = (cOperand[1] << 8) ^ cOperand[0];
			iAdd = 3;
			printf("LD\tSP, %04x\n", sCPU->iSP);
			break;

		case 0x7e: // LD	A, (HL)
			sCPU->A=cRAM[iHL];
			printf("LD\tA, (HL)\t(A->%02x HL=%02x)\n", sCPU->A, iHL);
			break;

		case 0x81: // ADD A, C
			sCPU->A += C;
			printf("ADD\tA, C\t(A=%02x C=%02x A->%02x)\n",A,C,A+C);
			break;

		case 0xC3: // JP	nnnn
			sCPU->iPC = (cOperand[1] << 8) ^ cOperand[0];
			iAdd=0;
			printf("JP\t%04x\n", sCPU->iPC);
			break;

		case 0xCD: //CALL	nnnn
			iAdd = iPUSH (sCPU, cRAM, &cRAM[iPC]);
			sCPU->iPC = (cOperand[1] << 8) ^ cOperand[0];
			printf("CALL\t%02x%02x\n", cOperand[1], cOperand[0]);
			iAdd=0;
			break;

		case 0xEF: //RST	28
			iAdd = iPUSH (sCPU, cRAM, &cRAM[iPC]);
			sCPU->iPC = 0x28;
			iAdd=0;
			printf("RST\t28\n");
			break;

		default:
			printf("???\t(%02x)\n", cOpcode);
			break;
	}			
	sCPU->iPC+=iAdd;

	return iAdd;
}

/*)\
\(*/
