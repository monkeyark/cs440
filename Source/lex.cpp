#include "lex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::ifstream;
using std::unordered_set;
using std::unordered_map;

unordered_set<char> symbols ({
	'!',//33
	'&',//38
	'%',//37
	'(',//40
	')',//41
	'*',//42
	'+',//43
	',',//44
	'-',//45
	'.',//46
	'/',//47
	':',//58
	';',//59
	'<',//60
	'=',//61
	'>',//62
	'?',//63
	'[',//91
	']',//93
	'{',//123
	'|',//124
	'}',//125
	'~',//126
});

unordered_set<char> escape ({
	' ',
	'\a',
	'\b',
	'\n',
	'\r',
	'\t',
	'\'',
	'\\'
});

unordered_map<string, int> operators ({
	{"==", 351},
	{"!=", 352},
	{">=", 353},
	{"<=", 354},
	{"++", 355},
	{"--", 356},
	{"||", 357},
	{"&&", 358},
	{"+=", 361},
	{"-=", 362},
	{"*=", 363},
	{"/=", 364}
});

unordered_map<string, int> keywords ({
	{"const", 401},
	{"struc", 402},
	{"for", 403},
	{"while", 404},
	{"do", 405},
	{"if", 406},
	{"else", 407},
	{"break", 408},
	{"continue", 409},
	{"return", 410},
	{"switch", 411},
	{"case", 412},
	{"default", 413}
});

unordered_set<string> types ({
	"void",
	"int",
	"double",
	"float",
	"char",
	"byte",
	"short",
	"long",
	"bool"
});

string filename;

int is_escape(char c)
{
	return (c == ' ') || (c == '\a') || (c == '\b') ||
			(c == '\n') || (c == '\r') || (c == '\t') ||
			(c == '\'') || (c == '\\');
}

int is_space(char c)
{
	return (c == ' ');
}

int is_newline(char c)
{
	return (c == '\n');
}

int is_c_comment_start(char a, char b)
{
	return (a == '/') && (b == '*');
}

int is_cpp_comment_start(char a, char b)
{
	return (a == '/') && (b == '/');
}

int is_c_comment_end(char a, char b)
{
	return (a == '*') && (b == '/');
}

int is_cpp_comment_end(char a, char b)
{
	return (a == '\n');
}

bool is_number(const string &str)
{
    for (char const &c : str)
	{
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

bool is_vaild_token(string token)
{
	char c = token[0];
	if (!isdigit(c) && !isalpha(c) && symbols.count(c))
	{
		return false;
	}
	else if (token.size() <= 1)
	{
		return false;
	}
	return true;
}

void output_token(string lexeme, int line)
{
	if (lexeme.empty() || escape.find(lexeme[0]) != escape.end())
	{
		return;
	}

	if (!is_vaild_token(lexeme))
	{
		cout <<
			"Lexer error in file " << filename <<
			" Line " << std::right << std::setw(5) << line <<
			" Near Text " << std::right << std::setw(5) << lexeme <<
		endl;
		return;
	}

	int tokenid = -1;

	cout <<
		"File " << filename <<
		" Line " << std::right << std::setw(5) << line <<
		" Token " << std::right << std::setw(5) << tokenid <<
		" Text " << lexeme <<
	endl;
}

void lex_text(string text)
{
	string token;
	string output;
	bool in_c_comment = false;
	bool in_cpp_comment = false;
	bool in_quotation = false;
	bool in_apostrophe = false;
	bool in_backslash = false;
	int line = 1;
	for (long unsigned int i = 0; i < text.length(); i++)
	{
		char c = text[i];
		char c_next = text[i+1];

		if (c == '\n') line++;	//line counter

		//check for c and c++ style comments
		if (in_c_comment)
		{
			if (is_c_comment_end(c, c_next))
			{
				in_c_comment = false;
				i++;
			}
			continue;
		}
		else if (in_cpp_comment)
		{
			if (is_cpp_comment_end(c, c_next))
			{
				in_cpp_comment = false;
				i++;
			}
			continue;
		}

		if (is_c_comment_start(c, c_next))
		{
			in_c_comment = true;
			continue;
		}
		else if (is_cpp_comment_start(c, c_next))
		{
			in_cpp_comment = true;
			continue;
		}

		//current char is a symbol
		if (symbols.count(c) && !in_quotation && !in_apostrophe)
		{
			if (c == '.' && is_number(token)) //find an real number
			{
				token.push_back(c);
				continue;
			}

			//TODO need to deal with three symbols like === in operator check
			//check for current and next char for operators
			string s(1, c);
			s.push_back(c_next);
			if (operators.count(s))
			{
				token.push_back(c);
				token.push_back(c_next);
				i++;
			}
			else //output previous token when meet a symbol
			{
				output_token(token, line);
				token.clear();
				token.push_back(c);
			}
		}
		else //reading normal characters and digits
		{
			token.push_back(c);

			if (in_backslash)
			{
				in_backslash = false;
				continue;
			}
			if (c == '"' && !in_backslash)
			{
				in_quotation = !in_quotation;
			}
			else if (c == '\'')
			{
				in_apostrophe = !in_apostrophe;
			}
			if (c == '\\' && !in_backslash && in_quotation)
			{
				in_backslash = true;
			}
		}

		// if(!token.empty() && !in_quotation)
		// {
		// 	token.push_back(c);
		// }

		if (escape.count(c) || symbols.count(c)) // check symbols again to avoid read newline
		{
			output_token(token, line);
			token.clear();
		}
	}


	// cout << output << endl;//
}

void lex_file(string file_path)
{
	filename = file_path;
	ifstream file(filename);
	if (!file.is_open())
	{
		cerr << "Could not open the file - '" << filename << "'" << endl;
		return;
	}
	char c;
	vector<char> bytes;
	while (file.get(c))
	{
		bytes.push_back(c);
	}
	file.close();

	string text(bytes.begin(), bytes.end());
	lex_text(text);
	// for (const auto &i : bytes) cout << i;
	// cout << endl;
}

string print_file();
