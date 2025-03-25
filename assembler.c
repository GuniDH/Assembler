#include "header.h"

/* Process each output file for a given assembly input file */
static void clean_and_handle_output_files(char* file_name) {
    FILE* entry_fp, *ob_fp, *extern_fp;
    char* file_name_cpy;
    
    file_name_cpy = safe_malloc((strlen(file_name) + 4) * sizeof(char), 0);
    if (file_name_cpy == NULL) {
        return;
    }
    
    /* Handle entry file */
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".ent");
    entry_fp = fopen(file_name_cpy, "r");
    if (entry_fp != NULL) {
        if(fgetc(entry_fp) == EOF) { /* Check if entry file is empty, if yes remove it */
            fclose(entry_fp);
            remove(file_name_cpy);
        } else {
            fclose(entry_fp);
        }
    }
    
    /* Check object file exists */
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".ob");
    ob_fp = fopen(file_name_cpy, "r");
    if (ob_fp != NULL) {
        fclose(ob_fp);
    }
    
    /* Handle extern file */
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".ext");
    extern_fp = fopen(file_name_cpy, "r");
    if (extern_fp != NULL) {
        if(fgetc(extern_fp) == EOF) { /* Check if extern file is empty, if yes remove it */
            fclose(extern_fp);
            remove(file_name_cpy);
        } else {
            fclose(extern_fp);
        }
    }
    
    free(file_name_cpy);
}

/* Writes output files (entry, object) from assembled code */
static void write_output_files(char* file_name, PassResult* result) {
    FILE* entry_fp, *ob_fp;
    char* file_name_cpy;
    int i, address;
    
    file_name_cpy = safe_malloc((strlen(file_name) + 4) * sizeof(char), 0);
    if (file_name_cpy == NULL) {
        return;
    }
    
    /* Write entry file */
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".ent");
    entry_fp = fopen(file_name_cpy, "w");
    if (entry_fp == NULL) {
        fprintf(stdout, "%s\n", "ERROR. The entry file cannot be opened.");
        free(file_name_cpy);
        return;
    }
    
    /* Write object file */
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".ob");
    ob_fp = fopen(file_name_cpy, "w");
    if (ob_fp == NULL) {
        fprintf(stdout, "%s\n", "ERROR. The object file cannot be opened.");
        fclose(entry_fp);
        free(file_name_cpy);
        return;
    }
    
    /* Write header of object file */
    fprintf(ob_fp, "\t\t%d %d\n", result->cells_size, result->data_size);
    
    /* Write entry labels */
    for(i = 0; i < result->label_table_size; i++) {
        if(result->label_table_type[i] == ENTRY) {
            fprintf(entry_fp, "%s %d \n", result->label_table_name[i], result->label_table_num[i] + 100);
        }
    }
    
    /* Write code section to object file */
    address = 100;
    for(i = 0; i < result->cells_size; i++) {
        if(address < 1000) fputc('0', ob_fp);
        fprintf(ob_fp, "%d\t", address++);
        cell_to_file(ob_fp, result->cells[i]);
    }
    
    /* Write data section to object file */
    for(i = 0; i < result->data_size; i++) {
        if(address < 1000) fputc('0', ob_fp);
        fprintf(ob_fp, "%d\t", address++);
        cell_to_file(ob_fp, result->data_array[i]);
    }
    
    fclose(entry_fp);
    fclose(ob_fp);
    free(file_name_cpy);
}

/* Free resources used by the PassResult structure */
static void free_pass_result(PassResult* result) {
    if (result == NULL) {
        return;
    }
    
    if (result->data_array != NULL) {
        free(result->data_array);
    }
    
    if (result->cells != NULL) {
        free(result->cells);
    }
    
    if (result->label_table_num != NULL) {
        free(result->label_table_num);
    }
    
    if (result->label_table_type != NULL) {
        free(result->label_table_type);
    }
    
    if (result->label_table_name != NULL) {
        free_string_array(result->label_table_name, result->label_table_size);
    }
    
    free(result);
}

/* Calls the assembler function for each of the files on the argument line */
void assemble_files(int file_count, char* file_names[]) {
    int i;
    for(i = 1; i < file_count; i++) {
        printf("\nFile number %d named %s\n\n", i, file_names[i]);
        assembler(file_names[i]);
    }
}

/* Assembles a single file */
int assembler(char* file_name) {
    FILE* fp;
    PassResult* result;
    char* file_name_cpy;
    
    file_name_cpy = safe_malloc((strlen(file_name) + 4) * sizeof(char), 0);
    if (file_name_cpy == NULL) {
        return 0;
    }
    
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".as");  /* Add extension to file name */
    
    fp = fopen(file_name_cpy, "r"); /* Open source code file */
    if (fp == NULL) {
        fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
        free(file_name_cpy);
        return 0;
    }
    
    strcpy(file_name_cpy, file_name);
    if ((fp = pre_assembler(fp, file_name_cpy)) == NULL) {
        free(file_name_cpy);
        return 0;
    }
    
    strcpy(file_name_cpy, file_name);
    strcat(file_name_cpy, ".am");
    fp = fopen(file_name_cpy, "r"); /* Open the deployed file the pre assembler created */
    if (fp == NULL) {
        fprintf(stdout, "%s\n", "ERROR. The input file cannot be opened.");
        free(file_name_cpy);
        return 0;
    }
    
    if ((result = first_pass(fp)) == NULL) {
        free(file_name_cpy);
        return 0;
    }
    
    strcpy(file_name_cpy, file_name);
    if ((result = second_pass(fp, file_name_cpy, result)) == NULL) {
        free(file_name_cpy);
        return 0;
    }
    
    if ((result->cells_size + result->data_size) > (MAX_CELLS - 99)) {
        fprintf(stdout, "%s\n", "ERROR. The assembly program takes too much memory.");
        free_pass_result(result);
        free(file_name_cpy);
        return 0;
    }
    
    /* Write all output files */
    write_output_files(file_name, result);
    
    /* Clean up temporary files and handle empty output files */
    clean_and_handle_output_files(file_name);
    
    /* Free resources */
    free_pass_result(result);
    free(file_name_cpy);
    
    return 1;
}
