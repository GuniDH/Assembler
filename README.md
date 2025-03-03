# Custom Assembler Project by Guni

## Overview

This project is a **custom assembler** for a **hypothetical 16-bit computer architecture**. It translates assembly language source files (`.as`) into machine code. The assembler follows a **two-pass assembly process**, ensuring correct resolution of labels, symbols, and memory references. 

The target machine has **8 general-purpose registers**, a **256-word memory**, and a **stack**. Instructions are encoded in a structured format, supporting multiple addressing modes.

## Features

- **Two-Pass Assembler**: First pass resolves symbols and labels; second pass generates machine code.
- **Instruction Set Implementation**: Handles various operations including arithmetic, logic, control flow, and I/O.
- **Symbol Table Management**: Supports `.entry` and `.extern` directives for linking.
- **Error Detection**: Identifies syntax errors, missing labels, incorrect addressing modes, and more.
- **File Processing**: Reads `.as` assembly files and outputs machine code for execution.

## Instruction Set

The assembler supports the following instructions, each mapped to a specific opcode:

### **1. Data Movement Instructions**
- `mov src, dest` - Moves data from `src` to `dest`.
- `lea label, reg` - Loads the address of `label` into `reg`.
- `not reg` - Performs bitwise NOT on `reg`.
- `clr reg` - Clears the value of `reg` (sets it to zero).

### **2. Arithmetic Instructions**
- `add src, dest` - Adds `src` to `dest`.
- `sub src, dest` - Subtracts `src` from `dest`.
- `inc reg` - Increments `reg` by 1.
- `dec reg` - Decrements `reg` by 1.

### **3. Control Flow Instructions**
- `jmp label` - Unconditional jump to `label`.
- `bne label` - Branch if the last comparison was not equal.
- `jsr label` - Jumps to a subroutine at `label`.
- `rts` - Returns from a subroutine.
- `stop` - Halts execution.

### **4. Input/Output Instructions**
- `red reg` - Reads an integer from the user into `reg`.
- `prn operand` - Prints `operand` to the output.

## Addressing Modes

Each instruction can use different **addressing modes**:

| Mode         | Syntax       | Description |
|-------------|-------------|-------------|
| Immediate   | `#value`    | Uses a constant value (e.g., `mov #5, r1`) |
| Direct      | `label`     | Uses a memory location (e.g., `mov x, r2`) |
| Indexed     | `label[i]`  | Uses an address offset by `i` |
| Register    | `rX`        | Uses a register (e.g., `mov r1, r2`) |

## Assembly Directives

- `.data value1, value2, ...` - Defines a list of numeric values in memory.
- `.string "text"` - Stores a string in memory.
- `.entry label` - Marks `label` as accessible from outside.
- `.extern label` - Declares `label` as an external symbol.

## Example Assembly Code

```assembly
.data 10, 20, 30
.string "Hello"
.extern EXTERNAL_LABEL
.entry MAIN

MAIN:    mov r3, r2
         add #5, r1
         jmp END

END:     stop
```

## Technologies Used

- **C Language** for assembler implementation
- **File I/O Operations** for reading/writing assembly and machine code
- **Data Structures** for managing symbol tables and instruction parsing

## Installation & Usage

### 1. Clone the Repository
```sh
git clone https://github.com/GuniDH/Assembler.git
cd custom-assembler
```

### 2. Compile the Assembler
```sh
gcc -o assembler main.c assembler.c first_pass.c -Wall
```

### 3. Run the Assembler
```sh
./assembler input.as
```
This will process `input.as` and generate the corresponding machine code output.

## Error Handling

The assembler reports errors with precise **line numbers and descriptions**:

- **Syntax Errors**: Invalid instructions, missing operands.
- **Undefined Labels**: Jumping to a non-existent label.
- **Memory Violations**: Using an out-of-bounds address.
- **Invalid Addressing Modes**: Using unsupported operand types.

## Contributions

Contributions are welcome! Submit issues and pull requests to improve functionality or add new features.

## License

This project is licensed under the **MIT License**.

---
### Author
**Guni**  


