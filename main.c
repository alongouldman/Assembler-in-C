#include "assembler.h"

/* ======= gobal variables ================= */
int line_counter;
int IC, DC;
int is_error;
short int instruction_array[MAX_MEMORY_ARRAY];
short int data_array[MAX_MEMORY_ARRAY];
int num_of_tokens;
lablep lable_list;

int main(int argc, char const *argv[])
{
	FILE *fp;
	int file_count;

	if (argc < 2) {
		print_error(DONT_PRINT_NUMBER, "No files submitted");
		return 1;
	}
	for (file_count = 1; file_count<argc; file_count++) /* for every file in command line */
	{
		fprintf(stderr, "\nOpening file: %s%s ....\n", argv[file_count], INPUT_FILE_SUFFIX);
		fp = open_file(argv[file_count], INPUT_FILE_SUFFIX, "r");
		if (fp == NULL) /* some error occured, was printed in the function */
		{
			continue; /* file could not be opened, moving to the next file */
		}

		/* initielize  global variables */
		is_error = NO_ERRORS;
		line_counter = 0;
		IC = 0;
		DC = 0;
		zero_array(instruction_array, MAX_MEMORY_ARRAY);
		zero_array(data_array, MAX_MEMORY_ARRAY);
        lable_list = NULL;

		assamble(fp); /* main function of the assembler */

		if (is_error == ERROR_FOUND)
		{
			print_error(DONT_PRINT_NUMBER, "Errors found on %s%s, Can't assamble file\n", argv[file_count], INPUT_FILE_SUFFIX);
			continue;
		}
		create_files(argv[file_count]);
		fprintf(stderr, "\n--DONE--\n\n" );
		
		freelist(&lable_list);
		fclose(fp);
	}
	return 0;
}
