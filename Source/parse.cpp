#include <iostream>
#include <vector>
#include <string> 
#include <unordered_map>
#include <unordered_set>
#include "lex.hpp"

using std::unordered_set;
using std::unordered_map;
using std::vector;
using std::cout;
using std::endl;
using std::cerr;


string opt[] = {
	",",
	"/=",
	"*=",
	"-=",
	"+=",
	"=",
	":",
	"?",
	"||",
	"&&",
	"|",
	"&",
	"==",
	"!=",
	"<",
	"<=",
	">",
	">=",
	"+",
	"-",
	"*",
	"/",
	"%",
	"TYPE",
	"++",
	"--",
	"UNARY",
	"~",
	"!",
	"(",
	")",
	"[",
	"]",
	"."
};

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
vector<int> toks;


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
	toks = v;

	while(i < toks.size())
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
	if(toks[i++] != TYPE)
	{
		print_error();
	}

	if(toks[i++] != IDENT)
	{
		print_error();
	}

	if(get_string(toks[i]) == ";")
	{
		i++;
		return;
	}

	if(get_string(toks[i]) == "(")
	{
		i++;
		parse_function();
	}
	// TODO add initialization for variable '=' case 
}

void parse_function()
{
	parse_parameter();

	if(get_string(toks[i]) == ";")
	{
		i++;
		return;
	}

	if(get_string(toks[i++]) != "{")print_error();
	parse_code_block();
}

void parse_parameter()
{
	while(get_string(toks[i]) != ")")
	{
		parse_parameter_variable();
		string tok = get_string(toks[i]);
		if(tok != "," || tok != ")")print_error();
	}
	i++;
}
void parse_parameter_variable()
{
	if(toks[i++] != TYPE)print_error;
	if(toks[i++] != IDENT)print_error;
}


void parse_code_block()
{

	while(get_string(toks[i]) != "}")
	{
		parse_statement();
	}
	i++;
}

void parse_statement()
{
	string tok = get_string(toks[i]);
	if(tok == ";")i++;
	else if (tok == "break")
	{
		i++;
		if(get_string(toks[i]) != ";")print_error();
		i++;
	}
	else if (tok == "continue")
	{
		i++;
		if(get_string(toks[i]) != ";")print_error();
		i++;
	}
	else if (tok == "if")
	{
		i++;
		if(get_string(toks[i++]) != "(")print_error();
		parse_expression();
		if(get_string(toks[i++]) != ")")print_error();
		if(get_string(toks[i]) != "{")
			parse_statement();
		else
			parse_code_block();
	}
	else if (tok == "else")
	{
		// TODO
		i++;
		if(get_string(toks[i]) == "if") //else if
		{
			i++;
			if(get_string(toks[i++]) != "(")print_error();
			parse_expression();
			if(get_string(toks[i++]) != ")")print_error();
			if(get_string(toks[i]) != "{")
				parse_statement();
			else
				parse_code_block();
		}
		else //single else
		{
			if(get_string(toks[i++]) != "{")
				parse_statement();
			else
				parse_code_block();
		}
	}
	else if (tok == "for")
	{
		// TODO
		i++;
		// check (
		if (get_string(toks[i++]) != "(")print_error();
		parse_expression();
		// check ;
		if (get_string(toks[i++]) != ";")print_error();
		parse_expression();
		// check ;
		if (get_string(toks[i++]) != ";")print_error();
		parse_expression();
		// check )
		if (get_string(toks[i++]) != ")")print_error();
	}
	else if (tok == "while")
	{
		i++;
		if(get_string(toks[i++]) != "(")print_error();
		parse_expression();
		if(get_string(toks[i++]) != ")")print_error();
		if(get_string(toks[i++]) != "{")
			parse_statement();
		else
			parse_code_block();
	}
	else if (tok == "do")
	{
		// check while and condition here
		//TODO
		i++;
		if(get_string(toks[i++]) != "{")print_error();
		parse_code_block();
		if(get_string(toks[i++]) != "while")print_error();
		if(get_string(toks[i++]) != "(")print_error();
		parse_expression();
		if(get_string(toks[i++]) != ")")print_error();
	}
	else
	{
		vector<int> exp_toks;
		while(get_string(toks[i]) != ";")
			exp_toks.push_back(toks[i++]);
		parse_expression(exp_toks);
		//TODO // check ;
		if(get_string(toks[i++]) != ";")print_error();
	}
}

// only enter this when you see [
void parse_array()
{
	i++; // pass [
	// check if is number
	// check and pass ]
}

// void parse_expression()
// {
// 	//TODO what if expression start with TYPE
// 	if(toks[i] == IDENT)
// 	{
// 		i++;
// 		string tok = get_string(toks[i]);
// 		if(tok == "(")
// 		{
// 			i++;
// 			parse_function_call();
// 		}
// 		else if(assign_op.count(tok) != 0) // find assignment operator
// 		{
// 			i++;
// 			parse_expression();
// 		}
// 		else if(tok == "--" || tok == "++")
// 		{
// 			i++;
// 			return; //TODO ?????
// 		}
// 		else
// 		{
// 			print_error();
// 		}

// 	}
// 	else if(unary_op.count(get_string(toks[i])) != 0) //unary operation
// 	{
// 		i++;
// 		parse_expression();
// 	}
// 	else if (binary_op.count(get_string(toks[i])) != 0) //TODO NOT check first EXPR, bin_op, expr
// 	{
// 		i++;
// 		parse_expression();
// 	}
// 	else if(get_string(toks[i]) == "?")
// 	{
// 		i++;
// 		parse_expression();
// 		if(get_string(toks[i++]) != ":")print_error();
// 		parse_expression();
// 	}
// 	else if(get_string(toks[i]) == "(")
// 	{
// 		i++;
// 		if (toks[i] == TYPE)
// 		{
// 			i++;
// 			if(get_string(toks[i++]) != ")")print_error();
// 			parse_expression();
// 		}
// 		else
// 		{
// 			print_error();
// 		}
// 	}
// }

void parse_expression(vector<int> exp_toks)
{
	if(exp_toks.size() == 1)
	{
		if(exp_toks[0] != IDENT) // TODO add iteral
			print_error();
		return;
	}
	for(auto x : opt)
	{
		int l = 0;

		for(int n : exp_toks)
		{
			if(get_string(n) == x && l == 0)
			{
				
			}

		}

		// () cases
	}

	// type checking
}

void parse_lvalue(vector<int> exp_toks)
{
	// base case

	if(exp_toks[0] != IDENT)print_error();

	if(exp_toks.size()>1)
	{
		vector<int> arr_exp;
		// get everything between []
		
		parse_expression(arr_exp);
	}


}