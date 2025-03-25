#include "header.h"

/* Helper function prototypes - static so they're only visible in this file */
static int process_label(char* label, int line_cnt, char** label_table_name, int* label_table_num, 
                      int* label_table_type, int label_table_size, int address, int type);
static int handle_data_directive(char* c, int* dc, CELL** data_array, int line_cnt);
static int handle_string_directive(char* c, int* dc, CELL** data_array, int line_cnt);
static int process_operation(char* c, char* word, int opcode, CELL** cells, int ic, int line_cnt, 
                          char* label, char** label_table_name, int* label_table_num, 
                          int* label_table_type, int label_table_size, int* L);
static int process_single_operand(char* c, char* param1, CELL* cell, int opcode, int line_cnt, int* L);
static int process_two_operands(char* c, CELL* cell, int opcode, int line_cnt, int* L);
static void encode_opcode(CELL* cell, int opcode);

PassResult* first_pass(FILE* fp) {
	PassResult* result;
	int ic, dc, label_flag, label_table_size, error_flag, extern_entry_label_flag;
	int line_cnt;
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
	int L;
	
	error_flag = F;
	line_cnt = 0;
	ic = 0;
	dc = 0;
	
	/* Memory allocations */
	result = (PassResult*)calloc(1, sizeof(PassResult));
	cells = (CELL*)calloc((ic + 1), sizeof(CELL));
	label_table_size = 1;
	data_array = (CELL*)calloc((dc + 1), sizeof(CELL));
	label_table_name = (char**)calloc(label_table_size, sizeof(char*));
	label_table_num = (int*)calloc(label_table_size, sizeof(int));
	label_table_type = (int*)calloc(label_table_size, sizeof(int));
	
	if (result == NULL || cells == NULL || label_table_name == NULL || 
	    label_table_num == NULL || label_table_type == NULL || data_array == NULL) {
		fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
		return NULL;
	}
	
	/* Process each line of input file */
	while ((c = fgets(line, LINE_MAX_SIZE, fp))) {
		label_flag = F;
		c = skip_spaces(c);
		line_cnt++;
		L = 0;
		extern_entry_label_flag = F;
		
		if (*c != ';' && *c != '\n') { /* Skip comments and empty lines */
			label = read_until_token(c, SPACE_TOKEN);
			if(check_null(label, line_cnt)) return NULL;
			
			if(strlen(label) == 0) {
				fprintf(stdout, "ERROR. Label name in line %d was not defined (only :).\n", line_cnt);
				error_flag = T;
				continue;
			}
			
			/* Check if line starts with a label */
			if(c[strlen(label)] == ':') {
				if(!is_label(label, line_cnt)) {
					fprintf(stdout, "ERROR. Label name in line %d doesn't match correct syntax.\n", line_cnt);
					error_flag = T;
					continue;
				}
				label_flag = T;
			}
			
			if(is_label(label, line_cnt) && (strlen(label) + 1) <= strlen(c) && 
               c[strlen(label)] == ' ' && c[strlen(label) + 1] == ':') {
				fprintf(stdout, "%s\n", "ERROR. Label name cannot have space between its name and ':' character.");
				error_flag = T;
				continue;
			}
			
			c += strlen(label) + 1;
			c = skip_spaces(c);
			
			if (label_flag) {
				word = read_until_token(c, SPACE_TOKEN);
				if(check_null(word, line_cnt)) return NULL;
                c += strlen(word) + 1;
				c = skip_spaces(c);
			} else {
				word = label;
			}
			
			/* Handle directives and operations */
			if (!strcmp(word, ".data") || !strcmp(word, ".string")) {
				if (label_flag) {
					if(process_label(label, line_cnt, label_table_name, label_table_num, 
                                 label_table_type, label_table_size, dc, DATA_RELOCATABLE)) {
						return NULL;
					}
				}
				
				if (!strcmp(word, ".data")) {
					if(handle_data_directive(c, &dc, &data_array, line_cnt)) {
						error_flag = T;
						continue;
					}
				} else { /* .string */
					if(handle_string_directive(c, &dc, &data_array, line_cnt)) {
						error_flag = T;
						continue;
					}
				}
			} else if (!strcmp(word, ".entry") || !strcmp(word, ".extern")) {
				if (!strcmp(word, ".extern")) {
					word = read_until_token(c, SPACE_TOKEN);
					if(check_null(word, line_cnt)) return NULL;
					c += strlen(word) + 1;
					
					if(!empty_line(c)) {
						fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
                               "after .extern or .entry there can only be 1 label.");
						error_flag = T;
					 	continue;
					}
					
					if(is_label_in_table(word, label_table_name, label_table_size - 1) != -1) {
						fprintf(stdout, "ERROR. Label in line %d has already been defined.\n", line_cnt);
						return NULL;
					}
					
					if(!is_label(word, line_cnt)) {
						fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
						error_flag = T;
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
				
				if(label_flag) {
					fprintf(stdout, "WARNING. Label in line %d before entry or extern.\n", line_cnt);
					extern_entry_label_flag = T;
				}
			} else { /* Operation lines */
				if (label_flag) {
					if(process_label(label, line_cnt, label_table_name, label_table_num, 
                                 label_table_type, label_table_size, ic, NOT_DATA_RELOCATABLE)) {
						return NULL;
					}
				}
				
				int opcode = op_pos(word);
				if (opcode == -1) {
					fprintf(stdout, "ERROR. Operation name does not exist in line %d.\n", line_cnt);
					error_flag = T;
					continue;
				} else {
					cells[ic].bits = 0; /* Initialize */
					
					if (process_operation(c, word, opcode, &cells, ic, line_cnt, 
                                      label, label_table_name, label_table_num, 
                                      label_table_type, label_table_size, &L)) {
						error_flag = T;
						continue;
					}
					
					encode_opcode(&cells[ic], opcode);
					ic += L;
				}
			}
			
			/* Add label to table if needed */
			if(label_flag && !extern_entry_label_flag) {
				label_table_size++;
				label_table_name = (char**)realloc(label_table_name, label_table_size * sizeof(char*));
				label_table_num = (int*)realloc(label_table_num, label_table_size * sizeof(int));
				label_table_type = (int*)realloc(label_table_type, label_table_size * sizeof(int));
				
				if (label_table_name == NULL || label_table_num == NULL || label_table_type == NULL) {
					fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
					error_flag = T;
				}
			}
			
			cells = (CELL*)realloc(cells, (ic + 1) * sizeof(CELL));
			if (cells == NULL) {
				fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
				error_flag = T;
			}
		}
	}
	
	if(error_flag) return NULL; /* Don't continue to second pass if errors occurred in first */
	
	label_table_size--;
	/* Add ic to every label which is data type */
	for (int i = 0; i < label_table_size; i++) {
		if (label_table_type[i] == DATA_RELOCATABLE) {
			label_table_num[i] += ic;
		}
	}
	
	/* Prepare the result */
	result->label_table_name = label_table_name;
	result->label_table_type = label_table_type;
	result->label_table_num = label_table_num;
	result->label_table_size = label_table_size;
	result->cells = cells;
	result->cells_size = ic;
	result->data_array = data_array;
	result->data_size = dc;
	
	/* Free memory that's no longer needed */
	if(label != NULL) free(label);
	
	return result;
}

/* Process a label definition and add it to the table */
static int process_label(char* label, int line_cnt, char** label_table_name, int* label_table_num, 
                      int* label_table_type, int label_table_size, int address, int type) {
    
    if(is_label_in_table(label, label_table_name, label_table_size - 1) != -1) {
        fprintf(stdout, "ERROR. Label in line %d has already been defined.\n", line_cnt);
        return 1;
    }
    
    return insert_label_to_table(label_table_num, address, label_table_type, type, 
                                label_table_name, label, label_table_size);
}

/* Process .data directive */
static int handle_data_directive(char* c, int* dc, CELL** data_array, int line_cnt) {
    char* word;
    int minus, curnum, i = 0;
    
    while (*c != '\0') {
        minus = F;
        word = read_until_token(c, COMMA_TOKEN);
        if(word == NULL) {
            fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
            return 1;
        }
        
        if(*word == '\0') {
            if(i == 0) {
                fprintf(stdout, "ERROR. .data in line %d has not received arguments.\n", line_cnt);
                free(word);
                return 1;
            } else {
                free(word);
                break;
            }
        }
        
        c += strlen(word) + 1;
        c = skip_spaces(c);
        
        if(!is_number(word, T)) {
            fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
                   "Data has received an argument which isn't a number.");
            free(word);
            return 1;
        }
        
        if(*word == '-' || *word == '+') {
            if(*word == '-') minus = T;
            word++;
        }
        
        curnum = atoi(word);
        (*data_array)[*dc] = minus ? num_to_cell((*data_array)[*dc], MAX_NUM - curnum) 
                                  : num_to_cell((*data_array)[*dc], curnum);
        (*dc)++;
        
        *data_array = (CELL*)realloc(*data_array, ((*dc) + 1) * sizeof(CELL));
        if (*data_array == NULL) {
            fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
            free(word);
            return 1;
        }
        
        free(word);
        i++;
    }
    
    if(!empty_line(c)) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
               "Commands aren't allowed after .data, and double-commas are forbidden.");
        return 1;
    }
    
    return 0;
}

/* Process .string directive */
static int handle_string_directive(char* c, int* dc, CELL** data_array, int line_cnt) {
    char* word;
    
    if(empty_line(c)) {
        fprintf(stdout, "ERROR. .string in line %d has not received arguments.\n", line_cnt);
        return 1;
    }
    
    if(*c == '\"') c++;
    else {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
               "string should be defined with the \" character at the start of its definition.");
        return 1;
    }
    
    word = read_until_token(c, QUOTATION_TOKEN);
    if(word == NULL) {
        fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
        return 1;
    }
    
    c += strlen(word);
    
    if(*c == '\"') c++;
    else {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
               "string should be defined with the \" character at the end of its definition.");
        free(word);
        return 1;
    }
    
    if(!empty_line(c)) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, 
               "after .string there can only be 1 word.");
        free(word);
        return 1;
    }
    
    char* str_ptr = word;
    while (*str_ptr != '\"' && *str_ptr != '\0') {
        if((*str_ptr < ' ' || *str_ptr > '~') && (!isspace(*str_ptr))) {
            fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "This isn't a legal word");
            free(word);
            return 1;
        }
        
        (*data_array)[*dc] = num_to_cell((*data_array)[*dc], (int)(*str_ptr));
        (*dc)++;
        str_ptr++;
        
        *data_array = (CELL*)realloc(*data_array, ((*dc) + 1) * sizeof(CELL));
        if(check_null(*data_array, line_cnt)) {
            free(word);
            return 1;
        }
    }
    
    /* Add null terminator */
    (*data_array)[*dc] = num_to_cell((*data_array)[*dc], 0);
    (*dc)++;
    *data_array = (CELL*)realloc(*data_array, ((*dc) + 1) * sizeof(CELL));
    if(check_null(*data_array, line_cnt)) {
        free(word);
        return 1;
    }
    
    free(word);
    return 0;
}

/* Process an operation line */
static int process_operation(char* c, char* word, int opcode, CELL** cells, int ic, int line_cnt, 
                          char* label, char** label_table_name, int* label_table_num, 
                          int* label_table_type, int label_table_size, int* L) {
    const int op_param_amount[] = { 2,2,2,2,1,1,2,1,1,1,1,1,1,1,0,0 };
    char* param1;
    
    if (op_param_amount[opcode] == 1) {
        param1 = read_until_token(c, LEFT_BRACKET_TOKEN);
        if(param1 == NULL) {
            fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
            return 1;
        }
        
        if (process_single_operand(c, param1, &(*cells)[ic], opcode, line_cnt, L)) {
            free(param1);
            return 1;
        }
        
        free(param1);
    } else if (op_param_amount[opcode] == 2) {
        if (process_two_operands(c, &(*cells)[ic], opcode, line_cnt, L)) {
            return 1;
        }
    } else { /* No parameters */
        if(!empty_line(c)) {
            fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "it isn't an argument");
            return 1;
        }
        *L = 1;
    }
    
    return 0;
}

/* Process an operation with a single operand */
static int process_single_operand(char* c, char* param1, CELL* cell, int opcode, int line_cnt, int* L) {
    char* word = read_until_token(c, ENTER_TOKEN);
    if(word == NULL) {
        fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
        return 1;
    }
    
    c += strlen(param1) + 1;
    c = skip_spaces(c);
    
    if (!strcmp(word, param1)) { /* Normal 1 argument */
        *L = 2;
        
        if(!empty_line(c)) {
            fprintf(stdout, "Error at line %d, cannot have commands or words after operation.\n", line_cnt);
            free(word);
            return 1;
        }
        
        if(is_number(param1, T)) {
            fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
            free(word);
            return 1;
        }
        
        if (is_number(param1, F)) {
            if(arg_syntax(opcode, 0, 0, 1, line_cnt)) {
                free(word);
                return 1;
            }
        } else if (is_reg(param1)) {
            if(arg_syntax(opcode, 0, 3, 1, line_cnt)) {
                free(word);
                return 1;
            }
            cell = update_bit(*cell, 1, 2);
            cell = update_bit(*cell, 1, 3);
        } else if(!is_label(param1, line_cnt)) {
            fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
            free(word);
            return 1;
        } else {
            if(arg_syntax(opcode, 0, 1, 1, line_cnt)) {
                free(word);
                return 1;
            }
            cell = update_bit(*cell, 1, 2);
        }
    } else { /* Argument with parameters (jump) */
        free(word);
        
        if(is_number(param1, T)) {
            fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
            return 1;
        }
        
        if (is_number(param1, F)) {
            if(arg_syntax(opcode, 1, 0, 1, line_cnt)) {
                return 1;
            }
        } else if (is_reg(param1)) {
            if(arg_syntax(opcode, 1, 3, 1, line_cnt)) {
                return 1;
            }
        } else if(!is_label(param1, line_cnt)) {
            fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
            return 1;
        } else {
            if(arg_syntax(opcode, 1, 1, 1, line_cnt)) {
                return 1;
            }
        }
        
        cell = update_bit(*cell, 1, 3);
        
        char* label_param1 = read_until_token(c, COMMA_TOKEN);
        if(label_param1 == NULL) {
            fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
            return 1;
        }
        
        if (is_reg(label_param1)) {
            cell = update_bit(*cell, 1, 12);
            cell = update_bit(*cell, 1, 13);
        } else if (is_number(label_param1, F) || is_number(label_param1, T)) {
            if(is_number(label_param1, T)) {
                fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
                free(label_param1);
                return 1;
            }
        } else if(!is_label(label_param1, line_cnt)) {
            fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
            free(label_param1);
            return 1;
        } else {
            cell = update_bit(*cell, 1, 12);
        }
        
        c += strlen(label_param1) + 1;
        if(isspace(*(c-2)) || isspace(*c) || *c == '\0') {
            fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "it isn't an argument");
            free(label_param1);
            return 1;
        }
        
        char* label_param2 = read_until_token(c, RIGHT_BRACKET_TOKEN);
        if(label_param2 == NULL) {
            fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
            free(label_param1);
            return 1;
        }
        
        if (is_reg(label_param2)) {
            cell = update_bit(*cell, 1, 10);
            cell = update_bit(*cell, 1, 11);
        } else if (is_number(label_param2, F) || is_number(label_param2, T)) {
            if(is_number(label_param2, T)) {
                fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
                free(label_param1);
                free(label_param2);
                return 1;
            }
        } else if(!is_label(label_param2, line_cnt)) {
            fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
            free(label_param1);
            free(label_param2);
            return 1;
        } else {
            cell = update_bit(*cell, 1, 10);
        }
        
        c += strlen(label_param2) + 1;
        if(*(c-1) != ')' && !empty_line(c)) {
            fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "it isn't an argument");
            free(label_param1);
            free(label_param2);
            return 1;
        }
        
        *L = (is_reg(label_param1) && is_reg(label_param2)) ? 3 : 4;
        
        if(!empty_line(c)) {
            fprintf(stdout, "Error at line %d, cannot have commands or words after operation.\n", line_cnt);
            free(label_param1);
            free(label_param2);
            return 1;
        }
        
        free(label_param1);
        free(label_param2);
    }
    
    return 0;
}

/* Process an operation with two operands */
static int process_two_operands(char* c, CELL* cell, int opcode, int line_cnt, int* L) {
    char* param1 = read_until_token(c, COMMA_TOKEN);
    if(param1 == NULL) {
        fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
        return 1;
    }
    
    if(is_number(param1, T)) {
        fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
        free(param1);
        return 1;
    }
    
    if (is_number(param1, F)) {
        if(arg_syntax(opcode, 0, 0, 0, line_cnt)) {
            free(param1);
            return 1;
        }
    } else if (is_reg(param1)) {
        if(arg_syntax(opcode, 0, 3, 0, line_cnt)) {
            free(param1);
            return 1;
        }
        cell = update_bit(*cell, 1, 4);
        cell = update_bit(*cell, 1, 5);
    } else if(!is_label(param1, line_cnt)) {
        fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
        free(param1);
        return 1;
    } else { 
        if(arg_syntax(opcode, 0, 1, 0, line_cnt)) {
            free(param1);
            return 1;
        }
        cell = update_bit(*cell, 1, 4);
    }
    
    c += strlen(param1) + 1;
    if(empty_line(c)) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "it isn't an argument");
        free(param1);
        return 1;
    }
    
    char* param2 = read_until_token(c, SPACE_TOKEN);
    if(param2 == NULL) {
        fprintf(stdout, "%s\n", "ERROR. No memory for allocation available.");
        free(param1);
        return 1;
    }
    
    c += strlen(param2) + 1;
    
    if(is_number(param2, T)) {
        fprintf(stdout, "ERROR. In line %d numbers as parameters of label should have # before them.\n", line_cnt);
        free(param1);
        free(param2);
        return 1;
    }
    
    if (is_number(param2, F)) {
        if(arg_syntax(opcode, 0, 0, 1, line_cnt)) {
            free(param1);
            free(param2);
            return 1;
        }
    } else if (is_reg(param2)) {
        if(arg_syntax(opcode, 0, 3, 1, line_cnt)) {
            free(param1);
            free(param2);
            return 1;
        }
        cell = update_bit(*cell, 1, 2);
        cell = update_bit(*cell, 1, 3);
    } else if(!is_label(param2, line_cnt)) {
        fprintf(stdout, "ERROR. In line %d the label name doesn't fit the correct syntax.\n", line_cnt);
        free(param1);
        free(param2);
        return 1;
    } else {
        if(arg_syntax(opcode, 0, 1, 1, line_cnt)) {
            free(param1);
            free(param2);
            return 1;
        }
        cell = update_bit(*cell, 1, 2);
    }
    
    if(!empty_line(c)) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "it isn't an argument");
        free(param1);
        free(param2);
        return 1;
    }
    
    *L = (is_reg(param2) && is_reg(param1)) ? 2 : 3;
    
    free(param1);
    free(param2);
    return 0;
}

/* Encode operation code to cell */
static void encode_opcode(CELL* cell, int opcode) {
    int i;
    unsigned int mask = 1;
    
    for (i = 6; i < 10; i++) {
        if (opcode & mask) {
            *cell = update_bit(*cell, 1, i);
        }
        mask *= 2;
    }
}
