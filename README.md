<h1>Assembler in C</h1>
etkes



This is an assembler, tranlating the assembly language to machine code (0 and 1 bits of memory "words").

this is an assembler. this program translates an assembly code, into a machine code.
this is an assembler in one (and a half) pass over the source code.
the assembler goes over each line of the assembly code provided.
the lexer reads the text and translates it into tokens, and the parser translates the
tokens to machine code.

memory word structure:
```
+----------------+------------------+---------+------------------+------------------+-------+
| 13     12      |    11     10     | 9 8 7 6 |    5    4        |     3    2       |  1  0 |
+----------------+------------------+---------+------------------+------------------+-------+
|first Parameter | second Parameter | opcode  | source           | destination      | E,R,A |
|                |                  |         | addressing mode  | addressing modes |       |
+----------------+------------------+---------+------------------+------------------+-------+
```
<h2>Output</h2>

Three files are being generated:
1. the machine code, in a file with ".ob" suffix
2. a list of all entry lables, in a file with ".ent" suffix
3. a list of all extern lables, in a file with ".ext" suffix


<h2>Usage:</h2>

<h4>Compilation</h4>
To compile, run:

```
make
```


in the command line (in the folder of the files)

<h4>Assembling files</h4>

```
assembler FILENAME1 FILENAME2 FILENAME3
```

you can run how many files you want at once, just seperate with spaces. 
note:
the name of the files should end with ".as" suffix, but when running dont write the end. 

<h4>Example:</h4>
if the name of the file is "fileToAssemble.as" and "anotherFile.as", run:

```
assembler fileToAssemble anotherFile
```

if there where no errors in the files, the output will be:
1. fileToAssemble.ob
2. fileToAssemble.ext
3. fileToAssemble.ent
