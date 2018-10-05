/* ====================================================

An assebler, written in C

Project by: Alon Gouldman & Yosi Cohen

this is an assembler. this program translates an assembly code, into a machine code.
this is an assembler in one (and a half) pass over the source code.
the assembler goes over each line of the assembly code provided.
the lexer reads the text and translates it into tokens, and the parser translates the
tokens to machine code.

finally, three files are being generated:
1. the machine code, in a file with ".ob" suffix
2. a list of all entry lables, in a file with ".ent" suffix
3. a list of all extern lables, in a file with ".ext" suffix

======================================================*/
#ifndef ASSEMBLER
#define ASSEMBLER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* general definitions */
#define MAX_LINE 80
#define MAX_LABLE 31
#define FIRST_MEMORY_ADDRESS 100 /* can be changed */
#define MAX_MEMORY_ARRAY 500 /* random length */
#define BITS_IN_MEMORY_WORD 14 /* project definition */

/*==== files ====*/
/* file's suffix */
#define INPUT_FILE_SUFFIX ".as"
#define OUTPUT_FILE_SUFFIX ".ob"
#define ENTRY_FILE_SUFFIX ".ent"
#define EXTERN_FILE_SUFFIX ".ext"
#define ZERO_OUTPUT '.' /* the character to be outputed when the memory has '0' in binary */
#define ONE_OUTPUT '/' /* the character to be outputed instead of '1' in binary */
#define DIGITS_IN_OUTPUT 4 /* e.g. 1234 has 4 digits, and also 0078 */

/* commands */
#define MAX_OPERANDS 3 /* maximum number of operands to a single command */
#define NUM_OF_ADDRESSING_MODES 4

/* checks if a charachret is space or tab */
#define IS_WHITE(x) (((x) == ' ')||((x) == '\t'))


enum error { NO_ERRORS, ERROR_FOUND }; /* error flag */
enum print_line_num { PRINT_NUMBER, DONT_PRINT_NUMBER }; /* print error flag */

/* types of addressing modes of the command operands */
enum addressing_modes {
	instant_addressing,
	direct_addressing,
	jump_addressing,
	register_addressing
};

/*type of memory word */
enum A_R_E {
	ABSOLUTE = 0,
	EXTERNAL = 1,
	RELOCATABLE = 2
};

/* types of tokens in the assembly program.
the token types are negative, so they will not conflict with the ASCII of a character */
enum tokens {
	lable_tok = -1,
	number_tok = -2,
	instruction_tok = -3,
	string_tok = -4,
	command_tok = -5,
	register_tok = -6,
	new_line_tok = -7,
	error_tok = -8
};

/* types of instructions */
enum instructions {
	STRING,
	DATA,
	ENTRY,
	EXTERN
};

/* all the register in the computer */
enum reg_type {
	R0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7
};

/* this is a token in the file. the token can be one of these following types, or a character.
each token has it's type, and some data (according to it's type) */
typedef struct Token {
	int type;
	union {
		char lable_name[MAX_LABLE];
		char string[MAX_LINE];
		int number;
		char ch;
		int command;
		int reg;
		int instruction;
	}data;
}Token;

typedef struct lable_occur *occp;

/* this is one occurrence of a lable in the program */
typedef struct lable_occur {
	int line;
	occp next;
}lable_occur;

typedef struct lable_t *lablep;

/* this is a lable in the program */
typedef struct lable_t {
	char name[MAX_LABLE]; /* the name of the lable */
	int declaration; /* the location in memory that this lable was declared */
	occp occurrence; /* a linked list of all the occurrences of this lable in the code */
	int data_or_instruction; /* the type of the lable */
	int entry_extern;
	lablep next;
}lable_t;

/* types of lables in the assably code */
enum lable_type {
	EXTERN_LABLE,
	ENTRY_LABLE,
	CODE_LABLE_DECLARATION,
	DATA_LABLE,
	CODE_LABLE, /* used, not declared */
	NOT_DECLARED = -1
};

/***********************************
memory word structure:

+----------------+------------------+---------+------------------+------------------+-------+
| 13     12      |    11     10     | 9 8 7 6 |    5    4        |     3    2       |  1  0 |
+----------------+------------------+---------+------------------+------------------+-------+
|first Parameter | second Parameter | opcode  | source           | destination      | E,R,A |
\                |                  |         | addressing mode  | addressing modes |       |
+----------------+------------------+---------+------------------+------------------+-------+

************************************/

enum memory_word_offset { /* what is the offset of this memory word part from the begginnig? */
	ERA = 0,
	DEST_ADDRESS = 2,
	SRC_ADDRESS = 4,
	COMMAND_OPCODE = 6,
	PARAM_2 = 10,
	PARAM_1 = 12,

	NUM = 2,
	SRC_REG = 8,
	DEST_REG = 2
};

/* lexing functions */
char *get_token(char *line, Token *curr_token);
char * get_string_token(char *line, Token *curr_token);
int get_register(char *word);
int get_instruction(char *word);

/* parsing */
void assamble(FILE *fp);
void data_to_dataArray(int input);
void string_to_dataArray(char *input);

/*lables functions */
int add_lable_occurrence(occp *head, int line);
lablep find_lable(lablep head, Token *lable);
void add2lable_table(lablep *head, Token *lable, int status);
void parse_lables();
void lable2array(int number, occp occurrence_list);
int is_valid_lable(char *tok);
void freelist(lablep *head);
void free_occur_list(occp *head);

/* commands functions */
int get_command(char *word);
char *parse_command(Token *command, char *line);
int is_valid_operands(Token *command, Token *operands, int operands_cnt);
int get_addressing_mode(Token operand);
void add_to_instruction_array(Token *command, Token operands[], int operands_cnt);
void add_to_mem(int num_to_add, int offset);

/* file handeling */
FILE *open_file(const char *file_name, char *file_suffix, char *mode);
void create_object_file(char *file_name);
void create_label_files();
void create_files();

/* utility functions */
void print_error(int print_, char *error, ...);
void zero_array(short int array[], int length);

/* global variables */
extern int is_error; /* error flag */
extern int line_counter;
extern int DC; /* data array counter */
extern int IC; /* instruction and commands array counter */
extern short int instruction_array[]; /* the memory array of the instruction part */
extern short int data_array[]; /* the memory array for the data part */
extern lablep lable_list; /* a linked list of lables */


#endif
