# Custom Assembler Project by Guni

## Project Overview
This project implements an assembler for a simple assembly language. The assembler translates assembly language code into machine code for a hypothetical computer.

## The Assembly Language and Computer Model

### Computer Hardware
The computer in this project is composed of:
- **CPU** (Central Processing Unit)
- **8 Registers**: r0, r1, r2, r3, r4, r5, r6, r7
  - Each register is 14 bits wide
- **Memory**: 256 cells (addresses 0-255)
  - Each memory cell is 14 bits wide, also called a "word"

## Assembler Operation

The assembler processes the assembly language in two passes and pre-assembler:
1.Pre-assember pass to withdraw macros
1. First pass: Collects information about labels and their addresses and translate most instructions to the machine code
2. Second pass: Translates the rest (Labels which are macros, or labels that were defined after hte instruction)

The assembler generates output files with the machine code representation of the program - object file, entries and externs.

## Instruction Format

Each instruction is encoded in a memory word (14 bits) with the following structure:

| Bits 0-1 | Bit 2 | Bit 3 | Bits 4-5 | Bits 6-9 | Bits 10-13 |
|----------|-------|-------|----------|----------|------------|
| E,R,A    | Destination register | Source register | Opcode | Parameter 1 | Parameter 2 |

- **A,R,E bits (0-1)** indicate addressing mode:
  - A (bit 0): Absolute addressing (value 0)
  - R (bit 1): External addressing (value 1)
  - E (bit 2): Relocatable addressing (value 2)
- **Bits 2-3** define the destination operand address
- **Bits 4-5** define the source operand address
- **Bits 6-9** contain the opcode
- **Bits 10-13** are used for parameters when needed

## Instruction Set

The assembler supports 16 operation codes (opcodes):

| Opcode (decimal) | Instruction | Description |
|------------------|-------------|-------------|
| 0 | mov | Move - Copies content from source to destination |
| 1 | cmp | Compare - Compares values between two operands |
| 2 | add | Add - Adds source to destination |
| 3 | sub | Subtract - Subtracts source from destination |
| 4 | not | Not - Logical NOT operation |
| 5 | clr | Clear - Sets operand to zero |
| 6 | lea | Load Effective Address - Loads address into register |
| 7 | inc | Increment - Increases operand by 1 |
| 8 | dec | Decrement - Decreases operand by 1 |
| 9 | jmp | Jump - Unconditional jump to address |
| 10 | bne | Branch if Not Equal - Jump if result is not zero |
| 11 | red | Read - Reads input from standard input |
| 12 | prn | Print - Prints operand to standard output |
| 13 | jsr | Jump to Subroutine - Jumps to subroutine |
| 14 | rts | Return from Subroutine - Returns from subroutine |
| 15 | stop | Stop - Terminates program execution |

## Instruction Categories

Instructions are divided into three categories:

1. **First Category**: mov, cmp, add, sub, lea
2. **Second Category**: not, clr, inc, dec, jmp, bne, jsr
3. **Third Category**: rts, stop

## Addressing Modes

The following addressing modes are supported:

1. **Immediate Addressing** (#):
   - Example: `mov #-1, r1` - Move the value -1 to register r1

2. **Direct Addressing**:
   - Example: `mov HELLO, r1` - Move the content from memory location HELLO to register r1

3. **Register Addressing**:
   - Example: `mov r1, r2` - Move the content of register r1 to register r2

4. **Jump Instructions**:
   - Standard: `jmp LINE` - Jump to the label LINE
   - With parameters: `jmp LINE(r6-6, r4)` - Jump with register parameters

5. **Subroutine Operations**:
   - Call: `jsr FUNC` - Jump to subroutine FUNC
   - Call with parameters: `jsr FUNC(#75, X)` - Call with immediate and label parameters
   - Return: `rts` - Return from subroutine

## Examples of Assembly Instructions

```
; Register operations
mov r1, r2     ; Move content from r1 to r2
cmp r2, r1     ; Compare r2 with r1

; Immediate value operations
mov #-1, r1    ; Move the value -1 to r1
cmp #5, r2     ; Compare 5 with the content of r2

; Memory operations
lea HELLO, r1  ; Load the address of HELLO into r1
mov X, r1      ; Move the content from memory location X to r1

; Jump operations
jmp LINE       ; Jump to LINE
jmp LINE(r6-6,r4) ; Jump with calculation using registers
bne LINE       ; Branch to LINE if result is not zero
bne LINE(X,r4) ; Branch with parameters

; Subroutine operations
jsr FUNC       ; Jump to subroutine FUNC
jsr FUNC(#75,X) ; Jump to subroutine with parameters
rts            ; Return from subroutine

; System operations
stop           ; Stop program execution
```
