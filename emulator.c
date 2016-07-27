#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
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

// Initializes the state of the cpu
State * InitChip8()
{
	State * chip8State = calloc(1, sizeof(State));
	chip8State->memory = malloc(1024*4);
	memset(chip8State->memory,0,1024*4);
	chip8State->screen = &chip8State->memory[SCREEN_BASE]; 
	chip8State->SP = 0xEA0;
	chip8State->PC = 0x200;
	chip8State->I = 0x0;
	/*
	int i;
	for(i=0; i < 16; ++i)
	{
		chip8State->V[i] = 0x0;
		chip8State->keys[i] = 0x0;
	}
	*/
	
	memset(chip8State->V,0,sizeof(uint8_t)*16);
	memset(chip8State->keys,0,sizeof(uint8_t)*16);
	
	//Initialize fonts
	uint8_t fonts[] = {
		//0
		0xF0,
		0x90,
		0X90,
		0X90,
		0XF0,
		
		//1
		0x20,
		0x60,
		0x20,
		0x20,
		0x70,
		
		//2
		0xF0,
		0x10,
		0xF0,
		0x80,
		0xF0,
		
		//3
		0xF0,
		0x10,
		0xF0,
		0x10,
		0xF0,
		
		//4
		0x90,
		0x90,
		0xF0,
		0x10,
		0x10,
		
		//5
		0xF0,
		0x80,
		0xF0,
		0x10,
		0xF0,
		
		//6
		0xF0,
		0x80,
		0xF0,
		0x90,
		0xF0,
		
		//7
		0xF0,
		0x10,
		0x20,
		0x40,
		0x40,
		
		//8
		0xF0,
		0x90,
		0xF0,
		0x90,
		0xF0,
		
		//9
		0xF0,
		0x90,
		0xF0,
		0x10,
		0xF0,
		
		//A
		0xF0,
		0x90,
		0xF0,
		0x90,
		0x90,
		
		//B
		0xE0,
		0x90,
		0xE0,
		0x90,
		0xE0,
		
		//C
		0xF0,
		0x80,
		0x80,
		0x80,
		0xF0,
		
		//D
		0xE0,
		0x90,
		0x90,
		0x90,
		0xE0,
		
		//E
		0xF0,
		0x80,
		0xF0,
		0x80,
		0xF0,
		
		//F
		0xF0,
		0x80,
		0xF0,
		0x80,
		0x80,
	};
	
	memcpy(chip8State->memory,fonts,FONT_SIZE);
	
	srand(time(NULL)); //For RND instruction

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
			exit(1);
	}
}

