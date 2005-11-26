/*
boot.c -- booting process
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

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gb.h"
#include "proc.h"

char cNintendoLogo[48] = {
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
};

int iCheckLogo(char *cLogo) {
	int i;

	for(i=0;i<48;i++)
		if(cLogo[i] != cNintendoLogo[i])
			return 0;

	return 1;
}

int iInit(char *cRAM, sState *sCPUstate) {

	// initial register values
	sCPUstate->A = 0x01;
	sCPUstate->F = 0xB0;
	sCPUstate->B = 0x00;
	sCPUstate->C = 0x13;
	sCPUstate->D = 0x00;
	sCPUstate->E = 0xD8;
	sCPUstate->H = 0x01;
	sCPUstate->L = 0x4D;

	sCPUstate->iPC = 0x0100; // entry point
	sCPUstate->iSP = 0xFFFE; // stack pointer init
	sCPUstate->iEI = 1; // enable interrupts

	cRAM[0xFF05] = 0x00; // TIMA
	cRAM[0xFF06] = 0x00; // TMA
	cRAM[0xFF07] = 0x00; // TAC
	cRAM[0xFF10] = 0x80; // NR10
	cRAM[0xFF11] = 0xBF; // NR11
	cRAM[0xFF12] = 0xF3; // NR12
	cRAM[0xFF14] = 0xBF; // NR14
	cRAM[0xFF16] = 0x3F; // NR21
	cRAM[0xFF17] = 0x00; // NR22
	cRAM[0xFF19] = 0xBF; // NR24
	cRAM[0xFF1A] = 0x7F; // NR30
	cRAM[0xFF1B] = 0xFF; // NR31
	cRAM[0xFF1C] = 0x9F; // NR32
	cRAM[0xFF1E] = 0xBF; // NR33
	cRAM[0xFF20] = 0xFF; // NR41
	cRAM[0xFF21] = 0x00; // NR42
	cRAM[0xFF22] = 0x00; // NR43
	cRAM[0xff23] = 0xBF; // NR30
	cRAM[0xFF24] = 0x77; // NR50
	cRAM[0xFF25] = 0xF3; // NR51
	cRAM[0xFF26] = 0xF1; // NR52 (0xF1 = GB, 0xF0 = SGB)
	cRAM[0xFF40] = 0x91; // LCDC
	cRAM[0xFF42] = 0x00; // SCY
	cRAM[0xFF43] = 0x00; // SCX
	cRAM[0xFF45] = 0x00; // LYC
	cRAM[0xFF47] = 0xFC; // BGP
	cRAM[0xFF48] = 0xFF; // OBP0
	cRAM[0xFF49] = 0xFF; // OBP1
	cRAM[0xFF4A] = 0x00; // WY
	cRAM[0xFF4B] = 0x00; // WX
	cRAM[0xFFFF] = 0x00; // IE

	return 1;
}

int iQuery(unsigned char *cRAM) {
		int i;

		printf("\ttitle:\t\t");
		for(i=0;i<16;i++)
			printf("%c", cRAM[0x0134+i]);
		printf("\n");

		printf("\tGBC Indicator:");
		i=cRAM[0x143];
		if(i!=0x80)
			printf("\tnon-");
		printf("GBC (0x%02x)\n",i);

		printf("\tGB/SGB Ind.:\t");
		if(cRAM[0x146]==0x03)
			printf("S");
		printf("GB\n");
		
		printf("\tlicensee:");
		printf("\t0x%02x%02x\n", cRAM[0x144], cRAM[0x145]);

		printf("\tcart type:");
		printf("\t0x%02x\n", cRAM[0x147]);

		printf("\tROM size:");
		printf("\t0x%02x\n", cRAM[0x148]);

		printf("\tRAM size:");
		printf("\t0x%02x\n", cRAM[0x149]);

		printf("\tDestination:\t");
		switch(cRAM[0x014A]) {
			case 1:
				printf("non-");
			case 0:
				printf("japanese\n");
				break;
			default:
				printf("unknown (0x%02x)\n", cRAM[0x014A]);
				break;
		}
	
		return 1;
}

int iBoot(char *cRomFilename, char *cRAM, sState *sCPUstate) {
	FILE *fRomFile;
	char *cLogo=(char*)malloc(48);
	int i, iCheck=0;

	printf("booting %s...\n", cRomFilename);
	
	fRomFile=fopen(cRomFilename, "r");
	if(fRomFile==NULL)
		return 0;

	printf("loading ROM into memory... ");
	
	fseek(fRomFile,0,SEEK_SET);
	for(i=0;i<0x8000;i++)
		cRAM[i]=fgetc(fRomFile);
	printf("done.\nROM info:\n");
	iQuery(cRAM);
	
	printf("\nreading Nintendo logo... ");
	fseek(fRomFile, 0x0104, SEEK_SET);
	fgets(cLogo, 49, fRomFile);
	printf("done.\ndisplaying logo...\n");
	// TODO
	
	printf("checking logo... ");
	if(iCheckLogo(cLogo))
		printf("ok\n");
	else {
		printf(":(\n");
		return 0;
	}
	
	printf("checksum: ");
	
	fseek(fRomFile, 0x0134, SEEK_SET);
	for(i=0x0134; i<0x014F; i++)
		iCheck+=fgetc(fRomFile);
	iCheck+=25;
	iCheck&=255;

	if(!iCheck)
		printf("ok\n");
	else
		printf(":(\n");

	printf("initializing memory, registers and stack... ");
	iInit(cRAM, sCPUstate);
	
	printf("done.\nboot sequence complete.\n\n");
	fclose(fRomFile);
	return 1;
}

int iCoreDump(char *cRAM, char *cDumpFilename) {
	FILE *fDumpFile;
	int i;
	
	fDumpFile=fopen(cDumpFilename, "w");

	for(i=0;i<65536;i++) {
				
		fputc(cRAM[i], fDumpFile);
	}
	
	fclose(fDumpFile);

	return 1;
}

int main(int argc, char *argv[]) {
	char *cRAM=(char*)malloc(65536);
	sState sCPUstate;
	int i, iNumOpcodes;

	if(argc == 1) {
		printf("SYNTAX: %s ROM\n", argv[0]);
		return -1;
	}

	if(argc > 2)
		iNumOpcodes = atoi(argv[2]);
	else
		iNumOpcodes = 10;
	
	// boot the ROM
	if(iBoot(argv[1],cRAM, &sCPUstate) == 0) {
		printf("ERROR: Could not open '%s'.\n", argv[1]);
		return 0;
	}
	
	// statistics
	printf("registers:\n");
	printf("AF: %02x%02x\n", sCPUstate.A & 255, sCPUstate.F & 255);
	printf("BC: %02x%02x\n", sCPUstate.B & 255, sCPUstate.C & 255);
	printf("DE: %02x%02x\n", sCPUstate.D & 255, sCPUstate.E & 255);
	printf("HL: %02x%02x\n", sCPUstate.H & 255, sCPUstate.L & 255);
	printf("stack pointer:   %04x\n", sCPUstate.iSP & 65535);
	printf("program counter: %04x\n\n", sCPUstate.iPC & 65535);
	
	printf("dumping core... ");
	iCoreDump(cRAM, "dump.bin");
	printf("done.\t\t |     AF   BC   DE   HL\n");

	// execute the beginning of the ROM
	for(i=0;i<iNumOpcodes;i++)
		iClock(&sCPUstate, cRAM);
	

	printf("exit.\n");
	return 0;
}

/*)\
\(*/
