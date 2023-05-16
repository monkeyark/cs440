#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "parse.hpp"


using std::unordered_set;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::cerr;


////////////////////////////////////////////////////////
// void output_parse_err(string msg);

// bool is_syntax_match(int value);
// bool next_syntax_match(int value);
// void hard_syntax_match(int value, string msg);
// void output_parse_result(int idx, string entity);

// void declare_syntax();
// void declare_var();
// void parse_declare();
// void parse_statement();
// void parse_const();
// void parse_bracket();
// void parse_function();
// void parse_variable();
// void parse_parameter();
// void parse_parameter_end();
// void parse_ident();
// void parse_code_block();
// void parse_expression();
// void parse_struct();
// void parse_function_call();
// void parse_struct_function_call();
// void parse_struct_declare();
// void Esub();
//////////////////////////////////////////////////////

/////////////////////////////////
// void declare_syntax();
// void D();
// void Func();
// void End();
// void Var();
// void Sign();
// void SigE();
// void I();
// void VD();
// void S();
// void Stmt();
// void B();
// void Else();
// void E();
// void Esub();
// void Es();
// void Inc();
// void U();
// void Bin();
// void As();
// void C();
// void St();
// void StD();
// void StF();
///////////////////////////////


string file_path;
string fname;

vector<int> lines_parse;
vector<int> tokens;
vector<string> words;
vector<string> result;
long unsigned int tok_idx = 0;
int in_block = 0;
int in_struct = 0;
unordered_set<int> assign_op({ASSIGN, PLUSASSIGN, MINUSASSIGN, STARASSIGN, SLASHASSIGN});
unordered_set<int> binary_op({EQ, NE, GT, GE, LT, LE, PLUS, MINUS, STAR, SLASH, MOD, PIPE, AMP, DPIPE, DAMP});
unordered_set<int> unary_op({BANG, MINUS, TILDE});


// Extract filename from the filepath
void filename_extract(string fname)
{
    cout << file_path << endl;

    fname = file_path.substr(file_path.find_last_of("/") + 1);
    cout << fname << endl;
    size_t dot_pos = fname.find_last_of(".");
    if (dot_pos != std::string::npos)
    {
        fname = fname.substr(0, dot_pos);
    }
    cout << fname << endl;

}

int parse_file(string path, bool is_output)
{
    file_path = path;
    filename_extract(file_path);
    lex_file(path , false);
    tokens = get_tokens();
    lines_parse = get_lines();
    words = get_words();
    file_path = path;
    while(tok_idx < tokens.size())
    {
        declare_syntax();
    }
    if(is_output)
    {
        for(auto r : result)
        {
            cout << r << endl;
        }
        return -1;
    }
    return 1;
}

bool is_syntax_match(int value)
{
    if(tok_idx >= tokens.size())
        output_parse_err("expect some thing follow but it's the end");
    return tokens[tok_idx] == value;
}

bool next_syntax_match(int value)
{
    if(tok_idx+1 >= tokens.size())
        output_parse_err("expect some thing follow but it's the end");
    return tokens[tok_idx+1] == value;
}

void hard_syntax_match(int value , string msg)
{
    if(value != tokens[tok_idx])
        output_parse_err(msg);
    tok_idx++;
}

//TODO need to format the output
void output_parse_result(int idx, string kind)
{
    string s = "File " + fname + " Line " + std::to_string(lines_parse[idx]) + ": " + kind + " " + words[idx];
    result.push_back(s);  
}

void output_parse_err(string msg)
{ 
    cerr << "Parser error in file " << file_path << " line " << lines_parse[tok_idx] << " at text " << words[tok_idx] << endl;
    cout << "\t"<< msg << endl; 
    // exit(0);
}

void declare_syntax()
{
    if(is_syntax_match(STRUCT) || next_syntax_match(STRUCT))
    {
        St();
        return;
    }
    if(is_syntax_match(CONST) || next_syntax_match(CONST))
    {
        C();
    }
    else
    {
        hard_syntax_match(TYPE , "Expect type_literal");
    }
    hard_syntax_match(IDENT , "Expect an identifier");
    D();
}


void C()
{
    if(is_syntax_match(CONST))
    {
        tok_idx++;
        if(is_syntax_match(STRUCT))
        {
            tok_idx++;
            hard_syntax_match(IDENT , "Expect an identifier");
        }
        else
            hard_syntax_match(TYPE , "Expect an Type Literal");
    }
    else if(is_syntax_match(TYPE))
    {
        tok_idx++;
        hard_syntax_match(CONST , "Expect an constant Declearation");
    }
    else
    {
        output_parse_err("Expect const keyword or Type Literal");
    }
}


void D()
{
    if(is_syntax_match(LPAR))
    {
        output_parse_result(tok_idx - 1 , "function");
        Func();
    }
    else if(is_syntax_match(LBRAK) || is_syntax_match(COMMA) || is_syntax_match(SEMI) || is_syntax_match(ASSIGN))
    {
        output_parse_result(tok_idx - 1 ,"global variable");
        VD();
    }
    else
        output_parse_err("Expecting a '(' keyword (for function) or keyword that is needed for variable declaration");
}


void VD()
{
    if(is_syntax_match(LBRAK))
    {
        tok_idx++;
        hard_syntax_match(INT_LIT , "Expecting an int Literal");
        hard_syntax_match(RBRAK , "Expecting a ']' keyword");
        Var();
    }
    else if(is_syntax_match(ASSIGN))
    {
        tok_idx++;
        E();
        Var();
    }
    else if(is_syntax_match(COMMA) || is_syntax_match(SEMI))
    {
        Var();
    }
    else
        output_parse_err("Expecting a variable declaration keyword");
}

void Func()
{
    hard_syntax_match(LPAR , "Expecting '(' for function parameter");
    Sign();
    hard_syntax_match(RPAR , "Expecting ')' for function parameter");
    End();
}

void End()
{
    if(is_syntax_match(SEMI))
    {
        tok_idx++;
    }
    else if(is_syntax_match(LBRACE))
    {
        hard_syntax_match(LBRACE , "Expect '{");
        S();
        hard_syntax_match(RBRACE , "expect '}");
    }
}

void Var()
{
if(is_syntax_match(SEMI))
    {
        tok_idx++;
    }
    else if(is_syntax_match(COMMA))
    {
        tok_idx++;
        output_parse_result(tok_idx , (in_struct > 0? "member" : (in_block >0? "local variable" : "global variable")));
        I();
        VD();
    }
}

void Sign()
{
    if(is_syntax_match(RPAR))
        return;

    if(is_syntax_match(STRUCT) || next_syntax_match(STRUCT))
    {
        if(is_syntax_match(CONST))
            tok_idx+=2;
        else if(is_syntax_match(STRUCT))
            tok_idx++;
        else
            output_parse_err("Expect struct in here");

        hard_syntax_match(IDENT , "Expect struct identifier");
        
    }
    else if(is_syntax_match(CONST) || next_syntax_match(CONST))
        C();
    else
        hard_syntax_match(TYPE , "Expect a Type literal");
    output_parse_result(tok_idx , "parameter");
    I();
    SigE();
    Sign();
}

void SigE()
{
    if(is_syntax_match(RPAR))
        return;
    
    hard_syntax_match(COMMA , "Expecting ','");
}

void I()
{
    hard_syntax_match(IDENT , "Expecting an identifier");
    if(is_syntax_match(LBRAK))
    {
        hard_syntax_match(LBRAK, "Expecting an '['");
        if(!is_syntax_match(RBRAK))
            E();
        hard_syntax_match(RBRAK, "Expecting an ']'");
    }
}

void S()
{
    in_block++;
    while(!is_syntax_match(RBRACE))
    {
        if(is_syntax_match(TYPE) || next_syntax_match(TYPE))
        {
            if(is_syntax_match(CONST) || next_syntax_match(CONST))
            {
                C();
            }
            else
            {
                tok_idx++;
            }
            hard_syntax_match(IDENT , "Expect an identifier");
            output_parse_result(tok_idx-1 , "local variable");
            VD();
        }
        else
            Stmt();
    }
    in_block--;
}


void Stmt()
{
    if(is_syntax_match(SEMI)){tok_idx++;return;}
    if(is_syntax_match(BREAK) || is_syntax_match(CONTINUE))
    {
        tok_idx++;
        hard_syntax_match(SEMI , "Expect ;");
    }
    else if(is_syntax_match(RETURN))
    {
        tok_idx++;
        if(!is_syntax_match(SEMI))
            E();
        hard_syntax_match(SEMI , "Expect ;");
    }
    else if(is_syntax_match(IF))
    {
        tok_idx++;
        hard_syntax_match(LPAR , "Expect (");
        E();
        hard_syntax_match(RPAR , "Expect )");
        B();
        Else();
    }
    else if(is_syntax_match(FOR))
    {
        tok_idx++;
        hard_syntax_match(LPAR, "Expect (");
        if(!is_syntax_match(SEMI))
            E();
        hard_syntax_match(SEMI, "Expect ;");
        if(!is_syntax_match(SEMI))
            E();
        hard_syntax_match(SEMI, "expect ;");
        if(!is_syntax_match(RPAR))
            E();
        hard_syntax_match(RPAR, "expect )");
        B();
    }
    else if(is_syntax_match(DO))
    {
        tok_idx++;
        B();
        hard_syntax_match(WHILE, "expect while");
        hard_syntax_match(LPAR, "expect (");
        E();
        hard_syntax_match(RPAR, "expect )");
        hard_syntax_match(SEMI, "expect ;");
    }
    else if(is_syntax_match(STRUCT))
    {
        St();
    }
    else
    {
        E();
        hard_syntax_match(SEMI , "expect ;");
    }
}

void B()
{
    if(is_syntax_match(LBRACE))
    {
        hard_syntax_match(LBRACE , "Expect { ");
        S();
        hard_syntax_match(RBRACE , "Expect } ");
    }
    else
        Stmt();
}

void Else()
{
    if(is_syntax_match(ELSE))
    {
        hard_syntax_match(ELSE , "Expect keyword else");
        B();
    }
}

void E()
{
    Esub();

    if(binary_op.find(tokens[tok_idx]) != binary_op.end())
    {
        tok_idx++;
        E();
    }
    else if(is_syntax_match(QUEST))
    {
        tok_idx++;
        E();
        hard_syntax_match(COLON , "Expect :");
        E();
    }
}
void Esub()
{
    if(tokens[tok_idx] >= CHAR_LIT && tokens[tok_idx] <= STR_LIT)
    {
        tok_idx++;
        return;
    }

    if(is_syntax_match(IDENT))
    {
        if(next_syntax_match(LPAR))
        {
            tok_idx += 2;
            Es();
            hard_syntax_match(RPAR , "Expect )");
            return;
        }

        I();

        while(is_syntax_match(DOT))
        {
            tok_idx++;
            I();
        }

        if(is_syntax_match(INCR) || is_syntax_match(DECR))
            tok_idx++;
        else if(assign_op.find(tokens[tok_idx]) != assign_op.end())
        {
            tok_idx++;
            E();
        }
    }
    else if(is_syntax_match(INCR) || is_syntax_match(DECR))
    {
        tok_idx++;
        Esub();
    }
    else if(unary_op.find(tokens[tok_idx]) != unary_op.end())
    {
        tok_idx++;
        E();
    }
    else if(is_syntax_match(LPAR))
    {
        tok_idx++;
        if(is_syntax_match(TYPE))
        {
            tok_idx++;
            hard_syntax_match(RPAR , "Expect )");
            E();
        }
        else
        {
            E();
            hard_syntax_match(RPAR , "Expect )");
        }
    }
    else
        output_parse_err("Expecting expression");
}

void Es()
{
    while(!is_syntax_match(RPAR))
    {
        E();
        if(is_syntax_match(COMMA))
            tok_idx++;
    }
}
void St()
{
    if(is_syntax_match(CONST))
    {
        tok_idx++;
        hard_syntax_match(STRUCT , "expect struct keyword");
    }
    else if(is_syntax_match(STRUCT))
    {
        tok_idx++;
    }
    else 
        output_parse_err("expect struct");
    
    hard_syntax_match(IDENT , "expect an identifier");

    if(is_syntax_match(LBRACE))
    {
        output_parse_result(tok_idx-1, (in_block > 0)? "local struct":"global struct");
        StF();
        hard_syntax_match(SEMI , "expecting ;");
    }   
    else if(is_syntax_match(IDENT))
    {
        if(next_syntax_match(LPAR))
        {
            output_parse_result(tok_idx , "function");
            hard_syntax_match(IDENT , "expecting ident ");
            Func();
        }
        else
        {
            StD();
            hard_syntax_match(SEMI , "expecting ;");
        }
        
    }
}

void StF()
{
    in_struct++;
    hard_syntax_match(LBRACE , "expecting a { for struct initialization");
    while(!is_syntax_match(RBRACE))
    {
        if(is_syntax_match(STRUCT))
        {
            tok_idx++;
            hard_syntax_match(IDENT , "expecting ident");
            StD();
            hard_syntax_match(SEMI , "expecting ;");
        }
        else
        {
            hard_syntax_match(TYPE , "expecting type");
            output_parse_result(tok_idx , "member");
            hard_syntax_match(IDENT , "expecting ident");
            VD();
        }   
    }
    in_struct--;
    hard_syntax_match(RBRACE , "expecting a } for struct initialization");
}

void StD()
{
    output_parse_result(tok_idx , (in_struct > 0? "member" : (in_block >0? "local variable" : "global variable")));
    I();
}
