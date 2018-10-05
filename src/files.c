#include "assembler.h"

/* This function opens a file, with a given suffix, in a given mode */
FILE *open_file(const char *file_name, char *file_suffix, char *mode)
{
	FILE *fp;
	char *file_name_with_suffix = (char *)malloc(strlen(file_name) + strlen(file_suffix) + 1); /* allocating space for the new file name, with the suffix */
	if (file_name_with_suffix == NULL) {
		print_error(DONT_PRINT_NUMBER, "\nError: error allocating memory");
		return NULL;
	}
	strcpy(file_name_with_suffix, file_name);
	strcat(file_name_with_suffix, file_suffix);
	fp = fopen(file_name_with_suffix, mode);
	if (fp == NULL) {
		print_error(DONT_PRINT_NUMBER, "\nError: could not open file \"%s\"", file_name_with_suffix);
		return NULL;
	}
	return fp;
}

/* this function creates the output files:
1. the machine code, in a file with ".ob" suffix
2. a list of all entry lables, in a file with ".ent" suffix
3. a list of all extern lables, in a file with ".ext" suffix
*/
void create_files(char *file_name)
{
	fprintf(stderr, "\nCreating files...");
	create_object_file(file_name);
	create_label_files(file_name);
}
/* Creates the .obj file, which contains the assembled lines in base 10. */
void create_object_file(char *file_name)
{
	int line_num = FIRST_MEMORY_ADDRESS;
	int i, j, k,  mem_word; /* three counters and a mem_word value */
	FILE *file;
	char number_in_str[MAX_LINE];
	unsigned short int mask = ((1 << (BITS_IN_MEMORY_WORD - 1)));
	file = open_file(file_name, OUTPUT_FILE_SUFFIX, "w");
	/* check if the array is empty */
	if ((DC + IC) == 0)
	{
		fprintf(stderr, "%s%s is empty file", file_name, INPUT_FILE_SUFFIX);
		return;
	}
	else
	{
		/* headline */
		fprintf(file, "\t%d\t%d\n", IC, DC);
		/*print the IC and DC array to a file*/
		/*IC array*/
		for (i = 0; i<IC; i++)
		{
			mem_word = instruction_array[i];
			/* all the lines's number are printed in a 4 digit output; line 34 will be outputted 0034 */
			sprintf(number_in_str, "%d",line_num + i); /* changing the line number to string */
			for(k=DIGITS_IN_OUTPUT - strlen(number_in_str) ; k >0; k--)
			{
				fprintf(file, "0" ); /* printing leading zero */
			}
			fprintf(file, "%d\t", line_num + i);
			for (j = 0; j<BITS_IN_MEMORY_WORD; j++)
			{
				if (mem_word&(mask >> j))
					fputc(ONE_OUTPUT, file);
				else
					fputc(ZERO_OUTPUT, file);
			}
			fprintf(file, "\n");
		}
		/*DC array*/
		line_num += i;
		for (i = 0; i<DC; i++)
		{
			mem_word = data_array[i];
			sprintf(number_in_str, "%d",line_num + i); /* changing the line number to string */
			for(k=DIGITS_IN_OUTPUT - strlen(number_in_str) ; k >0; k--)
			{
				fprintf(file, "0" ); /* printing leading zero */
			}
			fprintf(file, "%d\t", line_num+i);
			for (j = 0; j<BITS_IN_MEMORY_WORD; j++)
			{
				if (mem_word&(mask >> j))
					fputc(ONE_OUTPUT, file);
				else
					fputc(ZERO_OUTPUT, file);
			}
			fprintf(file, "\n");
		}
	}
	fclose(file);
}

/* this function creates:
1. a list of all entry lables, in a file with ".ent" suffix
2. a list of all extern lables, in a file with ".ext" suffix
*/
void create_label_files(char *file_name)
{
	FILE *entry_file, *extern_file;
	lablep curr = lable_list;
	int was_entry = 0, was_extern = 0;
	int address;
	occp occur_list;

	entry_file = open_file(file_name, ENTRY_FILE_SUFFIX , "w");
	extern_file = open_file(file_name, EXTERN_FILE_SUFFIX , "w");

	while(curr) /* going over all the lables */
	{
		if (curr->entry_extern == EXTERN_LABLE) /* lable is extern lable */
		{
			was_extern = 1; /* extern lable found */
			occur_list = curr->occurrence;
			address = FIRST_MEMORY_ADDRESS;
			if(curr->data_or_instruction == DATA_LABLE)
			{
				address+=IC;
			}
			while(occur_list) /* to all occurences */
			{
				fprintf(extern_file, "%s\t%d\n", curr->name, (occur_list->line) + address);
				occur_list = occur_list->next;
			}
		}
		else if(curr->entry_extern == ENTRY_LABLE)
		{
			was_entry = 1;
			address = FIRST_MEMORY_ADDRESS + curr->declaration;
			if(curr->data_or_instruction == DATA_LABLE)
			{
				address+=IC;
			}
			fprintf(entry_file, "%s\t%d\n", curr->name, address);
		}
		curr = curr->next;
	}
	fclose(entry_file);
	fclose(extern_file);
	if(!was_entry) /* if no entry lable found at all */
	{
		remove(strcat(file_name,ENTRY_FILE_SUFFIX )); /* deleteing the file */
	}
	if(!was_extern) /* if no extern lable found at all */
	{
		remove(strcat(file_name,EXTERN_FILE_SUFFIX )); /* ddeleteing the file */
	}
}
