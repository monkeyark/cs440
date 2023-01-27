#include "lex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

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

unordered_map<string , int> operators ({
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

unordered_map<string , int> keywords ({
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
int is_comment_start(char a, char b)
{
	return (a == '/') && ((b == '/') || (b == '*'));
}

int is_comment_end(char a, char b)
{
	return ((a == '*') && (b == '/')) || (a == '\n');
}

bool is_number(const string& str)
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
	if (!isdigit(c) && !isalpha(c) && symbols.contains(c))
	{
		return false;
	}
	else if (token.size() <= 1)
	{
		return false;
	}
	return true;
}

void save_token(string lexeme, int line)
{
	if (lexeme.empty() || escape.find(lexeme[0]) != escape.end())
	{
		return;
	}

	if (!is_vaild_token(lexeme))
	{
		cout << "Lexer error in file " << filename <<
			" Line " << line <<
			" Near Text " << lexeme <<
		endl;
		return;
	}

	int tokenid = 301;

	cout <<
		"File " << filename <<
		" Line " << "  " << line <<
		" Token "  << "    " << tokenid <<
		" Text " << lexeme <<
	endl;

}

void lex_text(string text)
{
	string token;
	string output;
	bool in_comment = false;
	bool in_quotation = false;
	bool in_apostrophe = false;
	bool in_backslash = false; 
	int lines = 0;
	for (long unsigned int i = 0; i < text.length(); i++)
	{
		char character = text[i];
		char character_next = text[i+1];//TODO handle EOF

		if (character == '\n')
		{
			lines++;
		}
		//deal with end of comment
		if (in_comment)
		{
			if (is_comment_end(character, character_next))
			{
				in_comment = false;
				i++;
			}
			continue;
		}

		//deal with symbols
		if (symbols.contains(character) && !in_quotation && !in_apostrophe)
		{
			if (character == '.' && is_number(token))
			{
				token.push_back('.');
				continue;
			}
			save_token(token, lines);
			// output.append(token);
			token.clear();
			//deal with comment start
			if (is_comment_start(character, character_next))
			{
				if (character_next == '/') break;
				in_comment = true;
				i++;
				continue;
			}
			//deal with operaters
			string s(1, character);
			s.push_back(character_next);
			if (operators.contains(s))
			{
				save_token(s, lines);
				// output.append(s);
				i++;
			}
			else
			{
				string s(1, character);
				save_token(s, lines);
				// output.push_back(character);
			}
		}
		else //reading normal characters and digits
		{
			token.push_back(character);

			if (in_backslash)
			{
				in_backslash = false;
				continue;
			}
			if (character == '"' && ! in_backslash)
			{
				in_quotation = !in_quotation;
			}
			else if (character == '\'')
			{
				in_apostrophe = !in_apostrophe;
			}
			if (character == '\\' && !in_backslash && in_quotation)
			{
				in_backslash = true;
			}
		}

		if(!token.empty() && !in_quotation)
		{
			save_token(token, lines);
			// output.append(token);
			token.clear();
		}
	}


	cout << output << endl;//
}

void lex_file(string file_path)
{
	filename = file_path;
	ifstream file(file_path);
	if (!file.is_open())
	{
		cerr << "Could not open the file - '" << file_path << "'" << endl;
		return;
	}
	char byte;
	vector<char> bytes;
	while (file.get(byte))
	{
		bytes.push_back(byte);
	}
	file.close();

	string text(bytes.begin(), bytes.end());
	lex_text(text);
	// for (const auto &i : bytes) cout << i;
	// cout << endl;
}

string print_file();