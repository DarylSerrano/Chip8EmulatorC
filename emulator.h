#include <stdint.h>

#define FONT_SIZE	5*16 // 5 bytes per font
#define SCREEN_BASE	0xF00 // Goes from 0xF00 to 0xFFF
#define	SCREEN_SIZE	256

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


// Initializes the state of the cpu
State * InitChip8();

// Decodes instruction and prints to the console for debugging
void Decode(uint8_t * code, uint16_t pc, Instruction * inst);

// Emulates a tick of the processor
void Tick(State * state, Instruction * inst);

// Executes the current instruction
void Execute(State * state, Instruction inst);

// Advance pc
void Advance(State * state);
