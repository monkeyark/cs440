%token NUM PLUS MUL

%%
S : S PLUS P | P;
P : P MUL NUM | NUM;
%%