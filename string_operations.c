#include "header.h"

/*The function moves the char pointer c along as long as it is space or tab */
char* skip_spaces(char *c)
{
    while (*c == '\t' || *c == ' ') c++;
    return c;
}

/*The function checks,according to token, if the char is the token*/
int is_token(char c, int token) {
    if (token == SPACE_TOKEN) return isspace(c);
    else if (token == COMMA_TOKEN) return c == ',';
	else if (token == LEFT_BRACKET_TOKEN) return c == '(';
	else if (token == RIGHT_BRACKET_TOKEN) return c == ')';
	else if (token == ENTER_TOKEN) return c == '\n';
	else if(token == QUOTATION_TOKEN)return c=='\"';
    return 0;
}

/*The function gets a string c,skips its spaces,and than returns a pointer to a word consisting of all the characters of c until the specified token,or until enter,or until :*/
/*for example: if c = "   abdefk, " and the token is of the char , the function will return the string "abdefk"*/
char* read_until_token(char* c, int token) {
    char* word;
    int size;
    size = 1;
    word = (char*)calloc(size, sizeof(char));
    if (word == NULL)
        return NULL;
    c = skip_spaces(c);
    while (!(is_token(*c, token) || *c=='\n' || *c==':')) {
        word[size-1] = *(c++);
        word = (char*)realloc(word, (++size) * sizeof(char));
        if (word == NULL)
            return NULL;
    }
    word[size-1] = '\0';
    return word;
}

/*The function checks if the string c consists only of tabs and spaces*/
int empty_line(char* c)
{

    while(*c != '\0'){
        if(!isspace(*c))return 0;
        c++;
    }
    return 1;
}
