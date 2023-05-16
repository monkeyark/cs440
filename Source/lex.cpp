#include "lex.hpp"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>
#include <regex>
#include <sstream>

using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using std::unordered_set;
using std::unordered_map;
using std::regex;
using std::string;

std::ostringstream oss;

vector<int> lines_lex;
vector<int> token_ids;
vector<string> lexemes;

vector<int> get_lines()
{
	return lines_lex;
}

vector<int> get_tokens()
{
	return token_ids;
}

vector<string> get_words()
{
	return lexemes;
}

enum token_msg {
/* Message */
	LEX_SUCC                    = 1,
	FILE_NOT_OPEN               = -100,
	TOKEN_SUCC                  = 1,
	TOKEN_ERR                   = 0,
	TOKEN_UNRECOGNIZED          = -1,
	TOKEN_ILLEGAL_CHARACTER     = -3,
	TOKEN_SIZE_EXCEEDED_REAL    = -21,
	TOKEN_SIZE_EXCEEDED_CHAR    = -22,
	TOKEN_SIZE_EXCEEDED_STR     = -23,
	TOKEN_SIZE_EXCEEDED_INT     = -24,
	TOKEN_SIZE_EXCEEDED_IDENT   = -25,
	TOKEN_UNCLOSED_COMMENT      = -31,
	TOKEN_UNCLOSED_QUOTE        = -32,
	TOKEN_UNCLOSED_CHARSEQ      = -33,
};

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
	{"const",     401},
	{"struc",     402},
	{"for",       403},
	{"while",     404},
	{"do",        405},
	{"if",        406},
	{"else",      407},
	{"break",     408},
	{"continue",  409},
	{"return",    410},
	{"switch",    411},
	{"case",      412},
	{"default",   413},
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
		return TOKEN_SIZE_EXCEEDED_IDENT;
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
		return TOKEN_SIZE_EXCEEDED_INT;
	}
	return INT_LIT;
}

int is_real_lit(string token)
{
	regex real("^[+-]?(\\d+\\.?\\d*|\\.\\d+)([eE][+-]?\\d+)?$");
	if (regex_match(token, real))
	{
		return REAL_LIT;
	}
	return TOKEN_ERR;
}

//TODO need to fix for ident: e1e2ee
int is_real_num(string token)
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
	if (token.length() == 1 && !isdigit(token[0]))
	{
		return TOKEN_ERR;
	}
	if (token.length() > 48)
	{
		return TOKEN_SIZE_EXCEEDED_REAL;
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
			return TOKEN_SIZE_EXCEEDED_CHAR;
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
		return TOKEN_SIZE_EXCEEDED_STR;
	}
	return STR_LIT;
}

int is_unary_increment(string token)
{
	// Check if the string is empty or has only two characters
	if (token.empty() || token.size() < 2) {
			return TOKEN_ERR;
	}
	
	// Check if the last two characters are "++"
	if (token.substr(token.size() - 2) != "++") {
			return TOKEN_ERR;
	}
	// Check if the rest of the string is a legal C identifier
	for (size_t i = 0; i < token.size() - 2; ++i) {
			if (i == 0 && !isalpha(token[i]) && token[i] != '_') {
					return TOKEN_ERR;
			}
			if (!isalnum(token[i]) && token[i] != '_') {
					return TOKEN_ERR;
			}
	}
	return IDENT;
}

int is_unary_decrement(string token)
{
	// Check if the string is empty or has only two characters
	if (token.empty() || token.size() < 2) {
			return TOKEN_ERR;
	}
	
	// Check if the last two characters are "++"
	if (token.substr(token.size() - 2) != "--") {
			return TOKEN_ERR;
	}
	// Check if the rest of the string is a legal C identifier
	for (size_t i = 0; i < token.size() - 2; ++i) {
			if (i == 0 && !isalpha(token[i]) && token[i] != '_') {
					return TOKEN_ERR;
			}
			if (!isalnum(token[i]) && token[i] != '_') {
					return TOKEN_ERR;
			}
	}
	return IDENT;
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
	is_unary_increment,
	is_unary_decrement,
};

int find_tokenid(string token)
{
	int tokenid = TOKEN_ERR;
	int i = 0;
	do {
		tokenid = token_search[i](token);
		i++;
	} while (tokenid && i < 10);
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
	else if (is_unary_increment(token))
		return is_unary_increment(token);
	else if (is_unary_decrement(token))
		return is_unary_decrement(token);
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

void output_token_err(string lexeme, int line, string fname, int tokenid)
{
	std::cerr
	<< "Lexer error in file " << fname
	<< " line " << line
	<< " at text " << lexeme.substr(0, 20)
	<< endl << "	";
	// cerr << " Lexer Error Text " << lexeme.substr(0, 20) << endl;
	
	string msg;
	switch (tokenid)
	{
		case TOKEN_UNRECOGNIZED:
			msg = "Token unrecognized";
			break;
		case TOKEN_SIZE_EXCEEDED_REAL:
			msg = "Real number size exceeded";
			break;
		case TOKEN_SIZE_EXCEEDED_CHAR:
			msg = "Char literal is too long";
			break;
		case TOKEN_SIZE_EXCEEDED_STR:
			msg = "String is too long";
			break;
		case TOKEN_SIZE_EXCEEDED_INT:
			msg = "Integer is too long";
			break;
		case TOKEN_SIZE_EXCEEDED_IDENT:
			msg = "Identifier is too long";
			break;
		case TOKEN_ILLEGAL_CHARACTER:
			msg = "Illegal character";
			break;
		case TOKEN_UNCLOSED_COMMENT:
			msg = "Unclosed comment";
			break;
		case TOKEN_UNCLOSED_QUOTE:
			msg = "Unclosed quote";
			break;
		case TOKEN_UNCLOSED_CHARSEQ:
			msg = "Unclose character sequence";
			break;
		case FILE_NOT_OPEN:
			msg = get_str_between_two_str(lexeme, "\"", "\"");
			msg = "Couldn't open file " + msg;
			break;
		default:
			return;
	}
	std::cerr << "\033[1;31m" << msg << "\033[0m" << endl;
	exit(1);
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
		if (lex_file(dir+filename, false) == FILE_NOT_OPEN)
		{
			output_token_err(lexeme, line, fname, FILE_NOT_OPEN);
			return;
		}
	}

	int tokenid = search_tokenid(lexeme);
	if (tokenid < TOKEN_ERR)
	{
		output_token_err(lexeme, line, fname, tokenid);
	}
	else
	{
		oss
		<< "File " << std::left << std::setw(9) << fname
		<< " Line " << std::right << std::setw(5) << line
		<< " Token " << std::right << std::setw(5) << tokenid
		<< " Text " << lexeme << endl;
		token_ids.push_back(tokenid);
		lines_lex.push_back(line);
		lexemes.push_back(lexeme);
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
	int token_line = line;
	for (long long unsigned int i = 0; i < text.length(); i++)
	{
		char c = text[i];char c_next = text[i+1];

		//deal with unclosed comment
		if (i == text.length()-1)
		{
			if (in_c_comment)
			{
				output_token_err("/*", token_line, fname, TOKEN_UNCLOSED_COMMENT);
				continue;
			}
			if (in_quotation)
			{
				output_token_err("\"", line, fname, TOKEN_UNCLOSED_QUOTE);
				continue;
			}
			if (in_apostrophe)
			{
				output_token_err("\'", line, fname, TOKEN_UNCLOSED_CHARSEQ);
				continue;
			}
		}

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
			if (in_quotation)
			{
				in_quotation = false;
				output_token_err("\"", line, fname, TOKEN_UNCLOSED_QUOTE);
				continue;
			}
			if (in_apostrophe)
			{
				in_apostrophe = false;
				output_token_err("\'", line, fname, TOKEN_UNCLOSED_CHARSEQ);
				continue;
			}
			in_hash = false;
			in_real = false;
			output_token(token, line, fname);
			token_line = line;
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
				output_token(token, line, fname);
				token_line = line;
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
				token_line = line;
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
				token_line = line;
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
			token_line = line;
			token.clear();
			token.push_back(c);
			continue;
		}
		if (c == '\'')
		{
			in_apostrophe = true;
			output_token(token, line, fname);
			token_line = line;
			token.clear();
			token.push_back(c);
			continue;
		}
		if (c == '#')
		{
			in_hash = true;
			output_token(token, line, fname);
			token_line = line;
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
			token_line = line;
			token.clear();
		}
		else if (!is_legal_character(c))
		{
			token_line = line;
			output_token(token, line, fname);
			token.clear();
			string token(1, c);
			token_line = line;
			output_token_err(token, line, fname, TOKEN_ILLEGAL_CHARACTER);
			token.clear();
		}
		else if (is_symbols(c)) //current is symbol
		{
			// check for real number
			if (in_real)
			{
				if (c == '-' || c == '+')
				{
					token.push_back(c);
					i++;
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
				token_line = line;
				output_token(token, line, fname);
				token.clear();
			}
			else //output previous token when meet a symbol
			{
				token_line = line;
				output_token(token, line, fname);
				token.clear();
				token.push_back(c);
			}
		}
		//reading normal characters and digits
		else if (isdigit(c) || isalpha(c) || c == '_')
		{
			// check for real number
			if (in_real)
			{
				if (isdigit(c))
				{
					token.push_back(c);
					continue;
				}
				else if (c != 'e' && c != 'E')
				{
					in_real = false;
					token_line = line;
					output_token(token, line, fname);
					token.clear();
					token.push_back(c);
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
			if ((c == 'e' || c == 'E') && !token.empty())
			{
				if ((is_int_lit(token) == INT_LIT || is_real_lit(token) == REAL_LIT))
				{
					in_real = true;
					token.push_back(c);
					if (c_next == '+' || c_next == '-')
					{
						token.push_back(c_next);
						i++;
					}
					continue;
				}
			}

			//if the previous char is a symbol and not in real number checking
			char c_prev = token[token.length()-1];
			if (is_symbols(c_prev))
			{
				if ((c_prev != '.') || c_prev != '-'|| c_prev != '+' ||
				(c_prev == '.' && !in_real))
				{
					token_line = line;
					output_token(token, line, fname);
					token.clear();
				}
			}
			token.push_back(c);
		}
		else //all other cases append current char to token
		{
			token.push_back(c);
		}

		// output token when reach EOF
		if (i == text.length()-1)
		{
			if (in_c_comment || in_cpp_comment)
			{
				output_token_err(token, token_line, fname, TOKEN_UNCLOSED_COMMENT);
			}
			else if (in_quotation || in_apostrophe)
			{
				output_token_err(token, token_line, fname, TOKEN_UNCLOSED_QUOTE);
			}
			else
			{
				token_line = line;
				output_token(token, line, fname);
			}
			token.clear();
		}
	}
}

int lex_file(string path, bool is_output)
{
	ifstream file(path);
	if (!file.is_open())
	{
		return FILE_NOT_OPEN;
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

	// Extract filename from the filepath
	std::string fname = path.substr(path.find_last_of("/") + 1);
	size_t dot_pos = fname.find_last_of(".");
	if (dot_pos != std::string::npos)
	{
		fname = fname.substr(0, dot_pos);
	}

	if (is_output)
	{
		// Write std::ostringstream to the file
		std::ofstream outfile(fname+".lexer");
		outfile << oss.str();
		outfile.close();
	}

	return LEX_SUCC;
}

string print_file()
{
	std::string lexer_str(oss.str());
	std::cout << lexer_str;
	return lexer_str;
}
