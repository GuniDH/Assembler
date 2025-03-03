#include "header.h"

/*The function will check if an operation can have these addressing methods (for example: mov cant have a number as its destination).
int x is the addressing method of the operand
return 1 if syntax problem and 0 if it is ok*/
int arg_syntax(int opcode,int is_jump,int x,int dest,int line_cnt)
{
	if(opcode>13){/*rts and stop cannot get any operands*/
		fprintf(stdout,"ERROR. In line %d operation does not support operands.\n", line_cnt);
		return 1;
	}
	else if(is_jump){/*jump with parameters*/
		if(opcode!=9&&opcode!=10&&opcode!=13)/*only jmp,bne and jsr can jump with parameters*/
		{
			fprintf(stdout,"ERROR. This kind of operation in line %d does not support the format of jumping with parameters.\n",line_cnt);
			return 1;
		}
		if(x!=1)/*when jumping with parameters the operand has to be a label*/
		{
			fprintf(stdout, "ERROR. Cannot have this kind of operand as source for this operation in line %d.\n",line_cnt);
			return 1;
		}
	}
	else{
		if(opcode==0||opcode==2||opcode==3){/*mov,add,sub*/
		
			if(x==0&&dest==1)
			{
				fprintf(stdout, "ERROR. Cannot have a number as destination operand for this operation in line %d.\n", line_cnt);
				return 1;
			}
		}
		else if(opcode==1){/*cmp*/
			return 0;
		}
		else if(opcode==6){/*lea*/
			if(x==0||(x==3&&dest==0))
			{
				fprintf(stdout,"ERROR. This kind of operation in line %d doesn't match with this kind of the arguments.\n",line_cnt);
				return 1;
			}
		}
		else if(dest==0)
		{
			printf("%d\n", opcode);
			fprintf(stdout, "ERROR. Cannot have this kind of operand in line %d as source for this operation.\n",line_cnt);
			return 1;
		}
		else if(opcode==12)return 0;/*prn*/
		else if(x==0)
		{
				fprintf(stdout, "ERROR. Cannot have a number as destination operand for this operation in line %d.\n",line_cnt);
				return 1;
		}
	}
	return 0;
}


int label_error(int i,int line_cnt)
{
	if(i!=-1)return 0;
	fprintf(stdout, "%s %d: %s\n", "Error at line",line_cnt, "the label isn't defined in the code.");
	return 1;
}
