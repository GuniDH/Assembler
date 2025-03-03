#include "header.h"


/*

	Assembler program
	Authors: Guni Dio Hannes (id 215615519) and Amir Lavi (id 328334966)

*/

/* 
	DISCLAIMER

	Following the algorithm that we were instructed to do, some errors in the program are found only in a certain level of the code.
	Therefore, for example, there can be errors in the first pass that will prevent the program from reporting on other errors
	that can be found just in the second pass. But without those specific errors, the other errors will be found as well.
*/


int main(int argc, char* argv []){
	if(argc < 2){
		fprintf(stdout, "%s\n", "ERROR. No files were given.");
		return 1;
	}
	assemble_files(argc, argv);
	return 0;
}


