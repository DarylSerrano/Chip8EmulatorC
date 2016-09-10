#include "debug.h"

void LoadDebugRom(State * state, uint8_t * Rom)
{
	memcpy(state->memory + 0x200, Rom, ROM1_SIZE);
}

void PrintMemoryStatus(State * state)
{
	int i = 0;
	for(;i < 1024 * 4; ++i)
	{
		printf("%04x \t %02x %02x\n", i, state->memory[i], state->memory[i+1]);
		fflush(stdout);
		++i;
	}
}

void PrintRegisters(State * state)
{
	int i = 0;
	for(; i < 16; ++i)
	{
		printf("V%01x: \t %02x\n", state->V[i]);
		fflush(stdout); 
	}
	
	printf("I: \t %04x\n", state->I);
}

void PrintStack(State * state)
{
	int i = 0;
	for(; i < 16; ++i)
	{
		printf("S%01x: \t %02x\n",state->Stack[i]);
		fflush(stdout); 
	}
	
	printf("SP: \t %d\n",state->SP);
}

void KeyStates(State * state)
{
	int i = 0;
	for(; i < 16; ++i)
	{
		if(state->keys[i]) // Key pressed
		{
			printf("%01x: \t %s\n",i, KEY_PRESSED);
		}
		else
		{
			printf("%01x: \t %s\n",i, KEY_NPRESSED);
		}
	}
}

void PrintFlagsTimers(State * state)
{
	printf("Key wait: \t %d\n",state->waitKey);
	printf("Advance PC: \t %d\n",state->AdvancePC);
	printf("Draw flag: \t %d\n",state->drawFlag);
	printf("DT: \t %d\n",state->DT);
	printf("ST: \t %d\n",state->ST);
}

void PrintPC(State * state)
{
	printf("PC: \t %d, %04x\n",state->PC, state->PC);
	fflush(stdout);
}

void PrintCurrentInst(State * state)
{
	uint16_t inst = (state->memory[state->PC] << 8) | state->memory[state->PC+1];
	printf("Instruction: \t %04x\n",inst);
}

void PrintScreenArray(State * state)
{
	int i = 0;
	int j;
	for(; i < 32; ++i)
	{
		j = 0;
		for(; j < 64; ++i)
		{
			printf("%02x",state->screen[i][j]);
			fflush(stdout);
		}
		printf("\n");
	}
}

FILE * OpenLog()
{
	FILE * file = fopen(LOG_NAME, "a");
	
	return file;
}

void CloseLog(FILE * debugLog)
{
	fclose(debugLog);
}
