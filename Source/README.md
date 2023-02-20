## Part Description

### Part 0 - Version information

mycc.cpp:

Main function of the program with takes arguments from standard
input as switch to enter specified mode. Version information of the project.

### Part 1 - C Lexer

lex.h:

Header file of the lexer. Defined token attributes and error messages.

lex.cpp:

A lexer that read the current and next char from the input
file and then output token when reach a token terminator or end of file.

	void lex_file(string path)
	void lex_text(string text, string fname)

take input from file and process the input character by character from fname.
The function use multiple flag variables to check if the current char is in comment
of c/c++ or in literals of string/character. There is also flag to check if the
current char is in a real literals or include path. When a flag is enable, the
current char will be consumed and until reach the end of the token by terminate
checking. Whenever a token is found, it will calls

	void output_token(string lexeme, int line, string fname)

to search if the current token is vaild one and output the token with its tokenid.