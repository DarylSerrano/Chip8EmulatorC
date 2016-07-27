#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "emulator.h"
#include "operations.h"

//TO-DO

// Initializes the state of the cpu
State * InitChip8()
{
	State * chip8State = calloc(1, sizeof(State));
	chip8State->memory = calloc(1024*4, 1);
	chip8State->screen = chip8State->memory[0xF00];
	chip8State->SP = 0xEA0; //Metemos e incrementamos el sp, sacamos y decrementamos el sp
	chip8State->PC = 0x200;
	return chip8State;
}

void Tick(State * state)
{
	uint8_t * code = state->memory[state->PC];
	uint8_t opcode = (code[0] >> 4);
	
	//TOO MUCH
	switch(opcode)
	{
		
	} 
}

int main(int argc, char ** argv)
{
	return 0;
}

