#include "emulator.h"

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

//Load a room
void LoadRoom(State * state, char * path)
{
	FILE * file;
	long size;
	
	if(NULL == (file = fopen(path,"rb")))
	{
		fprintf(stderr,"Error loading the rom\n");
		exit(1);
	}
	
	//Get size of the file in bytes
	if(-1 == (fseek(file, 0L, SEEK_END)))
	{
		fprintf(stderr,"Error setting file position to end\n");
		exit(1);
	}
	size = ftell(file);
	if(-1 == (fseek(file, 0L, SEEK_SET)))
	{
		fprintf(stderr,"Error setting file position to begining\n");
		exit(1);
	}
	
	//Read the file and store it on the memory of the chip-8
	if(0 == (fread(&state->memory[state->PC], size, 1, file)))
	{
		if(ferror(file))
		{
			fprintf(stderr,"Error reading the file\n");
			exit(1);
		}
		else
		{
			fprintf(stderr,"End of file reached, cant read more\n");
			exit(1);
		}
	}
	
	if( 0 != fclose(file))
	{
		fprintf(stderr,"Error upon closing file stream\n");
		exit(1);
	}
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

// Exit emulator
void ExitEmu(State * state, Instruction * inst)
{
	free(state);
	free(inst);
}

//Refresh Timers
void RefreshTimer(State * state)
{
	if(state->DT > 0)
		state->DT -= 1;
	
	if(state->ST > 0)
		state->ST -= 1;
}

// Advance pc
void Advance(State * state)
{
	state->PC += 2;
}

//	Instrucion implementations
void JumpCallReturn(State * state, Instruction inst) // SYS, JP, CALL, RET
{
	uint16_t dir; // for 0x0B instruction
	switch(inst.firstNib)
	{
		case 0x00:
			// RET: sets PC to the adress on top of the stack and decrements the stack pointer
			if(inst.firstByte == 0x00 && inst.secondByte == 0xEE) 
			{
				state->PC = (0x000F & state->memory[state->SP]) << 8; 
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
			dir = (0x0000 | inst.secondNib) << 8;
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
	state->V[inst.secondNib] = n & inst.secondByte;
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
	uint8_t ones, tens, hundreds, number; // for 0x33 instruction
	uint8_t i;
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
			number = state->V[inst.secondNib];
			ones = number % 10;
			number = number / 10;
			tens = number % 10;
			hundreds = number / 10;
			state->memory[state->I] = hundreds;
			state->memory[state->I+1] = tens;
			state->memory[state->I+2] = ones; 
			break;
		case 0x55:
			//Store registers V0 trough Vx in memory starting at location I
			i = 0x00;
			while(i <= state->V[inst.secondNib])
			{
				state->memory[state->I+i] = state->V[i];
			}
			break;
		case 0x65:
			//Read registers V0 through Vx from memory starting at location I
			i = 0x00;
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
			else if(inst.firstByte == 0x00 && inst.secondByte == 0x00) 
			{
				//NOOP Instruction
			}else
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
				fprintf(stderr,"Error, instruction not avalible: %01x %02x\n",inst.firstNib, inst.secondByte);
				exit(1);
			}
			break;
		case 0x0F:
			MiscInstruction(state, inst);
			break;
		default:
			fprintf(stderr,"Error, instruction not avalible: %01x\n",inst.firstNib);
			exit(1);
	}
}

