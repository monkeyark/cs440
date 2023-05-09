#ifndef LEX_HPP
#define LEX_HPP

#include <string>

#define find_max(x,y) (((x) >= (y)) ? (x) : (y))
#define find_min(x,y) (((x) <= (y)) ? (x) : (y))

using std::string;

enum tokentype {
	END     = 0,
	COMMA   = ',',
	DOT     = '.',
	SEMI    = ';',
	LPAR    = '(',
	RPAR    = ')',
	LBRAK   = '[',
	RBRAK   = ']',
	LBRACE  = '{',
	RBRACE  = '}',
	GT      = '>',
	LT      = '<',
	ASSIGN  = '=',
	PLUS    = '+',
	MINUS   = '-',
	STAR    = '*',
	SLASH   = '/',
	MOD     = '%',
	COLON   = ':',
	QUEST   = '?',
	TILDE   = '~',
	PIPE    = '|',
	AMP     = '&',
	BANG    = '!',

/* Stuff with attributes */

	TYPE      = 301,
	CHAR_LIT  = 302,
	INT_LIT   = 303,
	REAL_LIT  = 304,
	STR_LIT   = 305,
	IDENT     = 306,

/* Symbols */

	EQ        = 351,
	NE        = 352,
	GE        = 353,
	LE        = 354,
	INCR      = 355,
	DECR      = 356,
	DPIPE     = 357,
	DAMP      = 358,

	PLUSASSIGN    = 361,
	MINUSASSIGN   = 362,
	STARASSIGN    = 363,
	SLASHASSIGN   = 364,

/* Keywords */

	CONST     = 401,
	STRUCT    = 402,
	FOR       = 403,
	WHILE     = 404,
	DO        = 405,
	IF        = 406,
	ELSE      = 407,
	BREAK     = 408,
	CONTINUE  = 409,
	RETURN    = 410,
	SWITCH    = 411,
	CASE      = 412,
	DEFAULT   = 413,

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

int lex_file(string file_path);


#endif
