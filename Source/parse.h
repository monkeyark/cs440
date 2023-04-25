#include <iostream>
#include "lex.h"

int global_val();

int var_decr();

int type();

int func_dclr();

int formal_para();

int func_dfin();



int stat();
int stat_nothing();
int stat_expr();
int stat_keywords();
int stat_return();
int stat_if();
int stat_for();
int stat_while();
int stat_do_while();

int expr();
// A literal value
int expr_lit();
int expr_ident();
int expr_lval();
int expr_lval_assign();
int expr_lval_oprt();
int expr_unary_oprt();
int expr_binary_oprt();
int expr_question();
int expr_type();
int expr_parent();

int lval();
int unary_oprt();
int binary_oprt();
int assign_oprt();
