#include <stdint.h>

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
