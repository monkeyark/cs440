#include "lex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

using std::cout;
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

unordered_map<char, int> special_c_symbols ({
	{'#',35},
	{'\\',92},
	{'^',94},
	{'_',95},
});

// unordered_map<char, int> symbols_char ({
// 	{'!',33},
// 	{'%',37},
// 	{'&',38},
// 	{'(',40},
// 	{')',41},
// 	{'*',42},
// 	{'+',43},
// 	{',',44},
// 	{'-',45},
// 	{'.',46},
// 	{'/',47},
// 	{':',58},
// 	{';',59},
// 	{'<',60},
// 	{'=',61},
// 	{'>',62},
// 	{'?',63},
// 	{'[',91},
// 	{']',93},
// 	{'{',123},
// 	{'|',124},
// 	{'}',125},
// 	{'~',126},
// });

// unordered_map<string, int> symbols_str ({
// 	{"!",33},
// 	{"%",37},
// 	{"&",38},
// 	{"(",40},
// 	{")",41},
// 	{"*",42},
// 	{"+",43},
// 	{",",44},
// 	{"-",45},
// 	{".",46},
// 	{"/",47},
// 	{":",58},
// 	{";",59},
// 	{"<",60},
// 	{"=",61},
// 	{">",62},
// 	{"?",63},
// 	{"[",91},
// 	{"]",93},
// 	{"{",123},
// 	{"|",124},
// 	{"}",125},
// 	{"~",126},
// });

unordered_set<char> escapes ({
	' ',
	'\a',
	'\b',
	'\n',
	'\r',
	'\t',
	'\'',
	'\\'
});

// unordered_map<char, int> escapes_char ({
// 	{'\a',7},
// 	{'\b',8},
// 	{'\t',9},
// 	{'\n',10},
// 	{'\v',11},
// 	{'\f',12},
// 	{'\r',13},
// 	{' ', 32},
// });


int is_escape(char c)
{
	return escapes.count(c);
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

int is_symbol(string token)
{
	if (token.length() == 1)
	{
		char c = token[0];
		if (symbols.count(c))
			return c;
	}
	return TOKEN_ERR;
}

int is_symbols(char c)
{
	if (symbols.count(c))
	{
		return c;
	}
	return 0;
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

int is_legal_character(char c)
{
	return isdigit(c) || isalpha(c) || is_symbols(c) || special_c_symbols.count(c);
}

int is_legal_character(string token)
{
	char c = token[0];
	if (token.length() == 1)
	{
		if (is_legal_character(c))
		{
			return TOKEN_SUCC;
		}
		return TOKEN_ILLEGAL_CHARACTER;
	}
	return TOKEN_ERR;
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
	if (token.empty())
	{
		return TOKEN_ERR;
	}
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

typedef int (*token_function) (string token);
token_function token_search[] =
{
	is_type,
	is_symbol,
	is_keyword,
	is_operator,
	is_identifier,
	is_char_lit,
	is_str_lit,
	is_int_lit,
	is_real_lit,
	is_legal_character,
};

int find_tokenid(string token)
{
	int tokenid = TOKEN_ERR;
	int i = 0;
	do {
		tokenid = token_search[i](token);
		i++;
	} while (tokenid && i < 10);

	// switch (tokenid)
	// {
	// 	case TYPE:
	// 		return TYPE;
	// 		break;
	// 	case END:
	// 		return END;
	// 		break;
	// 	default:
	// 		return -1;
	// }

	return tokenid;
}

int search_tokenid(string token)
{
	if (is_type(token))
		return is_type(token);
	else if (is_symbol(token))
		return is_symbol(token);
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

std::string get_str_between_two_str(const std::string &s,
        const std::string &start_delim,
        const std::string &stop_delim)
{
    unsigned first_delim_pos = s.find(start_delim);
    unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
    unsigned last_delim_pos = s.find_first_of(stop_delim, end_pos_of_first_delim);
    
    return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
}

std::string get_str_before_last_delim_inclusive(std::string const& s, const std::string &delim)
{
	std::string::size_type pos = s.find_last_of(delim);
	if (pos != std::string::npos)
	{
		return s.substr(0, pos+1);
	}
	else
	{
		return "";
	}
}

void output_token(string lexeme, int line, string fname, int tokenid)
{
	//TODO unclosed comment
	if (lexeme.empty()) return;
	if (tokenid < TOKEN_ERR)
	{
		cout
		<< "Lexer error in file " << std::right << std::setw(9) << fname
		<< " Line " << std::right << std::setw(5) << line
		<< " Near Text " << lexeme << endl << "	";
		
		if (tokenid == TOKEN_UNCLOSED_COMMENT)
		{
			cout << "Unclosed comment" << endl;
		}
		if (tokenid == TOKEN_UNCLOSED_QUOTE)
		{
			cout << "Unclosed quotation" << endl;
		}
	}
}

void output_token(string lexeme, int line, string path)
{
	string fname = path.substr(path.find_last_of("/\\") + 1);
	// check if current lexeme is empty
	if (lexeme.empty()) return;
	if (lexeme.find("#include") != string::npos)
	{
		string dir = get_str_before_last_delim_inclusive(path, "/");
		string filename = get_str_between_two_str(lexeme, "\"", "\"");
		lex_file(dir+filename);
		return;
	}

	cout
	<< "File " << std::left << std::setw(9) << fname
	<< " Line " << std::right << std::setw(5) << line;

	int tokenid = search_tokenid(lexeme);
	if (tokenid < TOKEN_ERR)
	{
		cout << " Lexer Error Text " << lexeme.substr(0, 20) << endl;
		
		string message = "                         ";
		if (tokenid == TOKEN_SIZE_EXCEEDED)
		{
			message += "Size Exceeded";
		}
		else if (tokenid == TOKEN_UNRECOGNIZED)
		{
			message += "Token Unrecognized";
		}
		else if (tokenid == TOKEN_ILLEGAL_CHARACTER)
		{
			message += "Illegal Character";
		}
		cout << "\033[1;31m" << message << "\033[0m" << endl;
	}
	else
	{
		cout
		<< " Token " << std::right << std::setw(5) << tokenid
		<< " Text " << lexeme << endl;
	}
}

void lex_text(string text, string fname)
{
	string token;
	bool in_c_comment = false;
	bool in_cpp_comment = false;
	bool in_quotation = false;
	bool in_quotation_esc = false;
	bool in_apostrophe = false;
	bool in_apostrophe_esc = false;
	bool in_real = false;
	bool in_hash = false;
	int line = 1;
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
					output_token(token, line, fname);
					token.clear();
					line++;
					continue;
				}
			}
			in_hash = false;
			in_real = false;
			output_token(token, line, fname);
			token.clear();
			line++;
			continue;
		}

		//terminate c style comments
		if (in_c_comment && !in_quotation && !in_apostrophe)
		{
			if (i == text.length()-1)
			{
				//TODO deal with unclosed comment
				output_token("/*", line, fname, TOKEN_UNCLOSED_COMMENT);
			}

			if (is_c_comment_end(c, c_next))
			{
				in_c_comment = false;
				i++;
				output_token(token, line, fname);
				token.clear();
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
			i++;
			continue;
		}
		else if (is_cpp_comment_start(c, c_next) && !in_quotation && !in_apostrophe)
		{
			in_cpp_comment = true;
			i++;
			continue;
		}

		if (in_quotation && !in_c_comment && !in_cpp_comment && !in_hash)
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
				token.push_back(c);
				output_token(token, line, fname);
				token.clear();
				continue;
			}
			token.push_back(c);
			continue;
		}


		if (in_apostrophe && !in_c_comment && !in_cpp_comment &!in_hash)
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
			if (c == '\'')
			{
				in_apostrophe = false;
				token.push_back(c);
				output_token(token, line, fname);
				token.clear();
				continue;
			}
			token.push_back(c);
			continue;
		}
		if (in_hash && !in_quotation && !in_apostrophe)
		{
			token.push_back(c);
			continue;
		}

		// check for text string
		if (c == '"')
		{
			in_quotation = true;
			output_token(token, line, fname);
			token.clear();
			token.push_back(c);
			continue;
		}
		if (c == '\'')
		{
			in_apostrophe = true;
			output_token(token, line, fname);
			token.clear();
			token.push_back(c);
			continue;
		}
		if (c == '#')
		{
			in_hash = true;
			output_token(token, line, fname);
			token.clear();
			token.push_back(c);
			continue;
		}

		// skip the backslash in the end of line
		if (c == '\\' && c_next == '\n') continue;


		// comments and string have higher priority than other chars
		// after high priority text process
		

		// check if need to output buffered token
		if (is_escape(c)) //current is escape
		{
			output_token(token, line, fname);
			token.clear();
		}
		else if (!is_legal_character(c))
		{
			output_token(token, line, fname);
			token.clear();
			string token(1, c);
			output_token(token, line, fname);
			token.clear();
		}
		else if (is_symbols(c)) //current is symbol
		{
			// check for real number
			if (in_real)
			{
				if (!isdigit(c) && c != 'e' && c != 'E')
				{
					in_real = false;
					output_token(token, line, fname);
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

			//check for current and next char for operators
			string s(1, c);
			s.push_back(c_next);
			if (operators.count(s))
			{
				token.push_back(c);
				token.push_back(c_next);
				i++;
				output_token(token, line, fname);
				token.clear();
			}
			else //output previous token when meet a symbol
			{
				output_token(token, line, fname);
				token.clear();
				token.push_back(c);
			}
		}
		else if (isdigit(c) || isalpha(c) || c == '_') //reading normal characters and digits
		{
			// check for real number
			if (in_real)
			{
				if (!isdigit(c) && c != 'e' && c != 'E')
				{
					in_real = false;
					output_token(token, line, fname);
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
		else
		{
			token.push_back(c);
		}

		// output token when reach EOF
		if (i == text.length()-1)
		{
			if (in_c_comment || in_cpp_comment)
			{
				output_token(token, line, fname, TOKEN_UNCLOSED_COMMENT);
			}
			else if (in_quotation || in_apostrophe)
			{
				output_token(token, line, fname, TOKEN_UNCLOSED_QUOTE);
			}
			else
			{
				output_token(token, line, fname);
			}
			token.clear();
		}
	}
}

void lex_file(string path)
{
	ifstream file(path);
	if (!file.is_open())
	{
		cout << "Could not open the file - '" << path << "'" << endl;
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
	lex_text(text, path);
}

string print_file();
