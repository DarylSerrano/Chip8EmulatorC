#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <SDL2/SDL.h> //<SDL.h>
//#include <SDL2/SDL_image.h>

#ifndef EMULATOR_H_INCLUDED
#define	EMULATOR_H_INCLUDED

// Macros
#define FONT_SIZE		5*16 // 5 bytes per font
#define SCREEN_BASE		0xF00 // Goes from 0xF00 to 0xFFF
#define	SCREEN_SIZE		256
#define	DISPLAY_WIDHT	640 
#define	DISPLAY_HEIGHT	320
#define WHITE			0xFF
#define BLACK			0x00
#define OPAQUE			0x00

typedef struct Chip8State{
	uint16_t PC;
	uint16_t I; //Special memory address register
	uint16_t SP;
	uint8_t V[16];
	uint8_t DT;
	uint8_t ST;
	uint8_t * memory;
	uint8_t	screen[32][64]; //[y][x]
	//uint8_t * screen;
	uint8_t keys[16];
	uint8_t waitKey;
	uint8_t drawFlag;
} State;


/* Input keypad layout:

Original chip-8 layout
1	2	3	C
4	5	6	D
7	8	9	E
A	0	B	F

New layout:
4	5	6	7
R	T	Y	U
F	G	H	J
V	B	N	M
*/

typedef struct Instruction{
	uint8_t firstNib;
	uint8_t secondNib;
	uint8_t finalNib;
	uint8_t firstByte;
	uint8_t secondByte;
} Instruction;

// Initializes the state of the cpu
State * InitChip8();

//Load a room
void LoadRoom(State * state, char * path);

// Decodes instruction and prints to the console for debugging
void Decode(uint8_t * code, uint16_t pc, Instruction * inst);

// Advance pc
void Advance(State * state);

// Exit emulator
void ExitEmu(State * state, Instruction * inst);

//Refresh Timers
void RefreshTimer(State * state);

//	Initialize the display 
void InitDisplay(SDL_Window ** eWindow, SDL_Renderer ** eRenderer);

//	Close the display
void CloseDisplay(SDL_Window * eWindow, SDL_Renderer * eRenderer);

// Process Input
void ProcessInput(State * state);

// Instrucion implementations
void JumpCallReturn(State * state, Instruction inst); // SYS, JP, CALL, RET

void ClearScreen(State * state, Instruction inst, SDL_Renderer * eRenderer); // CLS

void SkipIfEqualIn(State * state, Instruction inst); // SE Vx, nn [Skip if Vx == nn]

void SkipIfNotEqualIn(State * state, Instruction inst); // SNE Vx, nn [Skip if Vx != nn]

void SkipIfEqualReg(State * state, Instruction inst); // SE Vx, Vy [Skip if Vx == Vy]

void SkipIfNotEqualReg(State * state, Instruction inst); // SNE Vx, Vy [Skip if Vx != Vy]

void LoadValue(State * state, Instruction inst); // LD Vx, nn

void AddIn(State * state, Instruction inst); // ADD Vx, nn

void Move(State * state, Instruction inst); // LD Vx, Vy

void Arithmetic(State * state, Instruction inst); // LD [Vx], [Vy], OR, AND, XOR, ADD, SUB, SHR, SHL, SUBN 

void SetAddress(State * state, Instruction inst); // LD I, nnn [I = nnn]

void Random(State * state, Instruction inst); // RND Vx, nn

void Draw(State * state, Instruction inst, SDL_Renderer * eRenderer); // DRW Vx, Vy, n(nibble)

void SkipIfKeyPress(State * state, Instruction inst); // SKP Vx

void SkipIfKeNotPress(State * state, Instruction inst); // SKNP Vx

void MiscInstruction(State * state, Instruction inst); // 0x0F instructions

// Executes the current instruction
void Execute(State * state, Instruction inst, SDL_Renderer * eRenderer);

#endif // EMULATOR_H_INCLUDED
