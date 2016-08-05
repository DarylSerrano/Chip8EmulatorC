#include "cpu.h"
#include <time.h>

#ifndef DEBUG_H_INCLUDED
#define	DEBUG_H_INCLUDED

#define	ROM1_SIZE		11

#define	KEY_PRESSED		"Key press"
#define	KEY_NPRESSED	"No Key press"

#define	LOG_NAME		"log.txt"

void LoadDebugRom(State * state, uint8_t * Rom);

void PrintMemoryStatus(State * state);

void PrintRegisters(State * state);

void PrintStack(State * state);

void KeyStates(State * state);

void PrintFlagsTimers(State * state);

void PrintPC(State * state);

void PrintCurrentInst(State * state);

FILE * OpenLog();

void CloseLog(FILE * debugLog);

#endif	// DEBUG_H_INCLUDED
