#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "emulator.h"
#include "operations.h"

/*
typedef struct Chip8State{
	uint16_t PC;
	uint16_t I; //Special memory address register
	uint16_t SP;
	uint8_t V[16];
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
	uint8_t firstByte;
	uint8_t secondByte;
} Instruction;
*/

// Instrucion implementations
void JumpCallReturn(State * state, Instruction inst) // SYS, JP, CALL, RET
{
	switch(inst.firstNib)
	{
		case 0x00:
			// RET: sets PC to the adress on top of the stack and decrements the stack pointer
			if(inst.firstByte == 0x00 && inst.secondByte == 0xEE) 
			{
				state->PC = (0x0F & state->memory[state->SP]) << 8; // Maybe only needs a shift of 8 bits
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
	memset(state->screen,0,SCREEN_SIZE);
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
	switch(inst.finalNib)
	{
		case 0x01:
			//Bitwise OR: Vx OR Vy
			state->V[inst.secondNib] = state->V[inst.secondNib] | state->V[inst.secondByte >> 4]; 
			break;
		case 0x02:
			//Bitwise AND: Vx AND Vy
			state->V[inst.secondNib] = state->V[inst.secondNib] & state->V[inst.secondByte >> 4]; 
			break;
		case 0x03:
			//Bitwise XOR: Vx XOR Vy
			state->V[inst.secondNib] = state->V[inst.secondNib] ^ state->V[inst.secondByte >> 4]; 
			break;
		case 0x04:
			// Set Vx = Vx + Vy, if result is bigger than 8 bits (result > 255) VF is set to 1 (carry)
			if(0xFF < state->V[inst.secondNib] + state->V[inst.secondByte >> 4])
			{
				state->V[0x0F] = 0x01;
			}
			state->V[inst.secondNib] = state->V[inst.secondNib] + state->V[inst.secondByte >> 4];  
			break;
		case 0x05:
			// Set Vx = Vx - Vy if Vx > Vy then VF is set to 1 (not borrow), otherwise 0. Then Vy is subtracted from Vx and stored in Vx 
			if(state->V[inst.secondNib] > state->V[inst.secondByte >> 4])
			{
				state->V[0x0F] = 0x01; // not borrow
			}
			state->V[inst.secondNib] = state->V[inst.secondNib] - state->V[inst.secondByte >> 4];  
			break;
		case 0x06:
			// Set Vx = Vx Right Shift 1 if the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0
			if(0x01 & state->V[inst.secondNib])
			{
				state->V[0x0F] = 0x01;
			}
			state->V[inst.secondNib] = state->V[inst.secondNib] >> 1;
			break;
		case 0x07:
			// Set Vx = Vy - Vx if Vy > Vx then VF is set to 1 (not borrow), otherwise 0. Then Vx is subtracted from Vy and stored in Vx
			if(state->V[inst.secondByte >> 4] > state->V[inst.secondNib])
			{
				state->V[0x0F] = 0x01; // not borrow
			}
			state->V[inst.secondNib] = state->V[inst.secondByte >> 4] - state->V[inst.secondNib];  
			break;
		case 0x0E:
			//Set Vx = Vx Left Shift 1 if most-significant bit of Vx is 1, then VF is set to 1, otherwise 0
			if(0x08 & state->V[inst.secondNib])
			{
				state->V[0x0F] = 0x01;
			}
			state->V[inst.secondNib] = state->V[inst.secondNib] << 1;
			break; 
		default:
			fprintf(stderr, "Error aritmethic instrucion not avalible: %02x\n",inst.finalNib);
			break;
	}
}

void SetAddress(State * state, Instruction inst) // LD I, nnn [I = nnn]
{
	//Stores inmediate value nnn to the register I (memory register)
	uint16_t dir = (0x0000 | inst.secondNib) << 8;
	dir = dir | inst.secondByte; 
	state->I = dir;
}

void Random(State * state, Instruction inst) // RND Vx, nn
{
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
	switch(inst.secondByte)
	{
		case 0x07:
			//Set Vx = DT, load the value of the delay timer on register Vx
			state->V[inst.secondNib] = state->DT;
			break;
		case 0x0A:
			// Wait for a key press and store value of the key in Vx, execution stops until a key is pressed
			//TO-DO
			break;
		case 0x15:
			//Set DT = Vx, DT is set equal to the value of Vx
			state->DT = state->V[inst.secondNib];
			break;
		case 0x18:
			// Set sound timer = Vx, ST is set equal to the value of Vx
			state->ST = state->V[inst.secondNib];
			break;
		case 0x1E:
			// Set I = I + Vx, values of I and Vx are added and stored in I
			state->I = state->I + state->V[inst.secondNib];
			break;
		case 0x29:
			//Set I = location of sprite for digit Vx, value of I is set to the location for the hexadecimal 
			//sprite corresponding to the value of Vx, and sprite of letters [0-F]
			state->I = (state->V[inst.secondNib]*5); //Because letters are stored from 0x000 to 0x06E
			break;
		case 0x33:
			//Store BCD representation of Vx in memory locations I(hundreds digit), I+1 (tens digit), I+2 (ones digit). 
			uint8_t ones, tens, hundreds;
			uint8_t number = state->V[inst.secondNib];
			ones = number % 10;
			number = number / 10;
			tens = number % 10;
			hundreds = number / 10;
			state-memory[state->I] = hundreds;
			state->memory[state->I+1] = tens;
			state->memory[state->I+2] = ones; 
			break;
		case 0x55:
			//Store registers V0 trough Vx in memory starting at location I
			uint8_t i = 0x00;
			while(i <= state->V[inst.secondNib])
			{
				state->memory[state->I+i] = state->V[i];
			}
			break;
		case 0x65:
			//Read registers V0 through Vx from memory starting at location I
			uint8_t i = 0x00;
			while(i <= state->V[inst.secondNib])
			{
				 state->V[i] = state->memory[state->I+i];
			}
			break;
		default:
			fprintf(stderr,"Error, unavalible miscelaneous instruction: %02x\n",inst.secondByte);
			exit(1);
	}
}

