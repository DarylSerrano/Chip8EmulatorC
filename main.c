#include "emulator.h"

int main(int argc, char ** argv)
{
	//	Initialize CHip-8 and display
	//	Load rom
	//	Main Loop:
	//		Fetch instruction
	//		Decode instruction
	//		Execute instruction
	//		Draw display
	//		Refresh timers
	//		Process Inputs
	//		Advance PC
	//	Exit emulator
	
	if(argc < 2)
	{
		fprintf(stderr,"Error, usage of the program: %s [nameOfTheRom]\n",argv[0]);
		exit(1);
	}
	
	State * chip8State = InitChip8();
	Instruction * inst = malloc(sizeof(Instruction));
	//Initialize display
	
	LoadRoom(chip8State,argv[1]);
	
	for(;;)
	{
		Decode(chip8State->memory,chip8State->PC,inst);
		Execute(chip8State,*inst);
		//Draw
		RefreshTimer(chip8State);
		//Process Inputs
		if(chip8State->waitKey == 0)
			Advance(chip8State);
	}
	
	ExitEmu(chip8State, inst);
	
	return 0;
}

