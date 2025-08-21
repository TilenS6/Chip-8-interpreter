/* Author: Tilen Stermecki
   Date: 21.8.2025
   Published: GitHub [TilenS]

   This is a Chip-8 emulator written in C++.
*/

#pragma once
#include <iostream>
#include <cstring>
#include <cstdint>
#include "mega_utils/timer.h"

using namespace std;

#define CHIP8_MEMORY_SIZE 1024*4
#define CHIP8_STACK_DEPTH 64
#define CHIP8_PC_OFFSET 0x200 // at 512 program starts

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_BUFFER_SIZE ((CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT)/8)


class Chip8 {
    uint8_t _memory[CHIP8_MEMORY_SIZE], _PC;
    uint16_t _stack[CHIP8_STACK_DEPTH], _stack_pointer;
    uint8_t _display_buffer[CHIP8_DISPLAY_BUFFER_SIZE];
    uint8_t _V[16]; // VF is also a flag register: set on carry (+), on no-borrow (-), or on overlap while drawing
    uint16_t _I;

    Timer _timer;
    double _DT, _ST; //delay and sound timer

    void _processOpCode(uint16_t opcode);

    void _op_00E0(uint16_t opcode); // Display - disp_clear()       Clears the screen.
    void _op_00EE(uint16_t opcode); // Flow - return;               Returns from a subroutine.
    void _op_1NNN(uint16_t opcode); // Flow - goto NNN;             Jumps to address NNN.
    void _op_2NNN(uint16_t opcode); // Flow - *(0xNNN)()            Calls subroutine at NNN.
    void _op_3XNN(uint16_t opcode); // Cond - if (Vx == NN)         Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
    void _op_4XNN(uint16_t opcode); // Cond - if (Vx != NN)         Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
    void _op_5XY0(uint16_t opcode); // Cond - if (Vx == Vy)         Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
    void _op_6XNN(uint16_t opcode); // Const - Vx = NN              Sets VX to NN.
    void _op_7XNN(uint16_t opcode); // Const - Vx += NN             Adds NN to VX (carry flag is not changed).
    void _op_8XY0(uint16_t opcode); // Assig - Vx = Vy              Sets VX to the value of VY.
    void _op_8XY1(uint16_t opcode); // BitOp - Vx |= Vy             Sets VX to VX or VY. (bitwise OR operation).
    void _op_8XY2(uint16_t opcode); // BitOp - Vx &= Vy             Sets VX to VX and VY. (bitwise AND operation).
    void _op_8XY3(uint16_t opcode); // BitOp - Vx ^= Vy             Sets VX to VX xor VY.
    void _op_8XY4(uint16_t opcode); // Math - Vx += Vy              Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
    void _op_8XY5(uint16_t opcode); // Math - Vx -= Vy              VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
    void _op_8XY6(uint16_t opcode); // BitOp - Vx >>= 1             Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
    void _op_8XY7(uint16_t opcode); // Math - Vx = Vy - Vx          Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
    void _op_8XYE(uint16_t opcode); // BitOp - Vx <<= 1             Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
    void _op_9XY0(uint16_t opcode); // Cond - if (Vx != Vy)         Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block).
    void _op_ANNN(uint16_t opcode); // MEM - I = NNN                Sets I to the address NNN.
    void _op_BNNN(uint16_t opcode); // Flow - PC = V0 + NNN         Jumps to the address NNN plus V0.
    void _op_CXNN(uint16_t opcode); // Rand - Vx = rand() & NN      Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    void _op_DXYN(uint16_t opcode); // Display - draw(Vx, Vy, N)    Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
    void _op_EX9E(uint16_t opcode); // KeyOp - if (key() == Vx)     Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block).
    void _op_EXA1(uint16_t opcode); // KeyOp - if (key() != Vx)     Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block).
    void _op_FX07(uint16_t opcode); // Timer - Vx = get_delay()     Sets VX to the value of the delay timer.
    void _op_FX0A(uint16_t opcode); // KeyOp - Vx = get_key()       A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing).
    void _op_FX15(uint16_t opcode); // Timer - delay_timer(Vx)      Sets the delay timer to VX.
    void _op_FX18(uint16_t opcode); // Sound - sound_timer(Vx)      Sets the sound timer to VX.
    void _op_FX1E(uint16_t opcode); // MEM - I += Vx                Adds VX to I. VF is not affected.
    void _op_FX29(uint16_t opcode); // MEM - I = sprite_addr[Vx]    Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    void _op_FX33(uint16_t opcode); // BCD - set_BCD(Vx)            Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
    void _op_FX55(uint16_t opcode); // MEM - reg_dump(Vx, &I)       Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
    void _op_FX65(uint16_t opcode); // MEM - reg_load(Vx, &I)       Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
    void _op_unknown(uint16_t opcode);

    void _throw(string message);


public:
    Chip8();

    void stop_execution();
    void start_execution();

};

#include "chip8.cpp"

/*
Type
Call
Display - disp_clear()
Flow - return;
Flow - goto NNN;
Flow - *(0xNNN)()
Cond - if (Vx == NN)
Cond - if (Vx != NN)
Cond - if (Vx == Vy)
Const - Vx = NN
Const - Vx += NN
Assig - Vx = Vy
BitOp - Vx |= Vy
BitOp - Vx &= Vy
BitOp - Vx ^= Vy
Math - Vx += Vy
Math - Vx -= Vy
BitOp - Vx >>= 1
Math - Vx = Vy - Vx
BitOp - Vx <<= 1
Cond - if (Vx != Vy)
MEM - I = NNN
Flow - PC = V0 + NNN
Rand - Vx = rand() & NN
Display - draw(Vx, Vy, N)
KeyOp - if (key() == Vx)
KeyOp - if (key() != Vx)
Timer - Vx = get_delay()
KeyOp - Vx = get_key()
Timer - delay_timer(Vx)
Sound - sound_timer(Vx)
MEM - I += Vx
MEM - I = sprite_addr[Vx]
BCD - set_BCD(Vx)
MEM - reg_dump(Vx, &I)
MEM - reg_load(Vx, &I)




*/