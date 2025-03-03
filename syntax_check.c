#include "header.h"

/*The function checks if the string is a register*/
int is_reg(char* c)
{
	if (strlen(c) < 2)return 0;
	if (!(c[0] == 'r' && '0' <= c[1] && '7' >= c[1]))return 0;
	c += 2;
	c = skip_spaces(c);
	if (*c=='\0')return 1;
	return 0;
}

/*The function checks if the string is a number
int is_data tells if we need to check for a number with or without #*/
int is_number(char* c,int is_data) 
{
	int n;
	n = strlen(c);
	if (is_data ? n<1 : n < 2)return 0;
	if (!is_data  && *c != '#')return 0;
	if(!is_data)c++;
	if (*c == '-'||*c=='+')
	{
		c++;
		if (is_data ? n == 1:n == 2)return 0;
	}
	if (*c == '0' && strlen(c) != 1) {
		return 0;
	}
	while (isdigit(*(c)))c++;
	return empty_line(c);
}

/*checks if the string is in the form of a label*/
int is_label(char* word,int line_cnt)
{
	int len, i;
	i = 0;
	if(is_reg(word) || op_pos(word) != -1)return F;/*cant be a register or an operation*/
	len = strlen(word);
	if ((len>MAX_LABEL_LEN) || (!isalpha(word[0])))return F;/*name is to long/doesn't have correct syntax*/
	while (i < len){
		if(empty_line(word+i))return T;
		if (!isalpha(word[i]) && !isdigit(word[i])) return F;
		i++;
	}
	return T;
}
