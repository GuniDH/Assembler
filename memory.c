#include "header.h"

int check_null(void* c,int line_cnt)
{
	if(c==NULL)
	{
		fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "No memory for allocation available.");
		return 1;
	}
	return 0;
}

/*The function frees the memory in str_arr*/
void free_string_array(char**str_arr, int arr_len) {
	int i;
	for (i = 0; i < arr_len; i++) {
        if(str_arr[i]!=NULL)free(str_arr[i]);  
    }
    if(str_arr!=NULL)free(str_arr); 
}

