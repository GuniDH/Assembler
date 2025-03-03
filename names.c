#include "header.h"

/*The function checks if any macro name is in the line*/
int is_macro_name_in_line(char** macro_names, int macro_name_count, char line[]) {
    int i;
    for (i = 0; i < macro_name_count; i++)
        if (strstr(line, macro_names[i]))
            return i;
    return MACRO_NAME_NOT_FOUND;
}

/*The function checks if the string label_name is an actual label in the code*/
int is_label_in_table(char* label_name, char** label_table_name, int label_table_size)
{
	int i;
	for (i = 0; i < label_table_size; i++) {
		if (!strcmp(label_table_name[i], label_name))
			return i;
	}
	return -1;
}

/*The function inserts label infromation to the tables of labels that include different information of labels (type name and num which is their value) */
int insert_label_to_table(int* label_table_num, int num, int* label_table_type, int type, char** label_table_name, char* name, int label_table_size) {
	label_table_num[label_table_size - 1] = num;
	label_table_type[label_table_size - 1] = type;
	if(label_table_name[label_table_size - 1]==NULL){
		label_table_name[label_table_size - 1] = (char*)calloc(strlen(name) , sizeof(char));
		if (label_table_name[label_table_size - 1] == NULL) {
			fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
			return 1;
		}
	}
	label_table_name[label_table_size - 1]=name;
	return 0;
}

/*The function checks if the word c is an operation. if it is, it returns its index in op_table, and if not it'll return -1*/
int op_pos(char* c)
{
	const char* op_table[] = { "mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop" };
	int i = 0;
	for (i = 0; i < OP_TABLE_LEN; i++)
	{
		if (!strcmp(c, op_table[i]))
		{
			return i;
		} /* return the opcode in decimal base. if the string isn't an operation name, -1 will be returned. */
	}
	return -1;
}
