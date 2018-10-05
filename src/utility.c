#include "assembler.h"

/* this function prints the error according to the line number it accured, and updates the error flag */
void print_error(int print_line, char *error, ...)
{
	va_list parameters_to_print;
	is_error = ERROR_FOUND; /* error flag */
	va_start(parameters_to_print, error);
	if (print_line == PRINT_NUMBER) /* an error during the pass over the file */
	{
		fprintf(stderr, "Error in line %d: ", line_counter);
	}
	else
	{
		fprintf(stderr, "Error: "); /* error not over the pass over the file */
	}
	vfprintf(stderr, error, parameters_to_print);
	fprintf(stderr, "\n");
	va_end(parameters_to_print);
}

/* this function puts 0 in all the cells in an array */
void zero_array(short int array[], int length)
{
	int i;
	for (i = 0; i<length; i++)
	{
		array[i] = 0;
	}
}
