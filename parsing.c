#include "assembler.h"

/* this is the main function of this program. this function reads a file line by line, and parse the line into the memory array. */
void assamble(FILE *fp)
{
	char *full_line = (char *)malloc(sizeof(char) * MAX_LINE); /* full line from a file */
	Token *curr_token = (Token *)malloc(sizeof(Token));
	Token *lable_token = (Token *)malloc(sizeof(Token)); /* in case there is a lable to store in the line */
	char tempch;
	char *line;

	if ((curr_token == NULL) || (lable_token == NULL)) /* an error accured */
	{
		print_error(DONT_PRINT_NUMBER,"Error allocating memory");
		return;
	}
	lable_list = NULL; /* creating the lable table */

	/* reads every line */
	while (fgets(full_line, MAX_LINE + 2, fp) != NULL) /*MAX LINE LENGTH + 2 for the '\n' and '\0' */
	{
		line = full_line;
		line_counter++;

		/* validation checks */
		if (!strchr(line, '\n')) /* checks if new line is in the scanned line. If not, the input line was longer then the maximum allowed */
		{
			print_error(PRINT_NUMBER, "line must be shorter then %d characters", MAX_LINE);
			while ((tempch = fgetc(fp)) != '\n') /* "eating" all the rest of the too long line */
				;
			continue; /* skipping to the next line */
		}

		line = get_token(line, curr_token);

		if ((curr_token->type == ';') || (curr_token->type == new_line_tok)) /* comment or new line */
		{
			continue;
		}
		if (curr_token->type == lable_tok)
		{
			memcpy(lable_token, curr_token, sizeof(Token)); /* saving the lable token for later use */
			line = get_token(line, curr_token);

		 	if (curr_token->type != ':') /* lable must end with ":" in the declaration, if its not extern */
			{
				print_error(PRINT_NUMBER, "colon (:) must follow a lable declaration");
				continue;
			}
			line = get_token(line, curr_token); /* identify which type of lable is that */

			if (curr_token->type == instruction_tok)
			{
				if ((curr_token->data.instruction == EXTERN) || (curr_token->data.instruction == ENTRY)) /* only .data and .string can follow a lable */
				{
					/* error */
					print_error(PRINT_NUMBER, "this instruction is invalid after a lable");
					continue;
				}
				add2lable_table(&lable_list, lable_token, DATA_LABLE); /* adds to the lable table */
			}
			else if (curr_token->type == command_tok)
			{
				add2lable_table(&lable_list, lable_token, CODE_LABLE_DECLARATION);
			}
			else /* something else follows the lable */
			{
				print_error(PRINT_NUMBER, "invalid parameter after the lable");
				continue;
			}
		}
		if (curr_token->type == instruction_tok)
		{
			if (curr_token->data.instruction == DATA)
			{
				line = get_token(line, curr_token);
				if (curr_token->type == new_line_tok)
				{
					print_error(PRINT_NUMBER, "No data recieved after the .data instruction");
					continue;
				}
				while (curr_token->type != new_line_tok) /* numbers dosnt have a limit after .data*/
				{
					if (curr_token->type == number_tok)
					{
						data_to_dataArray(curr_token->data.number);
						line = get_token(line, curr_token);
						if (curr_token->type == ',') /* comma must seperat the numbers */
						{
							line = get_token(line, curr_token);
							if (curr_token->type == new_line_tok)
							{
								print_error(PRINT_NUMBER, "comma at the end of the line");
							}
						}
						else if (curr_token->type == new_line_tok) break; /* end of data command */
						else
						{
							print_error(PRINT_NUMBER, "Only numbers allowed in the .data instruction");
							break;
						}
					}
					else
					{
						print_error(PRINT_NUMBER, "Only numbers allowed in the .data instruction");
						break;
					}
				}
			}
			else if (curr_token->data.instruction == EXTERN)
			{
				line = get_token(line, curr_token); /* should be a lable */
				if (curr_token->type != lable_tok)
				{
					print_error(PRINT_NUMBER, "a lable must follow an .extern declaration");
					continue;
				}
				memcpy(lable_token, curr_token, sizeof(Token)); /* saving the lable token for later use */
				line = get_token(line, curr_token); /* nothing else should follow the lable */
				if (curr_token->type != new_line_tok)
				{
					/* error */
					print_error(PRINT_NUMBER, "this instruction is invalid after a lable");
					continue;
				}
				add2lable_table(&lable_list, lable_token, EXTERN_LABLE);
			}
			else if (curr_token->data.instruction == ENTRY)
			{
				line = get_token(line, curr_token);
				if (curr_token->type != lable_tok)
				{
					print_error(PRINT_NUMBER, "a lable must follow an .extern declaration");
					continue;
				}
				memcpy(lable_token, curr_token, sizeof(Token)); /* saving the lable token for later use */
				line = get_token(line, curr_token);
				if (curr_token->type != new_line_tok)
				{
					/* error */
					print_error(PRINT_NUMBER, "this instruction is invalid after a lable");
					continue;
				}
				add2lable_table(&lable_list, lable_token, ENTRY_LABLE);
			}
			else if (curr_token->data.instruction == STRING) /* a string token */
			{
				line = get_string_token(line, curr_token);
				if (curr_token->data.string[0] == '\0') /* empty string/invalid string */
				{
					print_error(PRINT_NUMBER, "invalid string");
					continue;
				}
				else /* valid string */
				{
					string_to_dataArray(curr_token->data.string);
					line = get_token(line, curr_token);
					if (curr_token->type != new_line_tok) /* a new line must follow a string */
					{
						print_error(PRINT_NUMBER, "A new line must follow a string");
						continue;
					}
				}
			}
		}
		else if (curr_token->type == command_tok)
		{
			line = parse_command(curr_token, line);
		}
		else
		{
			print_error(PRINT_NUMBER, "invalid parameter");
			continue;
		}
	}
	/* the data and instruction array is now missing the lable's addresses */
	parse_lables();
	free(full_line);
	free(curr_token);
	free(lable_token);
}

/* adds numbers into the data array.
the number will be stored as a binary representation of it's ASCII value */
void data_to_dataArray(int input)
{
	int max_num_available = (1 << (BITS_IN_MEMORY_WORD - 1)) - 1; /* this number is the biggest number possible in the data array. -1 is for the sign. */
																  /* check if there is memory for the data */
	if (DC + IC >= MAX_MEMORY_ARRAY)
		print_error(PRINT_NUMBER, "The program is too big. Not enough memory");
	if ((input <= max_num_available) && (input > -max_num_available - 1)) /* make sure the number can  fit into the memory */
	{
		data_array[DC] = input;
		DC++;
	}
	else
	{
		print_error(PRINT_NUMBER, "The number %d is too big. Not enough memory", input);
	}
}

/* adds string into the data array.
the string will be stored as a binary representation of it's ASCII value,
character by character, followed by a closing '\0' memory word  */
void string_to_dataArray(char *input)
{
	int j;
	if (strlen(input) == 0)   /*check if the string is empty*/
		print_error(PRINT_NUMBER, "Invalid string, the string is empty");
	for (j = 0; j<strlen(input); j++)
	{
		if (DC + IC >= MAX_MEMORY_ARRAY - 1) /* 1 for the '\0' in the end */
			print_error(PRINT_NUMBER, "The program is too big. Not enough memory");
		else
		{
			data_array[DC] = input[j]; /* inputing data into the data array */
			DC++;
		}
	}
    data_array[DC++] = 0; /* end of string */
}
