#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "lex.hpp"

int parse_file(std::string file_path, bool is_output);



/////////////////////////////////////
// enum tokentype {
// 	END     = 0,
// 	COMMA   = ',',
// 	DOT     = '.',
// 	SEMI    = ';',
// 	LPAR    = '(',
// 	RPAR    = ')',
// 	LBRAK   = '[',
// 	RBRAK   = ']',
// 	LBRACE  = '{',
// 	RBRACE  = '}',
// 	GT      = '>',
// 	LT      = '<',
// 	ASSIGN  = '=',
// 	PLUS    = '+',
// 	MINUS   = '-',
// 	STAR    = '*',
// 	SLASH   = '/',
// 	MOD     = '%',
// 	COLON   = ':',
// 	QUEST   = '?',
// 	TILDE   = '~',
// 	PIPE    = '|',
// 	AMP     = '&',
// 	BANG    = '!',

// /* Stuff with attributes */

// 	TYPE      = 301,
// 	CHAR_LIT  = 302,
// 	INT_LIT   = 303,
// 	REAL_LIT  = 304,
// 	STR_LIT   = 305,
// 	IDENT     = 306,

// /* Symbols */

// 	EQ        = 351,
// 	NE        = 352,
// 	GE        = 353,
// 	LE        = 354,
// 	INCR      = 355,
// 	DECR      = 356,
// 	DPIPE     = 357,
// 	DAMP      = 358,

// 	PLUSASSIGN    = 361,
// 	MINUSASSIGN   = 362,
// 	STARASSIGN    = 363,
// 	SLASHASSIGN   = 364,

// /* Keywords */

// 	CONST     = 401,
// 	STRUCT    = 402,
// 	FOR       = 403,
// 	WHILE     = 404,
// 	DO        = 405,
// 	IF        = 406,
// 	ELSE      = 407,
// 	BREAK     = 408,
// 	CONTINUE  = 409,
// 	RETURN    = 410,
// 	SWITCH    = 411,
// 	CASE      = 412,
// 	DEFAULT   = 413,
// };
//
// int lex_file(std::string file_path, bool is_output);
// std::vector<std::string> get_words();
// std::vector<int> get_tokens();
// std::vector<int> get_lines();


/////////////////////////////////
void declare_syntax();
void output_parse_err(std::string msg);
void D();
void Func();
void End();
void Var();
void Sign();
void SigE();
void I();
void VD();
void S();
void Stmt();
void B();
void Else();
void E();
void Esub();
void Es();
void Inc();
void U();
void Bin();
void As();
void C();
void St();
void StD();
void StF();
///////////////////////////////





#endif
