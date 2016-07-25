#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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

// Initializes the state of the cpu
State * InitChip8()
{
	State * chip8State = calloc(1, sizeof(State));
	chip8State->memory = calloc(1024*4, 1);
	chip8State->screen = chip8State->memory[0xF00];
	chip8State->SP = 0xEA0; //Metemos e incrementamos el sp, sacamos y decrementamos el sp
	chip8State->PC = 0x200;
	chip8State->V = calloc(16,sizeof(uint8_t);
	chip8State->keys = calloc(16, sizeof(uint8_t));
	return chip8State;
}

// Decodes instruction and prints to the console for debugging
void Decode(uint8_t * code, uint16_t pc, Instruction * inst)
{
	inst->firstByte = code[pc];
	inst->secondByte = code[pc+1];
	inst->firstNib = (code[pc] >> 4);
	inst->secondNib = (code[pc] & 0x0f);
	inst->finalNib = (code[pc+1] & 0x0f);
	
}

// Advance pc
void Advance(State * state)
{
	state->PC += 2;
}

// Emulates a tick of the processor
void Tick(State * state, Instruction * inst)
{
	Decode(state->memory,inst);
	Execute(state,*inst);
	//Update timers
}

// Executes the current instruction
void Execute(State * state, Instruction inst)
{
	switch(inst.firstNib)
	{
		case 0x00:
			if(inst.secondByte == 0xE0)
			{
				ClearScreen(state,inst);
			}
			else
			{
				JumpCallReturn(state,inst);
			}
			break;
		case 0x01:
			JumpCallReturn(state,inst);
			break;
		case 0x02:
			JumpCallReturn(state,inst);
			break;
		case 0x03:
			SkipIfEqualIn(state,inst);
			break;
		case 0x04:
			SkipIfNotEqualIn(state,inst);
			break;
		case 0x05:
			SkipIfEqualReg(state,inst);
			break;
		case 0x06:
			LoadValue(state,inst);
			break;
		case 0x07:
			AddIn(state,inst);
			break;
		case 0x08:
			Arithmetic(state,inst);
			break;
		case 0x09:
			SkipIfNotEqualReg(state,inst);
			break;
		case 0x0A:
			SetAddress(state,inst);
			break;
		case 0x0B:
			JumpCallReturn(state,inst);
			break;
		case 0x0C:
			Random(state, inst);
			break;
		case 0x0D:
			Draw(state, inst);
			break;
		case 0x0E:
			if(inst.secondByte == 0x9E)
			{
				SkipIfKeyPress(state,inst);
			}
			else if(inst.secondByte == 0xA1)
			{
				SkipIfKeNotPress(state,inst);
			}
			else
			{
				fprintf(stderr,"Error, instruction not avalible: %01x %02x\n",inst->firstNib, inst->secondByte);
				exit(1);
			}
			break;
		case 0x0F:
			MiscInstruction(state, inst);
			break;
		default:
			fprintf(stderr,"Error, instruction not avalible: %01x\n",inst->firstNib);
			break;
	}
}

