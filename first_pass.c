#include "header.h"

/*Throughout the funciton if something isnt a number/register, if it has label syntax we assume it is an actual label and check it appers in the label table in the second pass */
PassResult* first_pass(FILE* fp) {
	PassResult* result;
	int ic, dc, label_flag, label_table_size, error_flag,extern_entry_label_flag;/*extern_entry_label_flag refers to wether we have a label definition before .extern or .entry (for example-   L1: .extern W)*/
	int minus,curnum,i,opcode,L,line_cnt,data_flag;/*L is the number of instructions in the line*/
	const int op_param_amount[] = { 2,2,2,2,1,1,2,1,1,1,1,1,1,1,0,0 };
	char line[LINE_MAX_SIZE];/*will be used to read lines*/
	CELL* cells;/*holds all the contents of the code instructions in binray form*/
	CELL* data_array;/*holds all the contents of .data and .string in binary form*/
	char** label_table_name;/*will hold all the names of the labels*/
	int* label_table_num;/*will hold the number of the label as defined in the instructions*/
	int* label_table_type;/*the type of the label: data_relocatable, not_data_relocatable (label that is defined in the code and isnt data/entry),external , and entry (which wont be used in the first pass)*/
	char* c;
	char* word;
	char* label;
	char* param1;/*1st operand of operation line*/
	char* param2;/*2nd operand*/
	char* label_param1;/*1st parameter of addressing method 2*/
	char* label_param2;/*2nd parameter*/
	unsigned int mask;
	error_flag = F;
	line_cnt=0;
	ic = 0;
	dc = 0;
	result = (PassResult*)calloc(1,sizeof(PassResult));
	cells = (CELL*)calloc((ic + 1) , sizeof(CELL));
	label_table_size = 1;
	data_array = (CELL*)calloc((dc + 1) , sizeof(CELL));
	label_table_name = (char**)calloc(label_table_size , sizeof(char*));
	label_table_num = (int*)calloc(label_table_size , sizeof(int));
	label_table_type = (int*)calloc(label_table_size , sizeof(int));
	if (result == NULL || cells == NULL || label_table_name == NULL || label_table_num == NULL || label_table_type == NULL || data_array == NULL) {
		fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
		return NULL;
	}
	while ((c = fgets(line, LINE_MAX_SIZE, fp))) {
		label_flag=F;
		c = skip_spaces(c);
		line_cnt++;
		L = 0;
		extern_entry_label_flag=F;
		if (*c != ';' && *c != '\n') /* check if line is empty or comment */
		{
			label = read_until_token(c, SPACE_TOKEN);
			if(check_null(label,line_cnt))return NULL;
			if(strlen(label)==0){
				fprintf(stdout, "ERROR. Label name in line %d was not defined (only :).\n",line_cnt);
				error_flag=T;
				continue;
			}
			if(c[strlen(label)]==':'){/*if we have the char : we have a label definition (just need to check it has correct syntax)*/
				if(!is_label(label,line_cnt)){
					fprintf(stdout, "ERROR. Label name in line %d doesn't match correct syntax.\n",line_cnt);
					error_flag=T;
					continue;
				}
				label_flag=T;
			}
			if(is_label(label,line_cnt)&&(strlen(label)+1)<=strlen(c)&&c[strlen(label)]==' '&&c[strlen(label)+1]==':'){
				fprintf(stdout, "%s\n", "ERROR. Label name cannot have space between its name and ':' character.");
				error_flag=T;
				continue; /* in the forum they wrote that we can use break and continue */
			}
			c+= strlen(label)+1;
			c=skip_spaces(c);/*move c to the next word. if*/
			if (label_flag) {/*in here we make sure that word is the first word in the line that isnt a label definition*/
				word = read_until_token(c, SPACE_TOKEN);
				if(check_null(word,line_cnt))return NULL;
                c+= strlen(word)+1;
				c = skip_spaces(c);
			}
			else word = label;
			if (!strcmp(word, ".data") || !strcmp(word, ".string"))
			{
				if (label_flag) {
					if(is_label_in_table(label, label_table_name, label_table_size-1) != -1){
						fprintf(stdout, "ERROR. Label in line %d has already been defined.\n",line_cnt );
						return NULL;
					}
					insert_label_to_table(label_table_num, dc, label_table_type, DATA_RELOCATABLE, label_table_name, label, label_table_size);/*insert the label*/
				}
				
				if (!strcmp(word, ".data"))
				{
					data_flag = F;
					i=0;
					while (*word != '\0')
					{
						minus = F;/*minus checks if the number has minus sign*/
						word = read_until_token(c, COMMA_TOKEN);
						if(word==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						if(*word=='\0'){/*edge case*/
							if(i==0){
								fprintf(stdout, "ERROR. .data in line %d has not received arguments.\n",line_cnt );
								error_flag=T;
								continue;
							}
							else
								break;
						}
						c+=strlen(word)+1;
						c=skip_spaces(c);
						if(!is_number(word,T))/*check if word is a .data number*/
						{
							data_flag=T;
							break;
						}
						
						if(*word=='-'||*word=='+'){
							if(*word=='-')minus=T;
							word++;
						}
						curnum = atoi(word);
						data_array[dc]=minus ? num_to_cell(data_array[dc], MAX_NUM - curnum) : num_to_cell(data_array[dc], curnum);/*encodes the number to binary*/
						dc++;
						data_array = (CELL*)realloc(data_array, (dc + 1) * sizeof(CELL));
						if (data_array == NULL) {
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							error_flag = T;
						}
						i++;
					}
					if(data_flag){/*if one argument wasn't a number*/
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "Data has received an argument which isn't a number.");
						error_flag=T;
						continue;
					}
					if(!empty_line(c))/*another edge case.something that goes here: c=L1: .data 3,,5*/
					{
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "Commands aren't allowed after .data, and double-commas are forbidden.");
						error_flag=T;
						continue;
					}
					
				}
				else if (!strcmp(word, ".string"))
				{
					
					if(empty_line(c)){
						fprintf(stdout, "ERROR. .string in line %d has not received arguments.\n",line_cnt );
						error_flag=T;
						continue;
					}
					if(*c == '\"')c++; /* skip first quotation mark */
					else{
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "string should be defined with the \" character at the start of its definition.");
						error_flag =T;
						continue;
					}
					word = read_until_token(c,QUOTATION_TOKEN);
					if(word==NULL){
						fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
						return NULL;
					}
					c+=strlen(word);
					if(*c == '\"')c++;/*check we actually end the quotation mark*/
					else{
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "string should be defined with the \" character at the end of its definition.");
						error_flag =T;
						continue;
					}
					if(!empty_line(c))
					{
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "after .string there can only be 1 word.");
						error_flag=T;
						continue;
					}
					else{
						while (*word != '\"' &&  *word!='\0')
						{
							if((*word<' '||*word>'~')&&(!isspace(*word)))/*check it is an allowed character*/
							{
								fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "This isn't a legal word");
								error_flag=T;
								break;
							}
							data_array[dc]=num_to_cell(data_array[dc], (int)(*word));
							dc++;
							word++;
							data_array = (CELL*)realloc(data_array, (dc + 1) * sizeof(CELL));
							if(check_null(data_array,line_cnt))return NULL;
						}
					}
					data_array[dc]=num_to_cell(data_array[dc], 0);
					dc++;
					data_array = (CELL*)realloc(data_array, (dc + 1) * sizeof(CELL));
					if(check_null(data_array,line_cnt))return NULL;
				}
			}
			else if (!strcmp(word, ".entry") || !strcmp(word, ".extern")) {
				if (!strcmp(word, ".extern")) {
					word = read_until_token(c, SPACE_TOKEN);
					if(check_null(word,line_cnt))return NULL;
					c+=strlen(word)+1;
					if(!empty_line(c))
					{
						fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "after .extern or .entry there can only be 1 label.");
						error_flag=T;
					 	continue;
					}
					if(is_label_in_table(word, label_table_name, label_table_size-1) != -1){/*check there aren't multiple definitions of a label*/
						fprintf(stdout, "ERROR. Label in line %d has already been defined.\n",line_cnt );
						return NULL;
					}
					if(!is_label(word,line_cnt)){/*doesn't follow allowed label syntax*/
						fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
						error_flag=T;
						continue;
					}
					insert_label_to_table(label_table_num, 0, label_table_type, EXTERNAL, label_table_name, word, label_table_size);
					label_table_size++;
					label_table_name = (char**)realloc(label_table_name, label_table_size * sizeof(char*));
					label_table_num = (int*)realloc(label_table_num, label_table_size * sizeof(int));
					label_table_type = (int*)realloc(label_table_type, label_table_size * sizeof(int));
					if (label_table_name == NULL || label_table_num == NULL || label_table_type == NULL) {
						fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
						return NULL;
					}
				}
				if(label_flag){
					fprintf(stdout, "WARNING. Label in line %d before entry or extern.\n",line_cnt);
					extern_entry_label_flag=T;
				}
			}
			else {/*here we analyze operation lines*/
				if (label_flag)
				{
					if(is_label_in_table(label, label_table_name, label_table_size-1) != -1){
						fprintf(stdout, "ERROR. Label in line %d has already been defined.\n",line_cnt );
						return NULL;
					}
					insert_label_to_table(label_table_num, ic, label_table_type, NOT_DATA_RELOCATABLE, label_table_name, label, label_table_size);
				}
				opcode = op_pos(word);/*opcode of operation*/
				if (opcode == -1)
				{
					fprintf(stdout, "ERROR. Operation name does not exist in line %d.\n",line_cnt);
					error_flag=T;
					continue;
				}
				else
				{	
					cells[ic].bits=0;/*initiallize*/
					if (op_param_amount[opcode] == 1)
                    {
						param1 = read_until_token(c, LEFT_BRACKET_TOKEN);
						if(param1==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						word=read_until_token(c, ENTER_TOKEN);
						if(word==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						c += strlen(param1) + 1;
						c=skip_spaces(c);
						if (!strcmp(word, param1)) /* normal 1 argument, because we have no parentheses */
						{
							L = 2;
							if(!empty_line(c)){
								fprintf(stdout, "Error at line %d, cannot have commands or words after operation.\n",line_cnt);
								error_flag=T;
								continue;
							}
							if(is_number(param1,T)){/*check if we have number without #*/
								fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n",line_cnt);
								error_flag=T;
								continue;
							}
							if (is_number(param1,F))
							{
								if(arg_syntax(opcode,0,0,1,line_cnt))/*doesn't follow allowed syntax for that opcode*/
								{	
									
									error_flag=T;
									continue;
								}
							}
							else if (is_reg(param1)) {
								if(arg_syntax(opcode,0,3,1,line_cnt))
								{
									error_flag=T;
									continue;
								}
								cells[ic]=update_bit(cells[ic], 1, 2);/*register is addressing method 3*/
								cells[ic]=update_bit(cells[ic], 1, 3);
							}
							else if(!is_label(param1,line_cnt)){/*we get here if param1 isn't a number,isn't a register, and doesn't follow label syntax*/
								fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
								error_flag=T;
								continue;
							}
                            else{
								if(arg_syntax(opcode,0,1,1,line_cnt)){
									error_flag=T;
									continue;
								}
                                cells[ic]=update_bit(cells[ic], 1, 2);
                            }
						}
						else /* 1 argument which is a label with 2 parameters */
						{
							if(is_number(param1,T)){
								fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
								error_flag=T;
								continue;
							}
							if (is_number(param1,F))
							{
								if(arg_syntax(opcode,1,0,1,line_cnt)){
									error_flag=T;
									continue;
								}
							}
							else if (is_reg(param1)) {
								if(arg_syntax(opcode,1,3,1,line_cnt)){
									error_flag=T;
									continue;
								}
							}
							else if(!is_label(param1,line_cnt)){
								fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
								error_flag=T;
								continue;
							}
                            else{
								if(arg_syntax(opcode,1,1,1,line_cnt)){
									error_flag=T;
									continue;
								}
                            }
							cells[ic]=update_bit(cells[ic], 1, 3);
							label_param1 = read_until_token(c, COMMA_TOKEN);
							if(label_param1==NULL){
								fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
								return NULL;
							}
							if (is_reg(label_param1)) {
								cells[ic]=update_bit(cells[ic], 1, 12);
								cells[ic]=update_bit(cells[ic], 1, 13);
							}
							else if (is_number(label_param1,F) || is_number(label_param1,T)) {
								if(is_number(label_param1,T)){
									fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
									error_flag=T;
									continue;
								}
							}
							else if(!is_label(label_param1,line_cnt)){
								fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
								error_flag=T;
								continue;
							}
                            else{
                                cells[ic]=update_bit(cells[ic], 1, 12);

                            }
							c += strlen(label_param1) + 1;
							if(isspace(*(c-2))||isspace(*c)||*c=='\0'){
								fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "it isn't an argument");
								error_flag=T;
								continue;
							}
							label_param2 = read_until_token(c, RIGHT_BRACKET_TOKEN);
							if(label_param2==NULL){
								fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
								return NULL;
							}
							if (is_reg(label_param2)) {
								cells[ic]=update_bit(cells[ic], 1, 10);
								cells[ic]=update_bit(cells[ic], 1, 11);
							}
							else if (is_number(label_param2,F) || is_number(label_param2,T)) {
								if(is_number(label_param2,T)){
									fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
									error_flag=T;
									continue;
								}
							}
							else if(!is_label(label_param2,line_cnt)){
								fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
								error_flag=T;
								continue;
							}
                            else{
                                cells[ic]=update_bit(cells[ic], 1, 10);
                            }
							c+=strlen(label_param2)+1;
							if(*(c-1)!=')' && !empty_line(c))
							{
								fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "it isn't an argument");
								error_flag=T;
								continue;
							}
							if (is_reg(label_param1) && is_reg(label_param2))L = 3;/*if we have 2 registers we need one less insturction line*/
							else L = 4;
							if(!empty_line(c)){
								fprintf(stdout, "Error at line %d, cannot have commands or words after operation.\n",line_cnt);
								error_flag=T;
								continue;
							}
						}
					}
					else if (op_param_amount[opcode] == 2) {/*operations like mov go here*/
						param1 = read_until_token(c, COMMA_TOKEN);
						if(param1==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						if(is_number(param1,T)){
							fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
							error_flag=T;
							continue;
						}
						if (is_number(param1,F)) {
							if(arg_syntax(opcode,0,0,0,line_cnt)){
								error_flag=T;
								continue;
							}
                        }
						else if (is_reg(param1)) {
							if(arg_syntax(opcode,0,3,0,line_cnt)){
								error_flag=T;
								continue;
							}
							cells[ic]=update_bit(cells[ic], 1, 4);
							cells[ic]=update_bit(cells[ic], 1, 5);
						}
						else if(!is_label(param1,line_cnt)){
							fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
							error_flag=T;
							continue;
						}
                        else{ 
							if(arg_syntax(opcode,0,1,0,line_cnt)){
								error_flag=T;
								continue;
							}
                            cells[ic]=update_bit(cells[ic], 1, 4);
                        }
						c += strlen(param1) + 1;
						if(empty_line(c))
						{
							fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "it isn't an argument");
							error_flag=T;
							continue;
						}
						param2 = read_until_token(c, SPACE_TOKEN);
						if(param2==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						c+=strlen(param2)+1;
						if(is_number(param2,T)){
							fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
							error_flag=T;
							continue;
						}
						if (is_number(param2,F))
						{
							if(arg_syntax(opcode,0,0,1,line_cnt)){
								error_flag=T;
								continue;
							}
						}
						else if (is_reg(param2)) {
							if(arg_syntax(opcode,0,3,1,line_cnt)){
								error_flag=T;
								continue;
							}
							cells[ic]=update_bit(cells[ic], 1, 2);
							cells[ic]=update_bit(cells[ic], 1, 3);
						}
						else if(!is_label(param2,line_cnt)){
							fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n",line_cnt );
							error_flag=T;
							continue;
						}
                        else{
							if(arg_syntax(opcode,0,1,1,line_cnt)){
								error_flag=T;
								continue;
							}
                            cells[ic]=update_bit(cells[ic], 1, 2);
                        }
						if(!empty_line(c))
						{
							fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "it isn't an argument");
							error_flag=T;
							continue;
						}
						if (is_reg(param2) && is_reg(param1))L = 2;
						else L = 3;
					}
					else/*if we have operations with no operands*/
					{
						if(!empty_line(c))
						{
							fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "it isn't an argument");
							error_flag=T;
							continue;
						}
						L = 1;
					}
					i = 6;
					for (mask = 1; i<10;i++) { /* encoding 6-9 bits with opcode*/
						if (opcode & mask)
						{
							cells[ic]=update_bit(cells[ic], 1, i);
						}
						mask*=2;
					}
					ic += L;
					
				}
			}
			if(label_flag&&!extern_entry_label_flag){
				label_table_size++;
				label_table_name = (char**)realloc(label_table_name, label_table_size * sizeof(char*));
				label_table_num = (int*)realloc(label_table_num, label_table_size * sizeof(int));
				label_table_type = (int*)realloc(label_table_type, label_table_size * sizeof(int));
				if (label_table_name == NULL || label_table_num == NULL || label_table_type == NULL) {
					fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
					error_flag=T;
				}
			}
			cells = (CELL*)realloc(cells, (ic + 1) * sizeof(CELL));
			if (cells == NULL) {
				fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
				error_flag = T;
			}
		}
	}
	if(error_flag)return NULL; /* dont continue to second pass if errors werent on first*/
	label_table_size--;
	for (i = 0; i < label_table_size; i++) {/*add ic to every label which is data type*/
		if (label_table_type[i] == DATA_RELOCATABLE) {
			label_table_num[i] += ic;
		}
	}
	result->label_table_name = label_table_name;
	result->label_table_type = label_table_type;
	result->label_table_num = label_table_num;
	result->label_table_size = label_table_size;
	result->cells = cells;
	result->cells_size=ic;
	result->data_array=data_array;
	result->data_size=dc;
	if(label!=NULL)free(label); /* free memory */
	if(param1!=NULL)free(param1);
	if(param2!=NULL)free(param2);
	if(label_param1!=NULL)free(label_param1);
	if(label_param1!=NULL)free(label_param2);
	return result;
}
