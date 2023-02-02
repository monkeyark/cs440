#include <stdio.h>

#define BUFFERSIZE 4

void read_int(FILE *in, char *lexeme, unsigned max_length)
{
	char c;
	for (;;)
	{
		c = fgetc(in);
		if (c == EOF) return;
		//load file to buffer

		printf("%c",c);
	}
}


int main(int argc, char** argv)
{
	FILE *infile = fopen("hw1_infile", "r");
	char *lexeme;
	read_int(infile, lexeme, BUFFERSIZE);
	printf("%s", lexeme);
	return 0;
}