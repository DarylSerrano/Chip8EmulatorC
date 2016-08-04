#include <stdint.h>
#include <SDL2/SDL.h> 

#ifndef CPU_H_INCLUDED
#define	CPU_H_INCLUDED

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
	uint16_t Stack[16];
	int SP;
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

#endif	// CPU_H_INCLUDED
