
%token NUM LBRACE RBRACE COMMA
%%
S : LBRACE L RBRACE;
L : L COMMA I | I;
I : NUM | S;
%%