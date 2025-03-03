# Custom Assembler Project

## Overview
This project is a **custom assembler** designed to translate assembly language source files (`.as`) into machine code. It follows a **two-pass assembly process**, ensuring proper handling of labels, instructions, and directives.

## Features
- **Two-Pass Assembler**: Processes the source code in two passes to resolve symbols and generate accurate machine code.
- **Symbol Table Management**: Efficient handling of labels and memory locations.
- **Instruction Parsing**: Supports a structured assembly language with various directives.
- **Error Handling**: Detects and reports syntax errors, undefined labels, and incorrect instructions.
- **File Input and Output**: Reads `.as` assembly files and generates corresponding machine code output.

## Technologies Used
- **C Language** (for assembler implementation)
- **File I/O Operations** (for reading/writing assembly and machine code files)
- **Data Structures** (for managing symbol tables and instruction sets)

## Project Structure
```
Assembler_Project/
│── assembler.c
│── main.c
│── first_pass.c
│── cell_operations.c
│── header.h
│── input1.as
│── input2.as
│── input3.as
│── input4.as
│── input_output_example.png
```

## Prerequisites
Ensure you have the following installed:
- **C Compiler** (GCC/Clang)
- **Make** (if using a Makefile for compilation)

## Installation & Usage
### 1. Clone the Repository
```sh
git clone https://github.com/yourusername/custom-assembler.git
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

## Example Input/Output
Refer to `input_output_example.png` for a visual representation of input assembly and expected output.

## Error Handling
If an error is detected in the assembly file, the assembler will report the issue with line numbers and error descriptions.

## Contributions
Contributions are welcome! Submit issues and pull requests to improve functionality or add new features.

## License
This project is licensed under the **MIT License**.

---
### Author
**Guni**  
