#include "assembler.h"

/* here is all the data about the commands.
every command stores it's addressing modes, and how many parameters it recives */
struct Command {
	char *name;
	int num_of_operands;
	int source_addressing_modes[NUM_OF_ADDRESSING_MODES];
	int dest_addressing_modes[NUM_OF_ADDRESSING_MODES];
}commands_table[] = {
	/* addressing modes:
	0: instant_addressing
	1: direct_addressing
	2: jump_addressing
	3: register_addressing */
	{ "mov", 2  ,{ 1,1,0,1 },{ 0,1,0,1 } } ,
{ "cmp", 2  ,{ 1,1,0,1 },{ 1,1,0,1 } } ,
{ "add", 2  ,{ 1,1,0,1 },{ 0,1,0,1 } } ,
{ "sub", 2  ,{ 1,1,0,1 },{ 0,1,0,1 } } ,
{ "not", 1  ,{ 0,0,0,0 },{ 0,1,0,1 } } ,
{ "clr", 1  ,{ 0,0,0,0 },{ 0,1,0,1 } } ,
{ "lea", 2  ,{ 0,1,0,0 },{ 0,1,0,1 } } ,
{ "inc", 1  ,{ 0,0,0,0 },{ 0,1,0,1 } } ,
{ "dec", 1  ,{ 0,0,0,0 },{ 0,1,0,1 } } ,
{ "jmp", 1  ,{ 0,0,0,0 },{ 0,1,1,1 } } ,
{ "bne", 1  ,{ 0,0,0,0 },{ 0,1,1,1 } } ,
{ "red", 1  ,{ 0,0,0,0 },{ 0,1,0,1 } } ,
{ "prn", 1  ,{ 0,0,0,0 },{ 1,1,0,1 } } ,
{ "jsr", 1  ,{ 0,0,0,0 },{ 0,1,1,1 } } ,
{ "rts", 0  ,{ 0,0,0,0 },{ 0,0,0,0 } } ,
{ "stop", 0 ,{ 0,0,0,0 },{ 0,0,0,0 } },
{ NULL }
};

/* returns the type of the command */
int get_command(char *word)
{
	int i;
	if (word[0] != 'r') /* the beggining of the register names */
		for (i = 0; commands_table[i].name; i++)
		{
			if (strcmp(word, commands_table[i].name) == 0) /* instruction found */
			{
				return i; /* the index in the commands array */
			}
		}
	return -1; /* its not a command */
}

/* handeling commands. this is how the function works:
1. make a list of the operand (list of 3 tokens, or two or one )
2. make sure that the number of operands is valid to this command
3. make sure the addressing mode is valid to the command
4. write everything into the array
5. adds lables to the lable array
*/
char *parse_command(Token *command, char *line)
{
	int operands_cnt = 0; /* how many operands found in the line read */
	Token operands[MAX_OPERANDS + 1]; /* 1 for the new line */
	enum {
		NOT_JMP,
		YES_JMP
	}jump_addressing_flag = NOT_JMP;

	/* getting the operands into an array of operands */
	line = get_token(line, &(operands[operands_cnt]));

	if (operands[operands_cnt].type == number_tok) /* cant be a number without '#' */
	{
		print_error(PRINT_NUMBER,"number must begin with hash ('#') sign");
		return line;
	}
	if (operands[operands_cnt].type == '#') /* instant_addressing */
	{
		line = get_token(line, &(operands[operands_cnt])); /* get the number */
		if (operands[operands_cnt].type != number_tok) /* a # sign without a number after */
		{
			print_error(PRINT_NUMBER, "number must follow '#'");
			return line;
		}
	}
	if (operands[operands_cnt].type != new_line_tok) /* there are operands */
	{
		operands_cnt++; /* one operand */
		line = get_token(line, &(operands[operands_cnt]));
		if (operands[operands_cnt].type == '(') /* there goind to be three operands */
		{
			jump_addressing_flag = YES_JMP;
			line = get_token(line, &(operands[operands_cnt]));
			if (operands[operands_cnt].type == number_tok) /* cant be a number without '#' */
			{
				print_error(PRINT_NUMBER,"number must begin with hash ('#') sign");
				return line;
			}
			if (operands[operands_cnt].type == '#') /* instant_addressing */
			{
				line = get_token(line, &(operands[operands_cnt])); /* get the number */
				if (operands[operands_cnt].type != number_tok) /* a # sign without a number after */
				{
					print_error(PRINT_NUMBER, "number must follow '#'");
					return line;
				}
			}
			operands_cnt++;
			line = get_token(line, &(operands[operands_cnt]));
		}
		if (operands[operands_cnt].type == ',') /* there going to be 2 operands, seperated with comma */
		{
			line = get_token(line, &(operands[operands_cnt])); /* get the second operand */
			if (operands[operands_cnt].type == number_tok) /* cant be a number without '#' */
			{
				print_error(PRINT_NUMBER,"number must begin with hash ('#') sign");
				return line;
			}
			if (operands[operands_cnt].type == '#') /* instant_addressing */
			{
				line = get_token(line, &(operands[operands_cnt])); /* get the number */
				if (operands[operands_cnt].type != number_tok) /* a # sign without a number after */
				{
					print_error(PRINT_NUMBER, "number must follow '#'");
					return line;
				}
			}
			if (operands[operands_cnt].type == new_line_tok) /* missing operand */
			{
				print_error(PRINT_NUMBER, "comma at the end of the line");
			}
			else /* valid operand */
			{
				operands_cnt++; /* two operands */
				line = get_token(line, &(operands[operands_cnt]));
				if (jump_addressing_flag) /* ther should be a ")" at the end */
				{
					if (operands[operands_cnt].type != ')') /* error */
					{
						print_error(PRINT_NUMBER, "missing closing bracket");
						return line;
					}
					else
					{
						line = get_token(line, &(operands[operands_cnt]));
					}
				}
				if (operands[operands_cnt].type != new_line_tok) /* after two operands must be end of the line */
				{
					print_error(PRINT_NUMBER, "invalid parameter after the operands ");
					return line;
				}
			}
		}
		else if (jump_addressing_flag) /* there where open brackest, but only one operand */
		{
			print_error(PRINT_NUMBER, "missinig operands");
			return line;
		}
		else if (operands[operands_cnt].type != new_line_tok) /* only one operand */
		{
			print_error(PRINT_NUMBER, "wrong argument after the operand");
			return line;
		}
	}

	/* operands validation checks */
	if (!(is_valid_operands(command, operands, operands_cnt)))
	{
		return line;
	}

	/* adds the commands and the operands into the instruction array */
	add_to_instruction_array(command, operands, operands_cnt);
	return line;
}

/* checks if the operands recieved are valid to this command.
the function returns 0 if not valid, and 1 if they are valid  */
int is_valid_operands(Token *command, Token *operands, int operands_cnt)
{
	int num_of_operands = commands_table[command->data.command].num_of_operands; /* getting the data from the commands table */
	int allowed_jmp_addrss[] = { 1,1,0,1 }; /* allows instant_addressing, direct_addressing, and register_addressing */

	/* checking the amount of operands supllied */
	if ((num_of_operands != operands_cnt) &&
		((num_of_operands != 1) || (operands_cnt != 3))) /* only 1 and 3 can be diffarent, in case of juming addressing mode */
	{
		print_error(PRINT_NUMBER, "invalid number of operands");
		return 0;
	}

	/* validating the addressing modes for this command */
	if (num_of_operands == 0)
	{
		return 1; /* the operands are valid */
	}
	else if (num_of_operands == 1)
	{
		if (operands_cnt == 3) /* jump addressing mode */
		{
			/* valid only if:
			1. The first parameter is a lable
			2. The command allowes jump addressing mode
			3. The other two parameters are allowed in the jump addressing mode */
			if (operands[0].type == lable_tok)
			{
				if ((commands_table[command->data.command].dest_addressing_modes)[jump_addressing]) /* the command allowes addressing mode */
				{
					if ((allowed_jmp_addrss[get_addressing_mode(operands[1])]) &&
						(allowed_jmp_addrss[get_addressing_mode(operands[2])]))
					{
						return 1;
					}
				}
			}
		}
		else /* only destination addressing mode, one operand */
		{
			if ((commands_table[command->data.command].dest_addressing_modes)[get_addressing_mode(operands[0])]) /* the only operand */
			{
				return 1;
			}
		}
	}
	else /* two operands */
	{
		if (((commands_table[command->data.command].source_addressing_modes)[get_addressing_mode(operands[0])])
			&& ((commands_table[command->data.command].dest_addressing_modes)[get_addressing_mode(operands[1])]))
		{
			return 1; /* is valid */
		}
	}
	print_error(PRINT_NUMBER, "invalid operand");
	return 0;
}

/* this function gets a token, and returns it's addressing mode:
instant_addressing,
direct_addressing,
or register_addressing.
if it's not one of the above - the function returns -1 */
int get_addressing_mode(Token operand)
{
	if (operand.type == number_tok) return instant_addressing;
	else if (operand.type == lable_tok) return direct_addressing;
	else if (operand.type == register_tok) return register_addressing;
	else
	{
		return -1;
	}
}


/* this function adds command and it's operands, into the instruction array.
this function is being called only if al the validation checks are OK,
so this function desn't do any validation checks */
void add_to_instruction_array(Token *command, Token operands[], int operands_cnt)
{
	int i;
	/* add the command word */
	add_to_mem(command->data.command, COMMAND_OPCODE);
	if (operands_cnt == 0) /* no operadnds to add */
	{
		IC++;
		return;
	}
	else if (operands_cnt == 1)
	{
		add_to_mem(get_addressing_mode(operands[0]), DEST_ADDRESS);
	}
	else if (operands_cnt == 2)
	{
		add_to_mem(get_addressing_mode(operands[0]), SRC_ADDRESS);
		add_to_mem(get_addressing_mode(operands[1]), DEST_ADDRESS);
	}
	else if (operands_cnt == 3)
	{
		add_to_mem(jump_addressing, DEST_ADDRESS);
		add_to_mem(get_addressing_mode(operands[1]), PARAM_1);
		add_to_mem(get_addressing_mode(operands[2]), PARAM_2);
	}

	/* adding the other memory words */
	IC++;
	for (i = 0; i<operands_cnt; i++)
	{
		if (operands[i].type == lable_tok)
		{
			add2lable_table(&lable_list, &(operands[i]), CODE_LABLE); /* CODE_LABLE is the type of lable to be added */
		}
		else if (operands[i].type == number_tok)
		{
			add_to_mem(operands[i].data.number, NUM); /* adds after the E,A,R part of the memory word */
		}
		else if (operands[i].type == register_tok)
		{
			if (operands_cnt>1)
			{
				if (i<operands_cnt - 1) /* the source register */
				{
					add_to_mem(operands[i].data.reg, SRC_REG); /* adds after the E,A,R part of the memory word */
				}
				else /* the destination register */
				{
					if (operands[i - 1].type == register_tok) /* if there are two registers, they add to the same memory word */
					{
						IC--; /* adds the register in the same memory word */
					}
					add_to_mem(operands[i].data.reg, DEST_REG);
				}
			}
		}
		IC++;
	}
}

/* adds a number to the memory array (instruction array only), at the right offset in the currand IC location.
example: the call: add_to_mem(1,7), adds the number 1 (in binary) in 7 offset:
memory word:
               <--7 offset-|
 0 0 0 0 0 0 7 0 0 0 0 0 0 0
 */
void add_to_mem(int num_to_add, int offset)
{
	instruction_array[IC] |= (num_to_add << offset);
}
