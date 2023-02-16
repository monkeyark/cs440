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
	{"/=", 364},
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
	{"default", 413},
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
	"bool",
});

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
	'\f',
	'\n',
	'\r',
	'\t',
	'\v',
});

string filename;

int is_escape(char c)
{
	return (c == ' ') || (c == '\a') || (c == '\b') || (c == '\f') ||
			(c == '\n') || (c == '\r') || (c == '\t') || (c == '\v');
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

bool is_identifier(string token)
{
	char c = token[0];
	if (c == '_' ||  isalpha(c))
	{
		for (unsigned i = 1; i < token.length(); i++)
		{
			c = token[i];
			if (c != '_' && !isdigit(c) && !isalpha(c))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool is_char_lit(string token)
{
	return token[0] == '\'' && token[token.length()-1] == '\'';
}

bool is_str_lit(string token)
{
	return token[0] == '"' && token[token.length()-1] == '"';
}

bool is_int_lit(string token)
{
	for (char const &c : token)
	{
		if (isdigit(c) == 0) return false;
	}
	return true;
}

bool is_real_lit(string token)
{
	bool seen_dot = false;
	for (char const &c : token)
	{
		if (isdigit(c) == 0 && c != '.')
		{
			return false;
		}
		else if (c == '.' && seen_dot)
		{
			return false;
		}
		else if (c == '.')
		{
			seen_dot = true;
		}
	}
	return true;
}

int find_tokenid(string token)
{
	if (types.find(token) != types.end())
		return TYPE;
	else if (escape.find(token[0]) != escape.end())
		return token[0];
	else if (symbols.find(token[0]) != symbols.end())
		return token[0];
	else if (keywords.count(token))
		return keywords[token];
	else if (operators.count(token))
		return operators[token];
	else if (is_identifier(token))
		return IDENT;
	else if (is_char_lit(token))
		return CHAR_LIT;
	else if (is_str_lit(token))
		return STR_LIT;
	else if (is_int_lit(token))
		return INT_LIT;
	else if (is_real_lit(token))
		return REAL_LIT;
	else
		return -1;
}

void output_token(string lexeme, int line)
{
	// check if current lexeme is empty string or an esacpe
	if (lexeme.empty() || is_escape(lexeme[0])) return;
	// if (lexeme.empty() || escape.find(lexeme[0]) != escape.end()) return;

	int tokenid = find_tokenid(lexeme);
	if (tokenid < 0)
	{
		cout <<
			"Lexer error " << filename <<
			" Line " << std::right << std::setw(5) << line <<
			" Near Text " << lexeme <<
		endl;
	}
	else
	{
		cout <<
			"File " << filename <<
			" Line " << std::right << std::setw(5) << line <<
			" Token " << std::right << std::setw(5) << tokenid <<
			" Text " << lexeme <<
		endl;
	}

}

void lex_text(string text)
{
	string token;
	string output;
	bool in_c_comment = false;
	bool in_cpp_comment = false;
	bool in_quotation = false;
	bool in_apostrophe = false;
	// bool in_backslash = false;
	int line = 1;
	for (long unsigned int i = 0; i < text.length(); i++)
	{
		char c = text[i];
		char c_next = text[i+1];
		
		//TODO need to deal with EOF next to a token
		if (c_next == EOF)
		{
			output_token(token, line);
			token.clear();
		}

		if (c == '\n') //line counter
		{
			line++;
			//terminate c++ style comments
			if (in_cpp_comment)
			{
				if (is_cpp_comment_end(c, c_next))
				{
					in_cpp_comment = false;
					i++;
				}
				continue;
			}
			continue;
		}

		//terminate c style comments
		if (in_c_comment)
		{
			if (is_c_comment_end(c, c_next))
			{
				in_c_comment = false;
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

		if (in_quotation)
		{
			if (c == '"') in_quotation = false;
			token.push_back(c);
			continue;
		}

		if (in_apostrophe)
		{
			if (c == '\'') in_apostrophe = false;
			token.push_back(c);
			continue;
		}

		// check for text string 
		if (c == '"')
		{
			in_quotation = true;
		}
		if (c == '\'')
		{
			in_apostrophe = true;
		}

		// skip the backslash in the end of line
		if (c == '\\' && c_next == '\n') continue;



		// comments and string have higher priority than other chars

		// after high priority text process
		

		//current char is a symbol
		if (symbols.count(c) && !in_quotation && !in_apostrophe)
		{
			if (c == '.' && is_int_lit(token)) //find an real number
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
				token.push_back(c); //TODO this line push newline after meet a symbol
			}
		}
		else if (!is_escape(c))//reading normal characters and digits
		{
			token.push_back(c);
		}

		// check symbols again to avoid read newline
		if ((is_escape(c) || symbols.count(c)) && !in_quotation && !in_apostrophe)
		{
			output_token(token, line);
			token.clear();
		}
	}
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
}

string print_file();
