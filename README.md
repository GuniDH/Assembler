# Assembler by Guni

## Overview

This project is a **custom assembler** designed to translate a **hypothetical assembly language** into machine code. It follows a **two-pass assembly process**, ensuring proper handling of labels, instructions, and directives. This assembler is designed for a custom **16-bit architecture**, meaning that all instructions, registers, and memory operations adhere to this framework. The assembler converts assembly files (`.as`) into binary output that can be loaded into an emulator or used directly in hardware simulation environments.

This project is a **custom assembler** designed to translate assembly language source files (`.as`) into machine code. It follows a **two-pass assembly process**, ensuring proper handling of labels, instructions, and directives.

## Features

- **Custom Assembly Language**: Implements a structured instruction set for arithmetic, branching, and memory operations.
- **Two-Pass Assembler**: First pass resolves labels and symbols, second pass generates machine code.
- **Symbol Table Management**: Handles labels, variables, and memory addressing.
- **Instruction Parsing**: Supports various directives and operations for a complete assembly workflow.
- **Error Handling**: Detects and reports syntax errors, undefined labels, and incorrect instructions.
- **File Input and Output**: Reads `.as` assembly files and generates corresponding machine code output.

- **Two-Pass Assembler**: Processes the source code in two passes to resolve symbols and generate accurate machine code.
- **Symbol Table Management**: Efficient handling of labels and memory locations.
- **Instruction Parsing**: Supports a structured assembly language with various directives.
- **Error Handling**: Detects and reports syntax errors, undefined labels, and incorrect instructions.
- **File Input and Output**: Reads `.as` assembly files and generates corresponding machine code output.

## Instruction Set

The assembler supports a range of operations categorized as follows:

### **General Information About the Assembly Language**
This assembler works with a **16-bit word size** and a **fixed instruction format**. The instruction encoding follows this structure:

| Bit Position | Description |
|-------------|-------------|
| 0-3         | Opcode (instruction type) |
| 4-7         | Source Register (if applicable) |
| 8-11        | Destination Register (if applicable) |
| 12-15       | Immediate Value or Memory Address (if applicable) |

Each instruction fits within this structure, and specific operations determine how bits are interpreted.

### **1. Arithmetic Operations**
Arithmetic instructions modify register values and include operations such as:

- `ADD R1, R2` – Adds contents of `R1` and `R2`, storing result in `R1`.
- `SUB R1, R2` – Subtracts `R2` from `R1`.
- `MUL R1, R2` – Multiplies `R1` by `R2`.
- `DIV R1, R2` – Divides `R1` by `R2`.
- `INC R1` – Increments `R1` by 1.
- `DEC R1` – Decrements `R1` by 1.

### **2. Data Movement**
These instructions transfer data between registers and memory locations:

- `MOV R1, R2` – Copies data from `R2` to `R1`.
- `LOAD R1, LABEL` – Loads value from memory address of `LABEL` into `R1`.
- `STORE R1, LABEL` – Stores value from `R1` into memory address of `LABEL`.
- `PUSH R1` – Pushes register `R1` onto the stack.
- `POP R1` – Pops the top of the stack into `R1`.

### **3. Branching & Control Flow**
These instructions alter the flow of execution based on conditions:

- `JMP LABEL` – Jumps to the given label.
- `BNE R1, R2, LABEL` – Branches to `LABEL` if `R1 != R2`.
- `BEQ R1, R2, LABEL` – Branches to `LABEL` if `R1 == R2`.
- `CALL LABEL` – Calls a subroutine at `LABEL` (saves return address).
- `RET` – Returns from a subroutine.

### **4. Logical & Bitwise Operations**
- `AND R1, R2` – Performs bitwise AND between `R1` and `R2`.
- `OR R1, R2` – Performs bitwise OR.
- `XOR R1, R2` – Performs bitwise XOR.
- `NOT R1` – Performs bitwise NOT on `R1`.
- `SHL R1, VALUE` – Shifts `R1` left by `VALUE` bits.
- `SHR R1, VALUE` – Shifts `R1` right by `VALUE` bits.

### **5. Directives**
- `.data VALUE` – Defines memory space initialized with `VALUE`.
- `.string "TEXT"` – Stores a string in memory.
- `.extern SYMBOL` – Declares an external symbol.
- `.entry SYMBOL` – Marks `SYMBOL` as an entry point.
- `.org ADDRESS` – Specifies the starting address for program execution.

The assembler supports a range of operations categorized as follows:

### **1. Arithmetic Operations**
- `ADD R1, R2` – Adds contents of `R1` and `R2`, storing result in `R1`.
- `SUB R1, R2` – Subtracts `R2` from `R1`.
- `MUL R1, R2` – Multiplies `R1` by `R2`.
- `DIV R1, R2` – Divides `R1` by `R2`.

### **2. Data Movement**
- `MOV R1, R2` – Copies data from `R2` to `R1`.
- `LOAD R1, LABEL` – Loads value from memory address of `LABEL` into `R1`.
- `STORE R1, LABEL` – Stores value from `R1` into memory address of `LABEL`.

### **3. Branching & Control Flow**
- `JMP LABEL` – Jumps to the given label.
- `BNE R1, R2, LABEL` – Branches to `LABEL` if `R1 != R2`.
- `BEQ R1, R2, LABEL` – Branches to `LABEL` if `R1 == R2`.

### **4. Directives**
- `.data VALUE` – Defines memory space initialized with `VALUE`.
- `.string "TEXT"` – Stores a string in memory.
- `.extern SYMBOL` – Declares an external symbol.
- `.entry SYMBOL` – Marks `SYMBOL` as an entry point.

## Technologies Used

- **C Language** (for assembler implementation)
- **File I/O Operations** (for reading/writing assembly and machine code files)
- **Data Structures** (for managing symbol tables and instruction sets)

## Prerequisites

Ensure you have the following installed:

- **C Compiler** (GCC/Clang)
- **Make** (if using a Makefile for compilation)

## Installation & Usage

### 1. Clone the Repository

```sh
git clone https://github.com/GuniDH/Assembler.git
cd custom-assembler
```

### 2. Compile the Project

```sh
gcc -o assembler main.c assembler.c first_pass.c cell_operations.c -Wall
```

### 3. Run the Assembler

```sh
./assembler input1.as
```

This will process `input1.as` and generate the corresponding machine code output.

## Error Handling

The assembler implements **detailed error detection** to ensure correct assembly. Common errors include:

### **1. Syntax Errors**
- Invalid instructions (`XYZ R1, R2` → Unknown opcode `XYZ`)
- Missing operands (`ADD R1` → Expected `ADD R1, R2`)
- Incorrect addressing mode (`LOAD 5, R1` → Invalid operand order)

### **2. Undefined Labels & Symbols**
- Using labels before defining them (`JMP UNDEFINED_LABEL`)
- Forgetting to declare `.extern` or `.entry` symbols

### **3. Memory Errors**
- Accessing out-of-bounds memory locations
- Stack overflow/underflow detection in `PUSH` and `POP` instructions

Errors are reported with precise **line numbers and descriptions**, making debugging easier.

If an error is detected in the assembly file, the assembler will report the issue with line numbers and error descriptions.

## Contributions

Contributions are welcome! Submit issues and pull requests to improve functionality or add new features.

## License

This project is licensed under the **MIT License**.

---

### Author
**Guni**
