#include <stdio.h>
#include <stdlib.h>

#define SIZE 4

void read_int(FILE *in, char *lexeme, unsigned max_length)
{
	char buffer[max_length+1];
	int count = 3; // counter for every three digits
	int checked = 0; // checker flag to determine if before first comma
	int errored = 0; // if error produced
	char c;
	int i = 0;

	while (1)
	{
		c = fgetc(in);
		// read all digits and commas, then save them into buffer
		if ((c >= '0' && c <= '9') )
		{
			buffer[i] = c;
			i++;
			i %= max_length;
			count--;
		}
		else
		{
			// validate buffter content
			if (count != 0) // count is 0 when reach a comma
			{
				if(checked == 0)
				{
					checked = 1; // passed the first comma
				}
				else
				{
					fprintf(stderr, "Error: expected digit\n");
					errored = 1;
					//clear buffer and goes to newline
					for (int k = 0; buffer[k] != 0; k++)
					{
						buffer[k] = 0;
						i = 0; //reset buffer index
					}
				}
				
			}
			if (count < 0) // consumed more than 3 digits
			{
				fprintf(stderr, "Error: expected comma\n");
				errored = 1;
				//clear buffer and goes to nextline
				for (int k = 0; buffer[k] != 0; k++)
				{
					buffer[k] = 0;
					i = 0;
				}
			}
			if (c == ',' && errored == 0) // consume a comma. If it's the not the first comma reset digit count
			{
				count = 3;
				continue;
			}
			while (c != '\n') // consume everything until newline
			{
				c = fgetc(in);
				if (c == EOF) return; // if reach EOF return
			}

			// the buffer is fulled with lexeme in one line
			// reset all flags for newline
			count = 3;
			checked = 0;
			errored = 0;

			//write buffer content to lexeme
			for (int j = 0; j < max_length; j++)
			{
				// printf("%c", buffer[j]);
				*lexeme = buffer[j];
				lexeme++;
			}
			*lexeme = '\n';
			lexeme++;

			printf("%s\n",lexeme); // test code

			//clear buffer
			for (int k = 0; buffer[k] != 0; k++)
			{
				buffer[k] = 0;
				i = 0;
			}
		}
	}
}


int main(int argc, char** argv)
{
	FILE *infile = fopen("hw1_infile", "r");
	char *lexeme = malloc(1000);
	read_int(infile, lexeme, 4);
	fclose(infile);
	// printf("%s", lexeme);
	return 0;
}