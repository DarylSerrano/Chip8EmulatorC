#include "emulator.h"

// Debug Roms
 uint8_t rom1[] = {
		0x60, 0x0F, //LD V0 = 0x00
		0xf0, 0x29, // LD F, V0  I = Font dir of '0'
		0x61, 0x05, // Set x = 10
		0x62, 0x05,	// Set y = 10 
		0xd1, 0x25, // Draw the font on (5, 5) 
		0x12, 0x00	// Jump again to 0x200
};

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
	
	//Load Rom
	LoadRoom(chip8State,argv[1]);
	//LoadDebugRom(chip8State, rom1);
	
	//PrintMemoryStatus(chip8State);
	
	while(!quit)
	{	
		PrintPC(chip8State);
		PrintCurrentInst(chip8State);
		Decode(chip8State->memory,chip8State->PC,inst);
		Execute(chip8State,*inst, eRenderer);
		if(!(chip8State->waitKey) && chip8State->AdvancePC)
			Advance(chip8State);
		//Draw
		//if (chip8State->drawFlag)
		//{
			SDL_RenderPresent(eRenderer);
			SDL_Delay(100);
		//}
		RefreshTimer(chip8State);
		//Process Inputs
		while(SDL_PollEvent(&e) > 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = 1;
			}
		}
		ProcessInput(chip8State);
		//SDL_Delay(5000);
		
	}
	
	ExitEmu(chip8State, inst, eWindow, eRenderer);
	
	return 0;
}

