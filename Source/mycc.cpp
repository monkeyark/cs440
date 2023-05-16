#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cctype>

#include "lex.hpp"
#include "parse.hpp"
#include "type.hpp"
#include "gen.hpp"
#include "gencf.hpp"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;

#define COMPILER_USAGE "Usage:\n\
	mycc -mode [options] infile \n\
Valid modes:\n\
	-0: Version information only\n\
	-1: Lexer for C\n\
	-2: C parser\n\
	-3: Type checking\n\
	-4: Code generation: expressions\n\
	-5: Code generation: control flow\n\
Valid options:\n\
	-o outfile: write to outfile instead of standard output"

#define COMPILER_INFO "Next-Generation-Compiler-But-God-Know-If-It-Would-Work\n\
	Wirrten by Zhi Wang (danryw@iastate.edu)\n\
	Version 0.01	Released Jan 26 2023"


void print_file(string file_path)
{
	ifstream file(file_path);
	string text, line;
	while (getline(file, line))
	{
		text = text + line + "\n";
	}
	cout << text << endl;
	file.close();
}

void save_file(string out_path, int num)
{
	ofstream file(out_path);
	switch(num)
	{
		case 0 :
			file << COMPILER_INFO << endl;
			break;
		case 1 :
			file << "Lexer for C" << endl;
			break;
		case 2 :
			file << "C Preprocessor" << endl;
			break;
		case 3 :
			file << "C Parser" << endl;
			break;
		case 4 :
			file << "Type checking" << endl;
			break;
		case 5 :
			file << "Code generation: expressions" << endl;
			break;
		default :
			break;
	}
	file.close();
}

void part0(int out, string out_path)
{
	if (out)
	{
		save_file(out_path, 0);
	}
	else
	{
		cout << COMPILER_INFO << endl;
	}
}

void part1(int in, int out, string in_path, string out_path)
{
	lex_file(in_path, true);
}

void part2(int in, int out, string in_path, string out_path)
{
	parse_file(in_path, true);
}

void part3(int in, int out, string in_path, string out_path)
{
	type_check_file(in_path, true);
}

void part4(int in, int out, string in_path, string out_path)
{
	gen_file(in_path, true);
}

void part5(int in, int out, string in_path, string out_path)
{
	gen_cf_file(in_path, true);
}

int main(int argc, char *argv[])
{
	string input_path;
	string output_path;
	bool is_input = false;
	bool is_output = false;
	int num = -1;
	char *c;
	if (argc == 1)
	{
		cout << COMPILER_USAGE << endl;
	}
	else
	{
		c = argv[1] + 1;
		if (isdigit(*c))
		{
			num = atoi(argv[1] + 1);
			if (argc == 2)
			{
				//mycc -mode
				if (num != 0) return -1;
			}
			else if (argc == 3)
			{
				//mycc -mode infile
				is_input = true;
				input_path = string(argv[2]);
			}
			else if ((argc == 4) && (strcmp(argv[2], "-o") == 0))
			{
				//mycc -mode -o outfile
				is_output = 1;
				output_path = string(argv[3]);
			}
			else if ((argc == 5) && (strcmp(argv[2], "-o") == 0))
			{
				//mycc -mode -o outfile infile
				is_input = true;
				is_output = true;
				output_path = string(argv[3]);
				input_path = string(argv[4]);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	
	switch(num)
	{
		case 0 :
			part0(is_output, output_path);
			break;
		case 1 :
			part1(is_input, is_output, input_path, output_path);
			break;
		case 2 :
			part2(is_input, is_output, input_path, output_path);
			break;
		case 3 :
			part3(is_input, is_output, input_path, output_path);
			break;
		case 4 :
			part4(is_input, is_output, input_path, output_path);
			break;
		case 5 :
			part5(is_input, is_output, input_path, output_path);
			break;
		default :
			break;
	}

	return 0;
}
