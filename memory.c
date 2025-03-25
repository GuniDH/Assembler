#include "header.h"

int check_null(void* c, int line_cnt)
{
    if(c == NULL)
    {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "No memory for allocation available.");
        return 1;
    }
    return 0;
}

void free_string_array(char** str_arr, int arr_len) {
    int i;
    
    if(str_arr == NULL) {
        return;
    }
    
    for (i = 0; i < arr_len; i++) {
        if(str_arr[i] != NULL) {
            free(str_arr[i]);
        }
    }
    
    free(str_arr);
}

/* Safe malloc wrapper that handles error checking */
void* safe_malloc(size_t size, int line_cnt) {
    void* ptr = malloc(size);
    if(ptr == NULL) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "No memory for allocation available.");
    }
    return ptr;
}

/* Safe calloc wrapper that handles error checking */
void* safe_calloc(size_t nmemb, size_t size, int line_cnt) {
    void* ptr = calloc(nmemb, size);
    if(ptr == NULL) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "No memory for allocation available.");
    }
    return ptr;
}

/* Safe realloc wrapper that handles error checking */
void* safe_realloc(void* ptr, size_t size, int line_cnt) {
    void* new_ptr = realloc(ptr, size);
    if(new_ptr == NULL) {
        fprintf(stdout, "%s %d: %s\n", "Error at line", line_cnt, "No memory for allocation available.");
    }
    return new_ptr;
}
