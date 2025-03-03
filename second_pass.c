#include "header.h"

PassResult* second_pass(FILE* fp,char*file_name,PassResult* result) {
	FILE* extern_fp;
	int ic, label_flag, label_table_size, i , opcode, L, data_size, cells_size, line_cnt,error_flag;
	const int op_param_amount[] = { 2,2,2,2,1,1,2,1,1,1,1,1,1,1,0,0 };
	char line[LINE_MAX_SIZE];
	CELL* cells;
	CELL* data_array;
	char** label_table_name;
	int* label_table_num;
	int* label_table_type;
	char* c;
	char* word;
	char* label;
	char* param1;
	char* param2;
	char* label_param1;
	char* label_param2;
	error_flag=F;
	line_cnt=0;
	rewind(fp);
	extern_fp = fopen(strcat(file_name,".ext"), "w"); /* open file for externs */
	label_table_name = result->label_table_name;
	label_table_num = result->label_table_num;
	label_table_type = result->label_table_type;
	label_table_size = result->label_table_size;
	cells = result->cells;
	cells_size=result->cells_size;
	data_size=result->data_size;
	data_array=result->data_array;
	ic = 0;
	while ((c = fgets(line, LINE_MAX_SIZE, fp))) {
		c = skip_spaces(c);
		line_cnt++;
		L = 0;
		
		if (*c != ';' && *c != '\n') /* check if line is empty or comment */
		{
			label = read_until_token(c, SPACE_TOKEN); /* first suppose the first word of line is label */
			if (label == NULL) {
				fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
			}
			label_flag = is_label(label,line_cnt)&&c[strlen(label)]==':'; /* we have already checked for syntax errors in first pass */
			c+= strlen(label)+1;
			c=skip_spaces(c);
			if (label_flag) { /* if the first word was a label, then read the next word */
				word = read_until_token(c, SPACE_TOKEN);
				if (word == NULL) {
					fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
				}
                c+= strlen(word)+1;
				c = skip_spaces(c);
			} /* if it wasn't there is no need to read the next word */
			else word = label;
			if (!strcmp(word, ".data") || !strcmp(word, ".string") || !strcmp(word, ".extern")) 
			{
				continue; /* just a reminder that in the forum they wrote we can use break and continue */
			}
			else if (!strcmp(word, ".entry")) {

				word = read_until_token(c, SPACE_TOKEN);
				if (word == NULL) {
					fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
				}
				i = is_label_in_table(word, label_table_name, label_table_size);
				if(label_error(i,line_cnt)){
					error_flag=T;
					continue; /* check label was found in first pass */
				}
				label_table_type[i] = ENTRY;
							
			}
			else { /* in this case the line is an operation */
				
				opcode = op_pos(word);
				if (opcode == -1)
				{
					fprintf(stdout, "ERROR. Operation name does not exist in line %d.\n",line_cnt);
				}
				else
				{
					if (op_param_amount[opcode] == 1) /* in this case operation has only 1 operand/parameter */
					{
						param1 = read_until_token(c, LEFT_BRACKET_TOKEN); /* get parameter */
						if(param1==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						
						i = is_label_in_table(param1, label_table_name, label_table_size); /* check if label was defined */
						if(is_label(param1,line_cnt) && i!=-1 && label_table_type[i]==EXTERNAL) /* write extern to externs file */
						{
							fprintf(extern_fp,"%s	%d\n",label_table_name[i],ic+101);
						}
						if(!is_reg(param1)&&!is_number(param1,F))
						{
							if(label_error(i,line_cnt)){
								error_flag=T;
								continue; /* check label was found in first pass */
							}
						}
						word = read_until_token(c, ENTER_TOKEN);
						if(word==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}
						c+=strlen(param1)+1;
						c=skip_spaces(c);		
						if (!strcmp(word, param1)) /* normal 1 argument */
						{
							
							L=2;
							cells[ic+1]=encode_arg(cells[ic + 1], param1, 0, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
							if(!empty_line(c)){
								fprintf(stdout, "Error at line %d, cannot have commands or words after operation.\n",line_cnt);
								error_flag=T;
								continue;
							}
						}
						else /* 1 argument which is a label with 2 parameters */
						{
							label_param1 = read_until_token(c, COMMA_TOKEN);
							if(label_param1==NULL){
								fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
								return NULL;
							}	
							c += strlen(label_param1) + 1;
							c=skip_spaces(c);
							label_param2 = read_until_token(c, RIGHT_BRACKET_TOKEN);
							if(label_param2==NULL){
								fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
								return NULL;
							}	
							/* encode arguments and determine L */
							cells[ic+1]=encode_arg(cells[ic + 1], param1, 0, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
							if (is_reg(label_param1) && is_reg(label_param2))
							{
								L = 3;
								cells[ic+2]=encode_two_regs(cells[ic + 2], label_param1, label_param2);
							}
							else
							{
								L = 4;
								i = is_label_in_table(label_param1, label_table_name, label_table_size);
								if(!is_reg(label_param1)&&!is_number(label_param1,F))
								{
									if(label_error(i,line_cnt)){
										error_flag=T;
										continue; /* check label was found in first pass */
									}
								}
								if(is_label(label_param1,line_cnt) && i!=-1 && label_table_type[i]==EXTERNAL)
								{
									fprintf(extern_fp,"%s	%d\n",label_table_name[i],ic+102);

								}
								i = is_label_in_table(label_param2, label_table_name, label_table_size);
								if(!is_reg(label_param2)&&!is_number(label_param2,F))
								{
									if(label_error(i,line_cnt)){
										error_flag=T;
										continue; /* check label was found in first pass */
									}
								}
								if(is_label(label_param2,line_cnt) &&i!=-1 && label_table_type[i]==EXTERNAL)
								{
									fprintf(extern_fp,"%s	%d\n",label_table_name[i],ic+103);

								}
								cells[ic+2]=encode_arg(cells[ic + 2], label_param1, 0, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
								cells[ic+3]=encode_arg(cells[ic + 3], label_param2, 1, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
								
							}
						}
					}
					else if (op_param_amount[opcode] == 2) { /* In this case operation with 2 parameters */
						param1 = read_until_token(c, COMMA_TOKEN);
						if(param1==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}	
						c += strlen(param1) + 1;
						param2 = read_until_token(c, SPACE_TOKEN);
						if(param2==NULL){
							fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
							return NULL;
						}	
						if (is_reg(param2) && is_reg(param1))
						{
							L = 2;
							cells[ic+1]=encode_two_regs(cells[ic + 1], param1, param2);
						}
						else
						{
							L = 3;
							i = is_label_in_table(param1, label_table_name, label_table_size);
							if(!is_reg(param1)&&!is_number(param1,F))
							{
								if(label_error(i,line_cnt)){
									error_flag=T;
									continue; /* check label was found in first pass */
								}
							}
							if(is_label(param1,line_cnt)&&i!=-1 && label_table_type[i]==EXTERNAL)
							{
								fprintf(extern_fp,"%s	%d\n",label_table_name[i],ic+101);
							}
							i = is_label_in_table(param2, label_table_name, label_table_size);
							if(!is_reg(param2)&&!is_number(param2,F))
							{
								if(label_error(i,line_cnt)){
									error_flag=T;
									continue; /* check label was found in first pass */
								}
							}
							if(is_label(param2,line_cnt)&&i!=-1 && label_table_type[i]==EXTERNAL)
							{
								fprintf(extern_fp,"%s	%d\n",label_table_name[i],ic+102);
							}
							cells[ic+1]=encode_arg(cells[ic + 1], param1, 0, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
							cells[ic+2]=encode_arg(cells[ic + 2], param2, 1, label_table_name, label_table_num, label_table_type, label_table_size,line_cnt);
						}
					}
					else /* no parameters for operation in this case */
					{
						L = 1; /* we already checked there is no extra text in the first pass */
					}
					ic += L;
				}
			}
		}
	}
	if(error_flag)return NULL;
	result->label_table_name = label_table_name;
	result->label_table_type = label_table_type;
	result->label_table_num = label_table_num;
	result->label_table_size = label_table_size;
	result->cells = cells;
	result->cells_size=cells_size;
	result->data_array=data_array;
	result->data_size=data_size;
	fclose(fp);
	fclose(extern_fp);
	/* free memory */
	if(label!=NULL)free(label);
	if(param1!=NULL)free(param1);
	if(param2!=NULL)free(param2);
	if(label_param1!=NULL)free(label_param1);
	if(label_param1!=NULL)free(label_param2);
	return result;
}
