my_assembler: assembler.o pre_assembler.o first_pass.o second_pass.o syntax_errors.o syntax_check.o string_operations.o names.o memory.o cell_operations.o main.o
	gcc -Wall -ansi -pedantic assembler.o pre_assembler.o first_pass.o second_pass.o syntax_errors.o syntax_check.o string_operations.o names.o memory.o cell_operations.o main.o -o my_assembler

main.o: main.c header.h
	gcc -c -Wall -ansi -pedantic main.c -o main.o

assembler.o: assembler.c header.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

second_pass.o: second_pass.c header.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

first_pass.o: first_pass.c header.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

pre_assembler.o: pre_assembler.c header.h
	gcc -c -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o

syntax_errors.o: syntax_errors.c header.h
	gcc -c -Wall -ansi -pedantic syntax_errors.c -o syntax_errors.o

syntax_check.o: syntax_check.c header.h
	gcc -c -Wall -ansi -pedantic syntax_check.c -o syntax_check.o

string_operations.o: string_operations.c header.h
	gcc -c -Wall -ansi -pedantic string_operations.c -o string_operations.o

names.o: names.c header.h
	gcc -c -Wall -ansi -pedantic names.c -o names.o

memory.o: memory.c header.h
	gcc -c -Wall -ansi -pedantic memory.c -o memory.o

cell_operations.o: cell_operations.c header.h
	gcc -c -Wall -ansi -pedantic cell_operations.c -o cell_operations.o


