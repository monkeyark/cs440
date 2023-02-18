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

unordered_set<char> c_symbols ({

	'!',//33
	'#',//35
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
	'\\',//92
	']',//93
	'^',//94
	'_',//95
	'{',//123
	'|',//124
	'}',//125
	'~',//126
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

int is_escape(string token)
{
	if (token.length() == 1)
	{
		char c = token[0];
		if (escape.count(c)) return c;
	}
	return 0;
}

int is_type(string token)
{
	if (types.find(token) != types.end())
		return TYPE;
	return TOKEN_ERR;
}

int is_keyword(string token)
{
	if (keywords.count(token))
		return keywords[token];
	return TOKEN_ERR;
}

int is_operator(string token)
{
	if (operators.count(token))
		return operators[token];
	return TOKEN_ERR;
}

int is_symbols(string token)
{
	if (token.length() == 1)
	{
		char c = token[0];
		if (symbols.count(c)) return c;
	}
	return TOKEN_ERR;
}

int is_symbol(char c)
{
	return symbols.count(c);
}

int is_legal_symbol(char c)
{
	return c_symbols.count(c) || isdigit(c) || isalpha(c);
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
	return (a == '\n') && (b != '/');
}

int is_legal_character(string token)
{
	if (token.length() == 1 && !is_legal_symbol(token[0]))
	{
		return TOKEN_ILLEGAL_CHARACTER;
	}
	return TOKEN_SUCC;
}

int is_identifier(string token)
{
	char c = token[0];
	if (c != '_' &&  !isalpha(c))
	{
		return TOKEN_ERR; // invaild char at first byte
	}
	for (unsigned i = 1; i < token.length(); i++)
	{
		c = token[i];
		if (c != '_' && !isdigit(c) && !isalpha(c))
		{
			return TOKEN_ERR; // invaild char after first byte
		}
	}
	if (token.length() > 48)
	{
		return TOKEN_SIZE_EXCEEDED;
	}
	return IDENT;
}

int is_int_lit(string token)
{
	for (char const &c : token)
	{
		if (!isdigit(c))
			return TOKEN_ERR;
	}
	if (token.length() > 48)
	{
		return TOKEN_SIZE_EXCEEDED;
	}
	return INT_LIT;
}

int is_real_lit(string token)
{
	bool seen_dot = false;
	bool seen_exp = false;
	for (char const &c : token)
	{
		if (!isdigit(c) && c != '.' && c != 'e' &&c != 'E' && c != '+' && c != '-')
		{
			return TOKEN_ERR;
		}
		else if (c == '.' && seen_dot)
		{
			return TOKEN_ERR;
		}
		else if ((c == 'e' || c == 'E') && seen_exp)
		{
			return TOKEN_ERR;
		}
		if (c == '.')
		{
			seen_dot = true;
		}
		if (c == 'e' || c == 'E')
		{
			seen_exp = true;
		}
	}
	if (token.length() > 48)
	{
		return TOKEN_SIZE_EXCEEDED;
	}
	return REAL_LIT;
}

int is_char_lit(string token)
{
	char c = token[0];
	char c_ = token[token.length()-1];
	if (c == '\'' && c_ == '\'')
	{
		if (token.length() > 1024)
		{
			return TOKEN_SIZE_EXCEEDED;
		}
		return CHAR_LIT;
	}
	return TOKEN_ERR;
}

int is_str_lit(string token)
{
	char c = token[0];
	char c_ = token[token.length()-1];
	if (c != '"' && c_ != '"')
	{
		return TOKEN_ERR;
	}
	if (token.length() > 1024)
	{
		return TOKEN_SIZE_EXCEEDED;
	}
	return STR_LIT;
}

int check_token(string token)
{
	return 1;
}

int search_tokenid(string token)
{
	// int id = is_identifier(token);
	// switch (check_token(token))
	// {
	// 	case TYPE:
	// 		return TYPE;
	// 		break;
	// 	default:
	// 		return -1;
	// }

	// if (is_type(token))
	// 	return TYPE;
	// else if (is_symbols(token))
	// 	return is_symbols(token);
	// else if (is_keyword(token))
	// 	return is_keyword(token);
	// else if (is_operator(token))
	// 	return is_operator(token);
	// else if (is_identifier(token))
	// 	return IDENT;
	// else if (is_char_lit(token))
	// 	return CHAR_LIT;
	// else if (is_str_lit(token))
	// 	return STR_LIT;
	// else if (is_int_lit(token))
	// 	return INT_LIT;
	// else if (is_real_lit(token))
	// 	return REAL_LIT;
	// else
	// 	return -1;

	if (is_type(token))
		return is_type(token);
	else if (is_symbols(token))
		return is_symbols(token);
	else if (is_keyword(token))
		return is_keyword(token);
	else if (is_operator(token))
		return is_operator(token);
	else if (is_identifier(token))
		return is_identifier(token);
	else if (is_char_lit(token))
		return is_char_lit(token);
	else if (is_str_lit(token))
		return is_str_lit(token);
	else if (is_int_lit(token))
		return is_int_lit(token);
	else if (is_real_lit(token))
		return is_real_lit(token);
	else if (is_legal_character(token))
		return is_legal_character(token);
	else
		return TOKEN_UNRECOGNIZED;
}

// void output_token(string lexeme, int line, int tokenid)
void output_token(string lexeme, int line)
{
	// check if current lexeme is empty string or an esacpe
	if (lexeme.empty() || is_escape(lexeme)) return;
	int tokenid = search_tokenid(lexeme);
	if (tokenid < TOKEN_ERR)
	{
		cout
			<< "Lexer error in file "
			<< filename
			<< " Line "
			<< std::right
			<< std::setw(5)
			<< line
			<< " Near Text "
			<< lexeme
			<< endl;
		if (tokenid == TOKEN_UNRECOGNIZED)
		{
			cout << "	Token unrecognized" << endl;
		}
		if (tokenid == TOKEN_SIZE_EXCEEDED)
		{
			cout << "	Size exceeded" << endl;
		}
		if (tokenid == TOKEN_ILLEGAL_CHARACTER)
		{
			cout << "	Illegal character" << endl;
		}
	}
	else
	{
		cout
			<< "File "
			<< filename
			<< " Line "
			<< std::right
			<< std::setw(5)
			<< line
			<< " Token "
			<< std::right
			<< std::setw(5)
			<< tokenid
			<< " Text "
			<< lexeme
			<< endl;
	}
}

void lex_text(string text)
{
	string token;
	bool in_c_comment = false;
	bool in_cpp_comment = false;
	bool in_quotation = false;
	bool in_quotation_esc = false;
	bool in_apostrophe = false;
	bool in_apostrophe_esc = false;
	bool in_real = false;
	int line = 1;
	// int tokenid = TOKEN_UNFOUND;
	for (long long unsigned int i = 0; i < text.length(); i++)
	{
		char c = text[i];char c_next = text[i+1];

		if (c == '\n') //line counter
		{
			//terminate c++ style comments
			if (in_cpp_comment)
			{
				if (is_cpp_comment_end(c, c_next))
				{
					in_cpp_comment = false;
					line++;
					continue;
				}
			}
			in_real = false;
			output_token(token, line);
			// tokenid = TOKEN_UNFOUND;
			token.clear();

			line++;
			continue;
		}

		//terminate c style comments
		if (in_c_comment && !in_quotation && !in_apostrophe)
		{
			if (is_c_comment_end(c, c_next))
			{
				in_c_comment = false;
				i++;
			}
			continue;
		}
		if (in_cpp_comment && !in_quotation && !in_apostrophe)
		{
			continue;
		}

		if (is_c_comment_start(c, c_next) && !in_quotation && !in_apostrophe)
		{
			in_c_comment = true;
			continue;
		}
		else if (is_cpp_comment_start(c, c_next) && !in_quotation && !in_apostrophe)
		{
			in_cpp_comment = true;
			i++;
			continue;
		}

		if (in_quotation && !in_c_comment && !in_cpp_comment)
		{
			if (c == '\\' && c_next == '\\')
			{
				token.push_back(c);
				token.push_back(c_next);
				i++;
				continue;
			}
			if (in_quotation_esc)
			{
				if (c == '\\' && c_next == '"')
				{
					in_quotation_esc = false;
				}
			}
			if (c == '\\' && c_next == '"')
			{
				in_quotation_esc = true;
				token.push_back(c);
				token.push_back(c_next);
				i++;
				continue;
			}

			if (c == '"')
			{
				in_quotation = false;
			}
			token.push_back(c);
			continue;
		}

		if (in_apostrophe && !in_c_comment && !in_cpp_comment)
		{
			if (c == '\\' && c_next == '\\')
			{
				token.push_back(c);
				token.push_back(c_next);
				i++;
				continue;
			}
			if (in_apostrophe_esc)
			{
				if (c == '\\' && c_next == '\'')
				{
					in_apostrophe_esc = false;
				}
			}
			if (c == '\\' && c_next == '\'')
			{
				in_apostrophe_esc = true;
				token.push_back(c);
				token.push_back(c_next);
				i++;
				continue;
			}
			if (c == '\'') in_apostrophe = false;
			token.push_back(c);
			continue;
		}

		// check for text string
		if (c == '"')
		{
			in_quotation = true;
			token.push_back(c);
			continue;
		}
		if (c == '\'')
		{
			in_apostrophe = true;
			token.push_back(c);
			continue;
		}

		// skip the backslash in the end of line
		if (c == '\\' && c_next == '\n') continue;


		// comments and string have higher priority than other chars
		// after high priority text process
		

		// check if need to output buffered token
		if (!is_legal_symbol(c))
		{
			output_token(token, line);
			token.clear();
			string s(1, c);
			output_token(s, line);
		}
		else if (escape.count(c)) //current is escape
		{
			output_token(token, line);
			token.clear();
		}
		else if (symbols.count(c)) //current is symbol
		{
			// check for real number
			if (in_real)
			{
				if (!isdigit(c) && c != 'e' && c != 'E')
				{
					in_real = false;
					output_token(token, line);
					token.clear();
					continue;
				}
				else if (c == 'e' || c == 'E')
				{
					token.push_back(c);
					if (c_next == '+' || c_next == '-')
					{
						token.push_back(c_next);
						i++;
					}
					continue;
				}
			}

			//find an real number with dot
			if (c == '.' && is_int_lit(token) == INT_LIT)
			{
				in_real = true;
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
			// check for real number
			if (in_real)
			{
				if (!isdigit(c) && c != 'e' && c != 'E')
				{
					in_real = false;
					output_token(token, line);
					token.clear();
					continue;
				}
				else if (c == 'e' || c == 'E')
				{
					token.push_back(c);
					if (c_next == '+' || c_next == '-')
					{
						token.push_back(c_next);
						i++;
					}
					continue;
				}
			}
			//find an real number without dot
			if ((c == 'e' || c == 'E') && is_int_lit(token) == INT_LIT) //find an real number
			{
				if (c)
				// cout << "real after EEEEEEEEEE" <<endl;
				in_real = true;
				token.push_back(c);
				if (c_next == '+' || c_next == '-')
				{
					token.push_back(c_next);
					i++;
				}
				continue;
			}
			token.push_back(c);
		}

		// output token when reach EOF
		if (i == text.length()-1)
		{
			output_token(token, line);
			token.clear();
		}
	}
}

void lex_file(string path)
{

	ifstream file(path);
	filename = path.substr(path.find_last_of("/\\") + 1);
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
