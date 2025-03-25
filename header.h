#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* Constants */
#define LINE_MAX_SIZE 82 /* 2 spaces for \n and \0 that fgets adds */
#define T 1
#define F 0
#define MACRO_NAME_NOT_FOUND -1
#define MAX_LABEL_LEN 30
#define DATA_RELOCATABLE 2
#define NOT_DATA_RELOCATABLE 3
#define EXTERNAL 4
#define ENTRY 5
#define MAX_NUM 16384 /* 2^14*/
#define OP_TABLE_LEN 16
#define COMMA_TOKEN 6
#define SPACE_TOKEN 7
#define LEFT_BRACKET_TOKEN 8
#define RIGHT_BRACKET_TOKEN 9
#define ENTER_TOKEN 10
#define QUOTATION_TOKEN 11
#define MAX_CELLS 256

/* Data structures */
typedef struct {
    unsigned int bits : 14;
} CELL;

typedef struct {
    char** label_table_name;
    int* label_table_num;
    int* label_table_type;
    int label_table_size;
    CELL* data_array;
    CELL* cells;
    int data_size;
    int cells_size;
} PassResult;

/* Memory management */
int check_null(void* c, int line_cnt);
void free_string_array(char** str_arr, int arr_len);
void* safe_malloc(size_t size, int line_cnt);
void* safe_calloc(size_t nmemb, size_t size, int line_cnt);
void* safe_realloc(void* ptr, size_t size, int line_cnt);

/* String operations */
char* skip_spaces(char* c);
char* read_until_token(char* c, int read_while);
int is_token(char c, int token);
int empty_line(char* c);

/* Assembly operations */
FILE* pre_assembler(FILE* fp, char* file_name);
PassResult* first_pass(FILE* fp);
PassResult* second_pass(FILE* fp, char* file_name, PassResult* result);
int assembler(char* file_name);
void assemble_files(int file_count, char* file_names[]);

/* Syntax checking */
int is_label(char* word, int line_cnt);
int is_reg(char* c);
int is_number(char* c, int is_data);
int arg_syntax(int opcode, int is_jump, int x, int dest, int line_cnt);
int label_error(int i, int line_cnt);

/* Label handling */
int is_macro_name_in_line(char** macro_names, int macro_name_count, char line[]);
int is_label_in_table(char* label_name, char** label_table_name, int label_table_size);
int insert_label_to_table(int* label_table_num, int num, int* label_table_type, int type, 
                         char** label_table_name, char* name, int label_table_size);
int op_pos(char* c);

/* Binary encoding */
CELL update_bit(CELL c, int value, int pos);
CELL num_to_cell(CELL c, int x);
CELL encode_two_regs(CELL c, char* r1, char* r2);
CELL encode_arg(CELL c, char* s, int dest, char** label_table_name, int* label_table_num, 
                int* label_table_type, int label_table_size, int line_cnt);
void cell_to_file(FILE* fp, CELL c);
