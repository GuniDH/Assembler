#include "header.h"


/* Calls the assembler function for each of the files on the argument line */
void assemble_files(int file_count, char* file_names[]){
	int i;
	for(i = 1; i<file_count;i++){
		printf("\nFile number %d named %s\n\n", i, file_names[i]);
		assembler(file_names[i]);
	}
}

/* Assembels each file */
int assembler(char*file_name){
	FILE*fp;
	FILE*entry_fp;
	FILE*ob_fp;
	FILE*extern_fp;
	CELL* cells;
	CELL* data_array;
	PassResult* result;
	char** label_table_name;
	char*file_name_cpy;
	int* label_table_num;
	int* label_table_type;
	int label_table_size, i, address, data_size, cells_size;
	file_name_cpy=(char*)malloc((strlen(file_name)+4)*sizeof(char));/* add 4 spaces for extension */
	if(file_name_cpy==NULL){	/* we use a copy for the file name in order to be able to add different extensions to it when opening files and such */
		fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
		return 0;
	}
	strcpy(file_name_cpy,file_name); /* each time after changing the extension of the copy of file name, we need to re-copy the original name to the copy */
	strcat(file_name_cpy,".as");	/* add extension to file name */
	fp = fopen(file_name_cpy,"r"); /* open source code file */
	if (fp == NULL) {
    	fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
    	return 0;
	}
	strcpy(file_name_cpy,file_name); /* the pre_assembler function needs a copy of the file name in order to create the deployed file */
	if((fp = pre_assembler(fp,file_name_cpy))==NULL)return 0; /* pre assemble the file */
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".am");
	fp = fopen(file_name_cpy,"r"); /* open the deployed file the pre assembler created */
	if (fp == NULL) {
    	fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
    	return 0;
	}
	if((result = first_pass(fp))==NULL)return 0; /* go on the first pass of the assembler */
	strcpy(file_name_cpy,file_name); /* the pre_assembler function needs a copy of the file name in order to create the externs file */
	if((result=second_pass(fp,file_name_cpy,result))==NULL)return 0; /* go on the first pass of the assembler */
	/* get data from second pass */
	label_table_name = result->label_table_name;
	label_table_num = result->label_table_num;
	label_table_type = result->label_table_type;
	label_table_size = result->label_table_size;
	cells = result->cells;
	cells_size=result->cells_size;
	data_array=result->data_array;
	data_size=result->data_size;
	if((cells_size+data_size)>(MAX_CELLS-99)){ /* The program starts from address 100 so we have space for 157 cells because the max amount is 256 */
		fprintf(stdout, "%s\n", "ERROR. The assembly program takes too much memory.");
    	return 0;
	}
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".ent");
	entry_fp = fopen(file_name_cpy, "w");
	if (entry_fp == NULL) {
		fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
		return 0;
	}
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".ob");
	ob_fp=fopen(file_name_cpy,"w");
	if (ob_fp == NULL) {
		fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
		return 0;
	}
	fprintf(ob_fp,"\t\t%d %d\n",  cells_size, data_size);
	for(i = 0; i < label_table_size;i++){ /* write to the entry file */
		if(label_table_type[i]==ENTRY){
			fprintf(entry_fp, "%s %d \n",label_table_name[i],label_table_num[i]+100);
		}
	}
	address = 100;
	for(i = 0; i < cells_size;i++){ /* write to the object file */
		if(address<999)fputc('0',ob_fp);
		fprintf(ob_fp,"%d\t",address++);
		cell_to_file(ob_fp,cells[i]);
	}
	for(i=0;i<data_size;i++){
		if(address<999)fputc('0',ob_fp);
		fprintf(ob_fp,"%d\t", address++);
		cell_to_file(ob_fp, data_array[i]);
	}
	fclose(entry_fp);
	fclose(ob_fp);
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".ent");
	entry_fp = fopen(file_name_cpy, "r");
	if (entry_fp == NULL) {
    	fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
    	return 0;
	}
	if(fgetc(entry_fp)==EOF){ /* check if entry file is empty, if yes remove it */
		remove(file_name_cpy);
	}
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".ob");
	ob_fp=fopen(file_name_cpy,"r");
	if (ob_fp == NULL) {
    	fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
    	return 0;
    }
	strcpy(file_name_cpy,file_name);
	strcat(file_name_cpy,".ext");
	extern_fp=fopen(file_name_cpy,"r");
	if (extern_fp == NULL) {
    	fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
    	return 0;
    }
	if(fgetc(extern_fp)==EOF){ /* check if extern file is empty, if yes remove it */
		remove(file_name_cpy);
	}
	fclose(entry_fp);
	fclose(ob_fp);
	fclose(extern_fp);
	if(data_array!=NULL)free(data_array); /* free memory */
	if(cells!=NULL)free(cells);
	if(result!=NULL)free(result);
	if(file_name_cpy!=NULL)free(file_name_cpy);
	if(label_table_num!=NULL)free(label_table_num);
	if(label_table_type!=NULL)free(label_table_type);
	if(label_table_name!=NULL)free_string_array(label_table_name,label_table_size);
	return 1;
}
