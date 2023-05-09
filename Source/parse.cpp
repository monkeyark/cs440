#include <iostream>
#include <vector>
#include <string> 
#include <unordered_map>
#include <unordered_set>
#include "lex.h"

using namespace std;

unordered_set<string> assign_op({
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
});

unordered_set<string> unary_op({
	"-",
	"!",
	"~",
});

unordered_set<string> binary_op({
	"==",
	"!=",
	">",
	">=",
	"<",
	"<=",
	"+",
	"-",
	"*",
	"/",
	"%",
	"|",
	"&",
	"||",
	"&&",
});

unordered_map<int, string> operators ({
	{351, "=="},
	{352, "!="},
	{353, ">="},
	{354, "<="},
	{355, "++"},
	{356, "--"},
	{357, "||"},
	{358, "&&"},
	{361, "+="},
	{362, "-="},
	{363, "*="},
	{364, "/="},
});

unordered_map<int, string> keywords ({
	{401, "const"},
	{402, "struc"},
	{403, "for"},
	{404, "while"},
	{405, "do"},
	{406, "if"},
	{407, "else"},
	{408, "break"},
	{409, "continue"},
	{410, "return"},
	{411, "switch"},
	{412, "case"},
	{413, "default"},
});


unordered_map<int, char> symbols_char ({
{33, '!'},
{37, '%'},
{38, '&'},
{40, '('},
{41, ')'},
{42, '*'},
{43, '+'},
{44, ','},
{45, '-'},
{46, '.'},
{47, '/'},
{58, ':'},
{59, ';'},
{60, '<'},
{61, '='},
{62, '>'},
{63, '?'},
{91, '['},
{93, ']'},
{123, '{'},
{124, '|'},
{125, '}'},
{126, '~'},
});

unordered_map<int, string> types ({
	{TYPE, "void"},
	{TYPE, "int"},
	{TYPE, "double"},
	{TYPE, "float"},
	{TYPE, "char"},
	{TYPE, "byte"},
	{TYPE, "short"},
	{TYPE, "long"},
	{TYPE, "bool"},
});

// unordered_map<string, int> operators_precedence ({
// 	{"(", (},
// 	{")", },
// 	{"[", },
// 	{"]", },
// 	{".", },
// 	{"!", },//right to left
// 	{"~", },//right to left
// 	{"-", },//unary //right to left
// 	{"--", },//right to left
// 	{"++", },//right to left
// 	{"", },//type //right to left
// 	{"*", },
// 	{"/", },
// 	{"%", },
// 	{"+", },
// 	{"-", },
// 	{"<", },
// 	{"<=", },
// 	{">", },
// 	{">=", },
// 	{"==", },
// 	{"!=", },
// 	{"&", },
// 	{"|", },
// 	{"&&", },
// 	{"||", },
// 	{"?", },//right to left
// 	{":", },//right to left
// 	{"=", },
// 	{"+=", },
// 	{"-=", },
// 	{"*=", },
// 	{"/=", },
// 	{",", },
// });


int i = 0;
vector<int> tokens;


unordered_map<int, string> token_names;



void parse_token(vector<int> v)
{
		// add all operator names to token_names
	for (auto& [code, name] : operators) {
			token_names[code] = name;
	}

	// add all keyword names to token_names
	for (auto& [code, name] : keywords) {
			token_names[code] = name;
	}

	// add all symbol names to token_names
	for (auto& [code, symbol] : symbols_char) {
			token_names[code] = string(1, symbol);
	}
	tokens = v;

	while(i < tokens.size())
	{

	}

}

bool is_type(int x){
	return types.find(x) != types.end();
}

void print_error(){
	cout << "error" << endl;
	exit(0);
}

string get_string(int x)
{
	if(token_names.find(x) == token_names.end())
		print_error();
	
	return token_names[x];
}

void parse_global()
{
	if(tokens[i++] != TYPE)
	{
		print_error();
	}

	if(tokens[i++] != IDENT)
	{
		print_error();
	}

	if(get_string(tokens[i]) == ";")
	{
		i++;
		return;
	}

	if(get_string(tokens[i]) == "(")
	{
		i++;
		parse_function();
	}
	// TODO add initialization for variable '=' case 
}

void parse_function()
{
	parse_parameter();

	if(get_string(tokens[i]) == ";")
	{
		i++;
		return;
	}

	if(get_string(tokens[i++]) != "{")print_error();
	parse_code_block();
}

void parse_parameter()
{
	while(get_string(tokens[i]) != ")")
	{
		parse_parameter_variable();
		string temp = get_string(tokens[i]);
		if(temp != "," || temp != ")")print_error();
	}
	i++;
}
void parse_parameter_variable()
{
	if(tokens[i++] != TYPE)print_error;
	if(tokens[i++] != IDENT)print_error;
}


void parse_code_block()
{

	while(get_string(tokens[i]) != "}")
	{
		parse_statement();
	}
	i++;


}

void parse_statement()
{
	string temp = get_string(tokens[i]);
	if(temp == ";")i++;
	else if (temp == "break")
	{
		i++;
		if(get_string(tokens[i]) != ";")print_error();
		i++;
	}
	else if (temp == "continue")
	{
		i++;
		if(get_string(tokens[i]) != ";")print_error();
		i++;
	}
	else if (temp == "if")
	{
		i++;
		if(get_string(tokens[i++]) != "(")print_error();
		expression_check();
		if(get_string(tokens[i++]) != ")")print_error();
		if(get_string(tokens[i]) != "{")
			parse_statement();
		else
			parse_code_block();
	}
	else if (temp == "else")
	{
		// TODO 
	}
	else if (temp == "for")
	{
		i++;
		// check (
		expression_check();
		// check ;
		expression_check();
		// check ;
		expression_check();
		// check )
	}
	else if (temp == "while")
	{
		i++;
		if(get_string(tokens[i++]) != "(")print_error();
		expression_check();
		if(get_string(tokens[i++]) != ")")print_error();
		if(get_string(tokens[i]) != "{")
			parse_statement();
		else
			parse_code_block();
	}
	else if (temp == "do")
	{
		i++;
		parse_code_block();
		// Check while and condition here
	}
	else
	{
		expression_check();
		// check ;
	}

}

// only enter this when you see [
void array_check()
{
	i++; // pass [
	// check if is number
	// check and pass ]
}

void expression_check()
{
	if(tokens[i] == IDENT)
	{
		i++;
		string temp = get_string(tokens[i]);
		if(temp == "(")
		{
			i++;
			parse_function_call();
		}
		else if(temp == "+=")
		{
			i++;
			expression_check();
		}
		else
		{
			print_error();
		}

	}
}

void parse_function_call()
{
	while(get_string(tokens[i]) != ")")
	{
		expression_check();
		// check , or )
	}
}