# cs440

## Name
Next Generation Compiler But God Know If It Would Work.

## Description
NGCBDKIIWW is a compiler for a subset of the C programming language.

## Directory

	Documentation

Developer Doc

	Homework

Homeowork file and code

	Project

Project description and test code

	Source

The NGCBDKIIWW compiler code


## Installation
For compiler

Compile NGCBDKIIWW executable
```
make
```

Remove compiled binary and linking files
```
make clean 
```

Remove all generated files
```
make clobber
```

For developer note

Compile Latex file of developer notes
```
make
```

Remove generated documentations
```
make clean
```

Remove all generated files
```
make clobber
```

## Useage
Usage:

	mycc -mode [options] infile

Valid modes:

	-0: Version information

	-1: Lexer for C

	-2: C parser

	-3: Type checking

	-4: Code generation: Expressions

	-5: Part 5

Valid options:

	-o outfile: write to outfile instead of standard output


## Project status
The part mark with * is under development.

	-0: Version information only
	-1: Lexer for C
	*-2: C parser
	*-3: Type checking
	*-4: Code generation: Expressions
	*-5: Part 5
