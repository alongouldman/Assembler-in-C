#include "assembler.h"

/* list of the register names, and token type enum */
struct Register {
	char *name;
	int num;
}register_table[] = {
	{ "r0",R0 },
{ "r1",R1 },
{ "r2",R2 },
{ "r3",R3 },
{ "r4",R4 },
{ "r5",R5 },
{ "r6",R6 },
{ "r7",R7 },
{ NULL }
};

/* types of instructions */
struct instruction {
	char * instruction;
	int type;
}instructions_table[] = {
	{ ".data", DATA },
{ ".string" , STRING },
{ ".extern", EXTERN },
{ ".entry" ,ENTRY },
{ NULL }
};

/* returns the number of register (if found), otherwise returns -1 */
int get_register(char *word)
{
	int i;
	for (i = 0; register_table[i].name; i++)
	{
		if (strcmp(word, register_table[i].name) == 0) /* instruction found */
		{
			return register_table[i].num;
		}
	}
	return -1; /* its not a command */
}

/* returns the type of the instuction (if found ). otherwise returns -1 */
int get_instruction(char *word)
{
	int i;
	for (i = 0; instructions_table[i].instruction; i++)
	{
		if (strcmp(word, instructions_table[i].instruction) == 0) /* instruction found */
		{
			return instructions_table[i].type;
		}
	}
	return -1; /* no istruction found */
}

/* this function reads a token from line. the curr_token will be updated with the proper type,
and pointer to the rest of the line will be returned */
char *get_token(char *line, Token *curr_token)
{
	char tok[MAX_LABLE];
	int tok_index = 0;

	{
		while (IS_WHITE(*line)) /* skip white spaces, (not '\n') */
		line++;
	}
	if (isdigit(*line) || ((*line) == '-')) /* a number */
	{
		do { /* collect all the digits */
			tok[tok_index++] = *line;
			line++;
		} while (isdigit(*line));
		tok[tok_index] = '\0';
		curr_token->type = number_tok;
		curr_token->data.number = atoi(tok);
	}
	else if (*line == '.') /* instruction */
	{
		do { /* collects all the instruction name */
			tok[tok_index++] = *line;
			line++;
		} while (!isspace(*line));
		tok[tok_index++] = '\0';
		if ((curr_token->data.instruction = get_instruction(tok)) >= 0) /* instruction found */
		{
			curr_token->type = instruction_tok;
		}
		else /* starts with a '.' but its not any instruction */
		{
			curr_token->type = error_tok;
		}
	}
	else if (*line == '\n')
	{
		curr_token->type = new_line_tok;
		line++;
	}
	else if (isalnum(*line)) /* lable, command or register */
	{
		do {
			tok[tok_index++] = *line;
			line++;
		} while (isalnum(*line));
		tok[tok_index] = '\0';
		if ((curr_token->data.command = get_command(tok)) >= 0) /* command found */
		{
			curr_token->type = command_tok;
			if(!isspace(*line)) /* space must follow command name */
			{
				curr_token->type = error_tok;
			}
		}
		else if ((curr_token->data.reg = get_register(tok)) >= 0) /* register found */
		{
			curr_token->type = register_tok;
		}
		else if (is_valid_lable(tok)) /* if its not a command and not a register - its a lable */
		{
			/* add lable to the token array */
			curr_token->type = lable_tok;
			strcpy(curr_token->data.lable_name, tok);
		}
		else /* error */
		{
			curr_token->type = error_tok;
		}
	}
	else /* its a spaciel character */
	{
		curr_token->type = *line; /* the type will be the ASCII value of the character itself */
		line++;
	}
	return line;
}

/* this function returns a string token. it will collect from line all the charachter between the quotes.
(ignoring spaces, special characters etc).
in case of an error, the first character of the string will be '\0' */
char * get_string_token(char *line, Token *curr_token)
{
	char tok[MAX_LINE];
	int tok_index = 0;

	curr_token->type = string_tok;
	while (IS_WHITE(*line)) /* skip white spaces */
	{
		line++;
	}
	if (*line == '\"') /* start of the string's content */
	{
		line++;
		if (*line == '\"') /* empty string */
		{
			curr_token->data.string[0] = '\0'; /* marks an error */
		}
		else /* not an empty string */
		{
			do {
				tok[tok_index++] = *line;
				line++;
			} while ((*line != '\"') && (*line != '\n'));
			tok[tok_index] = '\0';
			if (*line == '\n') /* missing quote (") at the end of the string */
			{
				curr_token->data.string[0] = '\0'; /* marks an error */
			}
			else
			{
				strcpy(curr_token->data.string, tok);
			}
		}
	}
	else
	{
		curr_token->data.string[0] = '\0'; /* marks an error */
	}
	line++;
	return line;
}
