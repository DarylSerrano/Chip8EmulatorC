#include <stdlib.h>
#include <stdio.h>
//#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "cpu.h"
#include "debug.h"

//#include <SDL2/SDL.h> //<SDL.h>
//#include <SDL2/SDL_image.h>

#ifndef EMULATOR_H_INCLUDED
#define	EMULATOR_H_INCLUDED

// Initializes the state of the cpu
State * InitChip8();

//Load a room
void LoadRoom(State * state, char * path);

// Decodes instruction and prints to the console for debugging
void Decode(uint8_t * code, uint16_t pc, Instruction * inst);

// Advance pc
void Advance(State * state);

// Exit emulator
void ExitEmu(State * state, Instruction * inst, SDL_Window * eWindow, SDL_Renderer * eRenderer);

//Refresh Timers
void RefreshTimer(State * state);

//	Initialize the display 
void InitDisplay(SDL_Window ** eWindow, SDL_Renderer ** eRenderer);

// Refresh display
void UpdateDisplay(SDL_Renderer * eRenderer, State * state);

// Process Input
void ProcessInput(State * state);

// Instrucion implementations
void JumpCallReturn(State * state, Instruction inst); // SYS, JP, CALL, RET

void ClearScreen(State * state, Instruction inst); // CLS

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

void Draw(State * state, Instruction inst); // DRW Vx, Vy, n(nibble)

void SkipIfKeyPress(State * state, Instruction inst); // SKP Vx

void SkipIfKeNotPress(State * state, Instruction inst); // SKNP Vx

void MiscInstruction(State * state, Instruction inst); // 0x0F instructions

// Executes the current instruction
void Execute(State * state, Instruction inst);

#endif // EMULATOR_H_INCLUDED
