#include "header.h"

/*The function searches for macros, removes their definition from the file, while in each place there is a call for a macro, the program will put there the macro's content instead.*/
FILE* pre_assembler(FILE* fp,char*file_name) {
    FILE* new_fp;
    char** macro_names;
    char** macro_contents;
	char**tmp1;
	char**tmp2;
    char* c;
    char* macro_name;
    char line[LINE_MAX_SIZE+1];
    int run, size, total_data_len, i,macro_size;
    size = 1;
    macro_names = (char**)calloc(size, sizeof(char*));
    macro_contents = (char**)calloc(size, sizeof(char*));
    if (macro_names == NULL || macro_contents == NULL) {
        fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
        return NULL;
    }
    while (fgets(line, LINE_MAX_SIZE, fp)) {
        if ((c = strstr(line, "mcr")) != NULL) {/*if we find in the line the word mcr*/
            c += 4; /* Skip word mcr */
            macro_name = read_until_token(c, SPACE_TOKEN);/*finds the next word after mcr, so it holds the name of the macro*/
            if (macro_name == NULL) {
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
				if (macro_names != NULL) free_string_array(macro_names,size);
        		if (macro_contents != NULL) free_string_array(macro_contents,size);
                return NULL;
            }
			if((is_reg(macro_name))||(op_pos(macro_name)!=-1)){
				fprintf(stderr, "%s\n", "ERROR. Macro name cannot be a name of register or operation.");
				return NULL;
			}
            macro_names[size-1] = (char*)calloc((strlen(macro_name)+1), sizeof(char));
            if (macro_names[size-1] == NULL) {
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
                if(macro_name!=NULL)free(macro_name);
                if(macro_contents!=NULL)free_string_array(macro_contents,size);
				if(macro_names!=NULL)free_string_array(macro_names,size);
                return NULL;
            }
			strcpy(macro_names[size-1], macro_name);
            run = T;
            total_data_len = 0;
			macro_size = 1;
            while ((run == T) && (fgets(line, LINE_MAX_SIZE, fp)))/*in here we add to macro contents the lines until we reach endmcr*/
            {  
                if ((c = strstr(line, "endmcr")) == NULL){
					total_data_len+=strlen(line);
					if(macro_size == 1){
						macro_contents[size-1] = (char*)calloc((total_data_len), sizeof(char));
					}
					else{
						macro_contents[size-1] = (char*)realloc(macro_contents[size-1], ((total_data_len) * sizeof(char)));
					}
					if (macro_contents[size-1] == NULL) { 
				        fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
				    	if(macro_contents!=NULL)free_string_array(macro_contents,size);
						if(macro_names!=NULL)free_string_array(macro_names,size);
				        return NULL;
		           	}
					strcat(macro_contents[size-1], line);
				}
                else
                {
					run = F;
					macro_contents[size-1] = (char*)realloc(macro_contents[size-1], (++total_data_len) * sizeof(char));
					if (macro_contents[size-1] == NULL) {
			        	fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
			    		if(macro_contents!=NULL)free_string_array(macro_contents,size);
						if(macro_names!=NULL)free_string_array(macro_names,size);
			        	return NULL;
	           		}
					strncat(macro_contents[size-1], "\0",1);
                }
				macro_size++;
            }
            tmp1 = (char**)realloc(macro_contents, (size+1) * sizeof(char*));
            tmp2 = (char**)realloc(macro_names, (size+1) * sizeof(char*));
			/* In case memory allocation won't be available and null will be returned, we need temp pointers, in order to be able to free the memory correctly */
            if (tmp1 == NULL || tmp2 == NULL) {
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
                if(macro_contents!=NULL)free_string_array(macro_contents,size-1);
    			if(macro_names!=NULL)free_string_array(macro_names,size-1);
                return NULL;
            }
            else{
				macro_contents = tmp1;
				macro_names = tmp2;
				size++;
			}
			if(macro_name!=NULL)free(macro_name);
        }
    }
 	
    fseek(fp, 0, SEEK_SET);/*start of file*/
	
    new_fp = fopen(strcat(file_name,".am"), "w");/*this is the spanned file*/
	if(new_fp==NULL){
		fprintf(stderr, "%s\n", "ERROR. The input file cannot be opened.");
		return NULL;
	}
	size--;
    while (fgets(line, LINE_MAX_SIZE, fp))/*building the new file*/
	{
        if ((i = is_macro_name_in_line(macro_names, size, line)) != MACRO_NAME_NOT_FOUND) {
            if (strstr(line, "mcr"))
                while (fgets(line, LINE_MAX_SIZE, fp) && !strstr(line, "endmcr"));/*skip those lines because we are inside macro definition*/
            else
                fputs(macro_contents[i], new_fp);
        }
        else
            fputs(line, new_fp);
	}
	
	if(macro_contents!=NULL)free_string_array(macro_contents,size);
	if(macro_names!=NULL)free_string_array(macro_names,size);
	
    fclose(fp);
	fclose(new_fp);
	
    return new_fp;
}
