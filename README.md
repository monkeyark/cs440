# cs440

## Name
Next Generation Compiler But God Know If It Would Work.

## Description
NGCBDKIIWW is a compiler for a subset of the C programming language.

## Installation
Move to the Source directory. All files will be generated in Source directory after compile.

Compile
```
make
```

Remove generated files
```
make clean 
```

## Useage
Usage:

	mycc -mode [options] infile

Valid modes:

	-0: Version information only

	-1: Lexer for C

	-2: C parser

	-3: Type checking

	-4: Code generation: Expressions

	-5: Part 5 (not yet implemented)

Valid options:

	-o outfile: write to outfile instead of standard output


## Project status
Part 0 is implemented

Part 1 is under development
