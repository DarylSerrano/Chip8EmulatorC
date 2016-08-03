#include "emulator.h"

int main(int argc, char ** argv)
{
	//	Initialize CHip-8 and display
	//	Load rom
	//	Main Loop:
	//		Fetch instruction
	//		Decode instruction
	//		Execute instruction
	//		Advance PC
	//		Draw display
	//		Refresh timers
	//		Process Inputs
	//		Update flags
	//	Exit emulator
	
	if(argc < 2)
	{
		fprintf(stderr,"Error, usage of the program: %s [nameOfTheRom]\n",argv[0]);
		exit(1);
	}
	
	//Display Variables
	SDL_Window * eWindow;
	SDL_Renderer * eRenderer;
	SDL_Event e;
	int quit = 0;
	
	// Init emulator state
	State * chip8State = InitChip8();
	Instruction * inst = malloc(sizeof(Instruction));
	
	//Initialize display
	InitDisplay(&eWindow, &eRenderer);
	
	LoadRoom(chip8State,argv[1]);
	
	while(!quit)
	{
		Decode(chip8State->memory,chip8State->PC,inst);
		Execute(chip8State,*inst, eRenderer);
		if(!(chip8State->waitKey))
			Advance(chip8State);
		//Draw
		if (chip8State->drawFlag)
		{
			SDL_RenderPresent(eRenderer);
			chip8State->drawFlag = 0x00;
		}
		RefreshTimer(chip8State);
		//Process Inputs
		while(SDL_PollEvent(&e) > 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = 1;
			}
		}
		SDL_RenderPresent(eRenderer);
		ProcessInput(chip8State);
	}
	
	ExitEmu(chip8State, inst);
	
	CloseDisplay(eWindow, eRenderer);
	
	return 0;
}

