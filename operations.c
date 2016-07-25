#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "emulator.h"
#include "operations.h"

/*
typedef struct Chip8State{
	uint16_t PC;
	uint16_t I; //Special memory address register
	uint8_t V[16];
	uint8_t SP;
	uint8_t DT;
	uint8_t ST;
	uint8_t * memory;
	uint8_t * screen;
	uint8_t keys[16];
} State;

typedef struct Instruction{
	uint8_t firstNib;
	uint8_t secondNib;
	uint8_t finalNib;
	uint16_t firstByte;
	uint16_t secondByte;
} Instruction; 

*/ 

// Instrucion implementations
void JumpCallReturn(State * state, Instruction inst) // SYS, JP, CALL, RET
{
	switch(inst.firstNib)
	{
		case 0x00:
			// RET: sets PC to the adress o top of the stack and decrements the stack pointer
			if(inst.firstByte == 0x00 && inst.secondByte == 0xEE) 
			{
				state->PC = (0x0F & state->memory[state->SP]) << 8; // Alomejor solo se necesita un shift de 8 bits
				state->PC = state->PC | state->memory[state->SP+1];
				state->SP -= 2; 
			}
			// SYS: jump to a machine code routine at nnn
			else
			{
				state->PC = (inst.firstByte << 8) | inst.secondByte;
			}
			break;
		case 0x01:
			// JP: jump to location nnn
			state->PC = (0x0F00 & (inst.firstByte << 8)) | inst.secondByte;
			break;
		case 0x02:
			// CALL: call subroutine at nnn, put PC on top of stack, then increments stack pointer
			state->memory[state->SP] = (state->PC >> 8);
			state->memory[state->SP+1] = state->PC & 0xFF;
			state->PC = (0x0F00 & (inst.firstByte << 8)) | inst.secondByte;
			state->SP += 2;
			break;
		case 0x0B:
			// JP: jump to location nnn + V0
			uint16_t dir = (0x0000 | inst.secondNib) << 8;
			dir = dir | inst.secondByte;
			state->PC = dir + state->V[0];
			break;
		default:
			fprintf(stderr,"Error, instruction not avalible\n");
			exit(1);
	}
}

void ClearScreen(State * state, Instruction inst) // CLS
{
	//TO-DO
}

void SkipIfEqualIn(State * state, Instruction inst) // SE Vx, nn [Skip if Vx == nn]
{
	if(state->V[inst.secondNib] == inst.secondByte)
	{
		state->PC += 2;
	}
}

void SkipIfNotEqualIn(State * state, Instruction inst) // SNE Vx, nn [Skip if Vx != nn]
{
	if(state->V[inst.secondNib] != inst.secondByte)
	{
		state->PC += 2;
	}	
}

void SkipIfEqualReg(State * state, Instruction inst) // SE Vx, Vy [Skip if Vx == Vy]
{
	if(state->V[inst.secondNib] == state->V[inst.secondByte >> 4])
	{
		state->PC += 2;
	}
}

void SkipIfNotEqualReg(State * state, Instruction inst) // SNE Vx, Vy [Skip if Vx != Vy]
{
	if(state->V[inst.secondNib] != state->V[inst.secondByte >> 4])
	{
		state->PC += 2;
	}	
}

void LoadValue(State * state, Instruction inst) // LD Vx, nn
{
	state->V[inst.secondNib] = inst.secondByte;
}

void AddIn(State * state, Instruction inst) // ADD Vx, nn
{
	state->V[inst.secondNib] += inst.secondByte;
}

void Move(State * state, Instruction inst) // LD Vx, Vy
{
	// Stores value of register Vy to register Vx
	state->V[inst.secondNib] = state->V[inst.secondByte >> 4];
}

void Arithmetic(State * state, Instruction inst) // OR, AND, XOR, ADD, SUB, SHR, SHL, SUBN 
{
	//TO-DO
	switch(inst.finalNib)
	{
		case 0x01:
			//Bitwise OR: Vx OR Vy
			state->V[inst.firstNib] = state->V[inst.firstNib] | state->V[inst.secondByte >> 4]; 
			break;
		case 0x02:
			//Bitwise AND: Vx AND Vy
			state->V[inst.firstNib] = state->V[inst.firstNib] & state->V[inst.secondByte >> 4]; 
			break;
		case 0x03:
			//Bitwise XOR: Vx XOR Vy
			state->V[inst.firstNib] = state->V[inst.firstNib] ^ state->V[inst.secondByte >> 4]; 
			break;
		case 0x04:
			// Set Vx = Vx + Vy, if result is bigger than 8 bits (result > 255) VF is set to 1 (carry)
			if(0xFF < state->V[inst.firstNib] + state->V[inst.secondByte >> 4])
			{
				state->V[0x0F] = 0x01;
			}
			state->V[inst.firstNib] = state->V[inst.firstNib] + state->V[inst.secondByte >> 4];  
			break;
		case 0x05:
			// Set Vx = Vx - Vy if Vx > Vy then VF is set to 1 (not borrow), otherwise 0. Then Vy is subtracted from Vx and stored in Vx 
			if(state->V[inst.firstNib] > state->V[inst.secondByte >> 4])
			{
				state->V[0x0F] = 0x01; // not borrow
			}
			state->V[inst.firstNib] = state->V[inst.firstNib] - state->V[inst.secondByte >> 4];  
			break;
		case 0x06:
			// Set Vx = Vx Right Shift 1 if the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
			if(0x01 & state->V[inst.firstNib])
			{
				state->V[0x0F] = 0x01;
			}
			state->V[inst.firstNib] = state->V[inst.firstNib] >> 1;
			break;
		case 0x07:
			// TO-DO
		default:
			fprintf(stderr, "Error aritmethic instrucion not avalible: %02x\n",inst.finalNib);
			break;
	}
}

void SetAddress(State * state, Instruction inst) // LD I, nnn [I = nnn]
{
	uint16_t dir = (0x0000 | inst.secondNib) << 8;
	dir = dir | inst.secondByte; 
	state->I = dir;
}

void Random(State * state, Instruction inst) // RND Vx, nn
{
	srand(time(NULL));
	uint8_t n = rand() % 256;
	state->V[inst.secondNib] = n & ins.secondByte;
}

void Draw(State * state, Instruction inst) // DRW Vx, Vy, n(nibble)
{
	//TO-DO
}

void SkipIfKeyPress(State * state, Instruction inst) // SKP Vx
{
	if(state->keys[state->V[inst.secondNib]])
	{
		state->PC += 2;
	} 
}

void SkipIfKeNotPress(State * state, Instruction inst) // SKNP Vx
{
	if(!state->keys[state->V[inst.secondNib]])
	{
		state->PC += 2;
	}	
}

void MiscInstruction(State * state, Instruction inst) // 0x0F instructions
{
	//TO-DO
}

