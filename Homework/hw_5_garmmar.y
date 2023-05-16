%{
#include <stdio.h>
%}

%token lbrace rbrace comma number

%%
S: lbrace L rbrace
  ;

L: L comma I
  | I
  ;

I: number
  | S
  ;

%%

int main() {
    yyparse();
    return 0;
}

int yyerror(const char* msg) {
    printf("Error: %s\n", msg);
    return 0;
}
