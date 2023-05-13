#ifndef TYPE_HPP
#define TYPE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm> 
#include <stack>

#include "lex.hpp"

using std::string;
using std::map;
using std::vector;

class ident {
    public:
        string type;
        string name;
        bool isconst;
        bool isarray;
        bool isstruct;
        bool isgobal;
        map<string , ident*> idents;
        int array_size;
        int index;
        int line;
        ident(string s);
        string get_type();
        string get_array_size();
        string type_with_strcut();
};
class Token{
    public:
        int num;
        int tline;
        string word;
        string ttype;
        Token(int a , string s);
};
class Express{
    public:
        vector<Token*> tok;
        int label;
        Express* des;
        bool loop_end;
        bool need_condi;
        Express();
};

class function {

    public:
        map<string , ident*> para;
        map<string , ident*> local;
        vector<ident*> idents;
        bool isarray;
        bool isstruct;
        string type;
        string name;
        string get_type();
        function(string t);
        vector<Express*> Expressions;
        string toString();
        int depth;
};





string dup(string dup_input);
void is_match();
void next_match();
vector<function*> type_check_file(string filename ,bool is_output);
void Declear();
void Gobal_declear(string type , bool is_const, bool is_gobal, bool is_struct);
void Function_declear(string type , bool is_struct);
string Expression(bool report , bool in_func);
void Function_parameter(function* func , string function_name);
void block(string type);
void Statement(string type);
string Lvalue(bool report , bool in_func);
void Struct_declear(bool is_gobal);

#endif