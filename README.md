<h1>Assembler in C</h1>

This is an assembler, tranlating the assembly language to machine code (0 and 1 bits of memory "words").

this is an assembler. this program translates an assembly code, into a machine code.
this is an assembler in one (and a half) pass over the source code.
the assembler goes over each line of the assembly code provided.
the lexer reads the text and translates it into tokens, and the parser translates the
tokens to machine code.

memory word structure:

+----------------+------------------+---------+------------------+------------------+-------+

| 13     12      |    11     10     | 9 8 7 6 |    5    4        |     3    2       |  1  0 |

+----------------+------------------+---------+------------------+------------------+-------+

|first Parameter | second Parameter | opcode  | source           | destination      | E,R,A |
|                |                  |         | addressing mode  | addressing modes |       |

+----------------+------------------+---------+------------------+------------------+-------+

<h2>Output</h2>

Three files are being generated:
1. the machine code, in a file with ".ob" suffix
2. a list of all entry lables, in a file with ".ent" suffix
3. a list of all extern lables, in a file with ".ext" suffix


<h2>Usage:</h2>

