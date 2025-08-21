#include "chip8.h"

Chip8::Chip8() {
    memset(_memory, 0, CHIP8_MEMORY_SIZE);
    memset(_display_buffer, 0, CHIP8_DISPLAY_BUFFER_SIZE);
    memset(_V, 0, 16);
    _I = 0;

    _timer.interval();

    _DT = _ST = 0.;
}

void Chip8::stop_execution() {
    // TODO
}

void Chip8::start_execution() {
    _PC = 0;
    // TODO
}

/*
Opcode 	Type 	C Pseudocode 	Explanation
*0NNN 	Call 		Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
00E0 	Display 	disp_clear() 	Clears the screen.
00EE 	Flow 	return; 	Returns from a subroutine
.
1NNN 	Flow 	goto NNN; 	Jumps to address NNN.

2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN.

3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).

4XNN 	Cond 	if (Vx != NN) 	Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).

5XY0 	Cond 	if (Vx == Vy) 	Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).

6XNN 	Const 	Vx = NN 	Sets VX to NN.

7XNN 	Const 	Vx += NN 	Adds NN to VX (carry flag is not changed).

8XY0 	Assig 	Vx = Vy 	Sets VX to the value of VY.
8XY1 	BitOp 	Vx |= Vy 	Sets VX to VX or VY. (bitwise OR operation).
8XY2 	BitOp 	Vx &= Vy 	Sets VX to VX and VY. (bitwise AND operation).
8XY3 	BitOp 	Vx ^= Vy 	Sets VX to VX xor VY.
8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
8XY6 	BitOp 	Vx >>= 1 	Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
8XY7 	Math 	Vx = Vy - Vx 	Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
8XYE 	BitOp 	Vx <<= 1 	Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.

9XY0 	Cond 	if (Vx != Vy) 	Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block).

ANNN 	MEM 	I = NNN 	Sets I to the address NNN.

BNNN 	Flow 	PC = V0 + NNN 	Jumps to the address NNN plus V0.

CXNN 	Rand 	Vx = rand() & NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.

DXYN 	Display 	draw(Vx, Vy, N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.

EX9E 	KeyOp 	if (key() == Vx) 	Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block).
EXA1 	KeyOp 	if (key() != Vx) 	Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block).

FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing).
FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
FX1E 	MEM 	I += Vx 	Adds VX to I. VF is not affected.
FX29 	MEM 	I = sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font.
FX33 	BCD 	set_BCD(Vx)	    Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
FX55 	MEM 	reg_dump(Vx, &I) 	Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
FX65 	MEM 	reg_load(Vx, &I) 	Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
*/
void Chip8::_processOpCode(uint16_t opcode) {
    uint8_t first_digit = (opcode >> 24);
    switch (first_digit) {
        case 0x0:
            switch (opcode) {
                case 0x00E0:
                    _op_00E0(opcode);
                    break;
                case 0x00EE:
                    _op_00EE(opcode);
                    break;
                default:
                    _op_unknown(opcode);
            }
            break;
        case 0x1:
            _op_1NNN(opcode);
            break;
        case 0x2:
            _op_2NNN(opcode);
            break;
        case 0x3:
            _op_3XNN(opcode);
            break;
        case 0x4:
            _op_4XNN(opcode);
            break;
        case 0x5:
            if (opcode & 0x000F == 0)
                _op_5XY0(opcode);
            else
                _op_unknown(opcode);
            break;
        case 0x6:
            _op_6XNN(opcode);
            break;
        case 0x7:
            _op_7XNN(opcode);
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0:
                    _op_8XY0(opcode);
                    break;
                case 0x1:
                    _op_8XY1(opcode);
                    break;
                case 0x2:
                    _op_8XY2(opcode);
                    break;
                case 0x3:
                    _op_8XY3(opcode);
                    break;
                case 0x4:
                    _op_8XY4(opcode);
                    break;
                case 0x5:
                    _op_8XY5(opcode);
                    break;
                case 0x6:
                    _op_8XY6(opcode);
                    break;
                case 0x7:
                    _op_8XY7(opcode);
                    break;
                case 0xE:
                    _op_8XYE(opcode);
                    break;
                default:
                    _op_unknown(opcode);
            }
            break;
        case 0x9:
            if (opcode & 0x000F == 0)
                _op_9XY0(opcode);
            else
                _op_unknown(opcode);
            break;
        case 0xA:
            _op_ANNN(opcode);
            break;
        case 0xB:
            _op_BNNN(opcode);
            break;
        case 0xC:
            _op_CXNN(opcode);
            break;
        case 0xD:
            _op_DXYN(opcode);
            break;
        case 0xE: {
            uint16_t part = opcode & 0xF0FF;
            switch (part) {
                case 0xE09E:
                    _op_EX9E(opcode);
                    break;
                case 0xE0A1:
                    _op_EXA1(opcode);
                    break;
                default:
                    _op_unknown(opcode);
            }
            break;
        }
        case 0xF: {
            uint8_t part = opcode & 0x00FF;
            switch (part) {
                case 0x07:
                    _op_FX07(opcode);
                    break;
                case 0x0A:
                    _op_FX0A(opcode);
                    break;
                case 0x15:
                    _op_FX15(opcode);
                    break;
                case 0x18:
                    _op_FX18(opcode);
                    break;
                case 0x1E:
                    _op_FX1E(opcode);
                    break;
                case 0x29:
                    _op_FX29(opcode);
                    break;
                case 0x33:
                    _op_FX33(opcode);
                    break;
                case 0x55:
                    _op_FX55(opcode);
                    break;
                case 0x65:
                    _op_FX65(opcode);
                    break;
                default:
                    _op_unknown(opcode);
            }
            break;
        }
    }
}

void Chip8::_op_00E0(uint16_t opcode) {  // Display - disp_clear()       Clears the screen.
    memset(_display_buffer, 0, CHIP8_DISPLAY_BUFFER_SIZE);
    _PC += 2;
}
void Chip8::_op_00EE(uint16_t opcode) {  // Flow - return;               Returns from a subroutine.
    if (_stack_pointer == 0) {
        stop_execution();
        return;
    }
    _PC = _stack[--_stack_pointer];
}
void Chip8::_op_1NNN(uint16_t opcode) {  // Flow - goto NNN;             Jumps to address NNN.
    uint16_t dest = (opcode & 0x0FFF) - CHIP8_PC_OFFSET;
    _PC = dest;
}
void Chip8::_op_2NNN(uint16_t opcode) {  // Flow - *(0xNNN)()            Calls subroutine at NNN.
    if (_stack_pointer+1 >= CHIP8_STACK_DEPTH) _throw("Stack overflowed!");
    uint16_t dest = (opcode & 0x0FFF) - CHIP8_PC_OFFSET;
    _stack[_stack_pointer++] = _PC + 2;
    _PC = dest;
}
void Chip8::_op_3XNN(uint16_t opcode) {  // Cond - if (Vx == NN)         Skips the next instruction if VX equals NN (usually the next instruction is a jump to skip a code block).
    uint8_t x = (opcode & 0x0F00) >> 16;
    uint8_t nn = opcode & 0x00FF;
    _PC += 2 + 2*(_V[x] == nn);
}
void Chip8::_op_4XNN(uint16_t opcode) {  // Cond - if (Vx != NN)         Skips the next instruction if VX does not equal NN (usually the next instruction is a jump to skip a code block).
}
void Chip8::_op_5XY0(uint16_t opcode) {  // Cond - if (Vx == Vy)         Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block).
}
void Chip8::_op_6XNN(uint16_t opcode) {  // Const - Vx = NN              Sets VX to NN.
}
void Chip8::_op_7XNN(uint16_t opcode) {  // Const - Vx += NN             Adds NN to VX (carry flag is not changed).
}
void Chip8::_op_8XY0(uint16_t opcode) {  // Assig - Vx = Vy              Sets VX to the value of VY.
}
void Chip8::_op_8XY1(uint16_t opcode) {  // BitOp - Vx |= Vy             Sets VX to VX or VY. (bitwise OR operation).
}
void Chip8::_op_8XY2(uint16_t opcode) {  // BitOp - Vx &= Vy             Sets VX to VX and VY. (bitwise AND operation).
}
void Chip8::_op_8XY3(uint16_t opcode) {  // BitOp - Vx ^= Vy             Sets VX to VX xor VY.
}
void Chip8::_op_8XY4(uint16_t opcode) {  // Math - Vx += Vy              Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
}
void Chip8::_op_8XY5(uint16_t opcode) {  // Math - Vx -= Vy              VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
}
void Chip8::_op_8XY6(uint16_t opcode) {  // BitOp - Vx >>= 1             Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF.
}
void Chip8::_op_8XY7(uint16_t opcode) {  // Math - Vx = Vy - Vx          Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
}
void Chip8::_op_8XYE(uint16_t opcode) {  // BitOp - Vx <<= 1             Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset.
}
void Chip8::_op_9XY0(uint16_t opcode) {  // Cond - if (Vx != Vy)         Skips the next instruction if VX does not equal VY. (Usually the next instruction is a jump to skip a code block).
}
void Chip8::_op_ANNN(uint16_t opcode) {  // MEM - I = NNN                Sets I to the address NNN.
}
void Chip8::_op_BNNN(uint16_t opcode) {  // Flow - PC = V0 + NNN         Jumps to the address NNN plus V0.
}
void Chip8::_op_CXNN(uint16_t opcode) {  // Rand - Vx = rand() & NN      Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
}
void Chip8::_op_DXYN(uint16_t opcode) {  // Display - draw(Vx, Vy, N)    Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.
}
void Chip8::_op_EX9E(uint16_t opcode) {  // KeyOp - if (key() == Vx)     Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block).
}
void Chip8::_op_EXA1(uint16_t opcode) {  // KeyOp - if (key() != Vx)     Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block).
}
void Chip8::_op_FX07(uint16_t opcode) {  // Timer - Vx = get_delay()     Sets VX to the value of the delay timer.
}
void Chip8::_op_FX0A(uint16_t opcode) {  // KeyOp - Vx = get_key()       A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing).
}
void Chip8::_op_FX15(uint16_t opcode) {  // Timer - delay_timer(Vx)      Sets the delay timer to VX.
}
void Chip8::_op_FX18(uint16_t opcode) {  // Sound - sound_timer(Vx)      Sets the sound timer to VX.
}
void Chip8::_op_FX1E(uint16_t opcode) {  // MEM - I += Vx                Adds VX to I. VF is not affected.
}
void Chip8::_op_FX29(uint16_t opcode) {  // MEM - I = sprite_addr[Vx]    Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font.
}
void Chip8::_op_FX33(uint16_t opcode) {  // BCD - set_BCD(Vx)            Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
}
void Chip8::_op_FX55(uint16_t opcode) {  // MEM - reg_dump(Vx, &I)       Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
}
void Chip8::_op_FX65(uint16_t opcode) {  // MEM - reg_load(Vx, &I)       Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.
}

void Chip8::_op_unknown(uint16_t opcode) {
}

void Chip8::_throw(string message) {
    stop_execution();
    cerr << "Execution at 0x" << hex << _PC << dec << " threw: " << message << "\n";
}
