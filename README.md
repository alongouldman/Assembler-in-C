Assembler in C

This is an assembler, tranlating the assembly language to bits (0 and 1).



this is an assembler. this program translates an assembly code, into a machine code.
this is an assembler in one (and a half) pass over the source code.
the assembler goes over each line of the assembly code provided.
the lexer reads the text and translates it into tokens, and the parser translates the
tokens to machine code.

finally, three files are being generated:
1. the machine code, in a file with ".ob" suffix
2. a list of all entry lables, in a file with ".ent" suffix
3. a list of all extern lables, in a file with ".ext" suffix


<h2>Usage:</h2>

