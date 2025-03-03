#include "header.h"

/* The function will update bit number pos with value in cell c */
CELL update_bit(CELL c, int value, int pos) 
{
	if (value == 0)
		c.bits &= ~(1 << pos);
	else
		c.bits |= (1 << pos);
	return c;
}

/* The function turns a number to a cell (cell is a binary number with 14 bits) */
CELL num_to_cell(CELL c, int x)
{
	int i;
	for (i = 0; i < 14; i++)
	{
		c=update_bit(c, x % 2, i);
		x /= 2;
	}
	return c;
}

/*The function will turn a paramater (char *s) into his binary representation according to the project instructions */
CELL encode_arg(CELL c, char* s, int dest, char** label_table_name, int* label_table_num, int* label_table_type, int label_table_size,int line_cnt) {
	int n, i, flag = 0, tmp;
	c.bits = 0;
	if (is_reg(s))
	{
		n = (s[1] - '0');
		if (dest)
		{
			for (i = 2; i < 8; i++)/*if its operand is the destination we encode it to the 2-7 bits*/
			{
				c=update_bit(c, n % 2, i);
				n /= 2;
			}
		}
		else
		{
			for (i = 8; i < 14; i++)
			{
				c=update_bit(c, n % 2, i);
				n /= 2;
			}
		}
	}
	else if (is_number(s,F))
	{
		s++;
		if (s[0] == '-')
		{
			flag = 1;
			s++;
		}
		n = flag ? MAX_NUM/4 - atoi(s) : atoi(s);
		for (i = 2; i < 14; i++)
		{
			c=update_bit(c, n % 2, i);
			n /= 2;
		}
	}
	else if(is_label(s,line_cnt))
	{
		n = is_label_in_table(s, label_table_name, label_table_size); /* There's no need to check if n is -1 because we've already checked the label is in the table */
		if (label_table_type[n] == EXTERNAL)
		{
			c=update_bit(c, 1, 0);
		}
		else
		{
			c=update_bit(c, 1, 1);
			tmp = label_table_num[n]+100;
			for (i = 2; i < 14; i++)
			{
				c=update_bit(c, tmp%2, i);
				tmp /= 2;
			}
		}
	}
	return c;
}

/* The function will encode 2 registers into their binary representation according to the project instructions */
CELL encode_two_regs(CELL c, char* r1, char* r2) { /* suppose r1 is source register and r2 is dest register */
	int i, num1, num2;
	c.bits = 0;
	num1 = *(r1 + 1) - '0';
	num2 = *(r2 + 1) - '0';
	for (i = 2; i < 8; i++) {/*2-7 bit*/
		c=update_bit(c, num2 % 2, i);
		num2 /= 2;
	}
	for (i=8; i < 14; i++) {/*8-13 bit*/
		c=update_bit(c, num1 % 2, i);
		num1 /= 2;
	}
	return c;
}

/*The function writes the cell into the file*/
void cell_to_file(FILE*fp, CELL c){
	int i;
	for(i=13;i>=0;i--)
		fputc((c.bits&(1<<i))?'/':'.',fp);
	fputc('\n',fp);
}


