#include "header.h"

/* Process a single macro definition and add it to the list */
static int process_macro_definition(FILE* fp, char* macro_name, char*** macro_names, 
                                 char*** macro_contents, int* size) {
    char line[LINE_MAX_SIZE];
    char* c;
    int run, total_data_len, macro_size;
    
    /* Validate macro name */
    if ((is_reg(macro_name)) || (op_pos(macro_name) != -1)) {
        fprintf(stderr, "%s\n", "ERROR. Macro name cannot be a name of register or operation.");
        return 0;
    }
    
    /* Allocate space for the macro name */
    (*macro_names)[*size - 1] = (char*)calloc((strlen(macro_name) + 1), sizeof(char));
    if ((*macro_names)[*size - 1] == NULL) {
        fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
        free(macro_name);
        return 0;
    }
    
    strcpy((*macro_names)[*size - 1], macro_name);
    run = T;
    total_data_len = 0;
    macro_size = 1;
    
    /* Read macro content until endmcr */
    while ((run == T) && (fgets(line, LINE_MAX_SIZE, fp))) {
        if ((c = strstr(line, "endmcr")) == NULL) {
            total_data_len += strlen(line);
            
            /* Allocate or reallocate memory for macro content */
            if (macro_size == 1) {
                (*macro_contents)[*size - 1] = (char*)calloc((total_data_len), sizeof(char));
            } else {
                (*macro_contents)[*size - 1] = (char*)realloc((*macro_contents)[*size - 1], 
                                                             ((total_data_len) * sizeof(char)));
            }
            
            if ((*macro_contents)[*size - 1] == NULL) { 
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
                return 0;
            }
            
            strcat((*macro_contents)[*size - 1], line);
        } else {
            run = F;
            (*macro_contents)[*size - 1] = (char*)realloc((*macro_contents)[*size - 1], 
                                                         (++total_data_len) * sizeof(char));
            if ((*macro_contents)[*size - 1] == NULL) {
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
                return 0;
            }
            
            strncat((*macro_contents)[*size - 1], "\0", 1);
        }
        
        macro_size++;
    }
    
    /* Expand macro tables */
    char** tmp1 = (char**)realloc(*macro_contents, (*size + 1) * sizeof(char*));
    char** tmp2 = (char**)realloc(*macro_names, (*size + 1) * sizeof(char*));
    
    if (tmp1 == NULL || tmp2 == NULL) {
        fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
        return 0;
    }
    
    *macro_contents = tmp1;
    *macro_names = tmp2;
    (*size)++;
    
    return 1;
}

/* The function searches for macros, removes their definition from the file,
   and expands macro calls */
FILE* pre_assembler(FILE* fp, char* file_name) {
    FILE* new_fp;
    char** macro_names;
    char** macro_contents;
    char* c;
    char* macro_name;
    char line[LINE_MAX_SIZE + 1];
    int size, i;
    
    /* Initialize macro tables */
    size = 1;
    macro_names = (char**)calloc(size, sizeof(char*));
    macro_contents = (char**)calloc(size, sizeof(char*));
    
    if (macro_names == NULL || macro_contents == NULL) {
        fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
        return NULL;
    }
    
    /* First pass: Find and store all macros */
    while (fgets(line, LINE_MAX_SIZE, fp)) {
        if ((c = strstr(line, "mcr")) != NULL) {
            c += 4; /* Skip word mcr */
            macro_name = read_until_token(c, SPACE_TOKEN);
            
            if (macro_name == NULL) {
                fprintf(stderr, "%s\n", "ERROR. No memory for allocation available.");
                free_string_array(macro_names, size);
                free_string_array(macro_contents, size);
                return NULL;
            }
            
            /* Process the macro definition */
            if (!process_macro_definition(fp, macro_name, &macro_names, &macro_contents, &size)) {
                free_string_array(macro_names, size);
                free_string_array(macro_contents, size);
                if (macro_name != NULL) free(macro_name);
                return NULL;
            }
            
            if (macro_name != NULL) free(macro_name);
        }
    }
    
    /* Rewind file for second pass */
    fseek(fp, 0, SEEK_SET);
    
    /* Open output file */
    char* am_filename = safe_malloc(strlen(file_name) + 4, 0);
    if (am_filename == NULL) {
        free_string_array(macro_names, size);
        free_string_array(macro_contents, size);
        return NULL;
    }
    
    strcpy(am_filename, file_name);
    strcat(am_filename, ".am");
    new_fp = fopen(am_filename, "w");
    free(am_filename);
    
    if (new_fp == NULL) {
        fprintf(stderr, "%s\n", "ERROR. The output file cannot be opened.");
        free_string_array(macro_names, size);
        free_string_array(macro_contents, size);
        return NULL;
    }
    
    /* Second pass: Expand macros in the code */
    size--;  /* Adjust size since we incremented it one extra time at the end */
    while (fgets(line, LINE_MAX_SIZE, fp)) {
        if ((i = is_macro_name_in_line(macro_names, size, line)) != MACRO_NAME_NOT_FOUND) {
            if (strstr(line, "mcr")) {
                /* Skip macro definition lines */
                while (fgets(line, LINE_MAX_SIZE, fp) && !strstr(line, "endmcr"));
            } else {
                /* Replace macro call with its contents */
                fputs(macro_contents[i], new_fp);
            }
        } else {
            /* Copy line as is */
            fputs(line, new_fp);
        }
    }
    
    /* Clean up */
    free_string_array(macro_contents, size);
    free_string_array(macro_names, size);
    fclose(fp);
    fclose(new_fp);
    
    /* Reopen the expanded file for reading */
    am_filename = safe_malloc(strlen(file_name) + 4, 0);
    if (am_filename == NULL) {
        return NULL;
    }
    
    strcpy(am_filename, file_name);
    strcat(am_filename, ".am");
    new_fp = fopen(am_filename, "r");
    free(am_filename);
    
    return new_fp;
}
