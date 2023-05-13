#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "type.hpp"
#include "parse.hpp"

using std::cout;
using std::endl;
using std::queue;
using std::unordered_set;
using std::unordered_map;


queue<string> output;
queue<string> error_output;
map<string , ident*> gobal;
map<string , ident*> gobal_struct;
map<string , ident*> local_struct;
map<string , ident*> local;
map<string , function*> functions;
vector<function*> result_func;
vector<int> token;
vector<int> line;
vector<string> word;
queue<Express*> condi_exp;
int idx = 0;
int label_count = 0;
bool need_label = false;
// unordered_set<int> assign ({61,361,362,363,364});
// unordered_set<int> binary ({351,352,62,353,60,354,43,45,42,47,37,124,38,357,358});
// unordered_set<int> binary2 ({60,62,351,352,353,354,357,358});
// unordered_set<int> unary ({33,45 , '~'});
unordered_set<int> assign({ASSIGN, PLUSASSIGN, MINUSASSIGN, STARASSIGN, SLASHASSIGN});
unordered_set<int> binary({EQ, NE, GT, GE, LT, LE, PLUS, MINUS, STAR, SLASH, MOD, PIPE, AMP, DPIPE, DAMP});
unordered_set<int> binary2({LT, GT, EQ, NE, GE, LE, PLUS, MINUS, STAR, SLASH, MOD, PIPE, AMP, DPIPE, DAMP});
unordered_set<int> unary({BANG, MINUS, TILDE});

function* last_function;
unordered_set<string> type_N ({"char", "int" , "float" , "const char", "const int" , "const float"});
unordered_set<string> type_I ({"char" , "int" , "const char" , "const int"});
unordered_set<string> type_C ({"const char[]","char[]","string"});

ident::ident(string s)
{
    type = s;
    isconst = false;
    isarray = false;
    isstruct = false;
    array_size = 0;
}

Express::Express()
{
    this->label = -1;
    this->des = NULL;
    this->loop_end = false;
    this->need_condi = false;
    if(need_label)
    {
        label = ++label_count;
        while(!condi_exp.empty())
        {
            condi_exp.front()->des = this;
            condi_exp.pop();
        }
        need_label = false;
    }    
}

string ident::type_with_strcut()
{
    if(isstruct)return "struct " + type;
    return type;
}

string ident::get_type()
{
    string result = type;
    if(isarray)
        result += "[]";
    if(isstruct)
        result = "struct " + result;
    if(isconst)
        result = "const " + result;

    return result;
}

string ident::get_array_size()
{
    if(array_size != 0)
        return "[" + std::to_string(array_size) + "]";

    return "[]";
}

function::function(string t)
{
    type = t;
    isarray = false;
    isstruct = false;
}

string function::get_type()
{
    if(isarray)
        return type + "[]";
    
    return type;
}

string function::toString()
{
    string result = "";
    result += type + " " + name + "\n";
    for(auto x : para)
        result += x.first + " " + x.second->get_type() + " , ";

    result += '\n';

    for(auto x : local)
        result += x.first + " " + x.second->get_type() + " , ";

    for(auto x : Expressions)
    {
        result += std::to_string(x->tok.front()->num) + "\n";
    }

    return result;
}

Token::Token(int a , string s)
{
    num = a;
    word = s;
    tline = line[idx];
}

bool same_type(string t1 , string t2)
{

    //cout << t1 << " :: " << t2 <<endl;
    //t1 can not change t2 can change
    if(t1 == t2)
        return true;
    if(t1 == "const " + t2 || "const " + t1 == t2)
        return true;
    if((t1 == "string" && t2 == "char[]")||(t1 == "char[]" && t2 == "string"))
        return true;
    if(type_C.find(t1) != type_C.end() && type_C.find(t2) != type_C.end())
        return true;
    if(((t2 == "char" || t2 == "const char") && type_N.find(t1) != type_N.end()) || ((t2 == "int" || t2 == "const int") && (t1 == "float" || t1 == "const float")))
        return true;
    
    return false;
}

string defined_location(string name)
{
    if(last_function->para.find(name) != last_function->para.end())
        return "para";
    if(local.find(name) != local.end())
        return "local";
    if(gobal.find(name) != gobal.end())
        return "gobal";
    
    return "none";   
}

string read_cur_word()
{
    // if(last_function != NULL && last_function->Expressions.size() > 0)
    //     last_function->Expressions.back().push_back(token[idx]);
    return word[idx++];
}

bool is_match(int value)
{
    return token[idx] == value;
}

bool next_match(int value)
{
    return token[idx+1] == value;
}
void hard_match(int value , string msg)
{
    //cout << line[idx] << " : " << word[idx] << endl;
    idx++;
}
vector<function*> start_typeCheck(string filename , bool need_print)
{
    parse_file(filename ,false);
    token = get_tokens();
    line = get_lines();
    word = get_words();
    result_func.push_back(new function("gobal"));
    function* getchar = new function("int");
    getchar->name = "getchar";
    function* putchar = new function("int");
    putchar->name = "putchar";
    putchar->para["a"] = new ident("int");
    putchar->idents.push_back(putchar->para["a"]);

    result_func.push_back(getchar);
    result_func.push_back(putchar);
    
    functions["getchar"] = getchar;
    functions["putchar"] = putchar;

    while(idx < token.size())
    {
        Declear();
    }

    result_func[0]->local = gobal;

    if(error_output.size() > 0)
    {
        result_func.clear();
        while(!error_output.empty())
        {
            cout << error_output.front() << endl;
            error_output.pop();
        }
    }
    else
    {
        while(!output.empty())
        {
            if(need_print)
                cout << output.front() << endl;
            output.pop();
        }
    }


    // for(auto x : result_func)
    //     cout << x->toString() << endl;
    return result_func;
}

void Declear()
{
    if(is_match(402) && token[idx+2] == 123)
    {
        Struct_declear(true);
        idx++;
        return;
    }

    string cur_type = "";
    bool is_const = false;
    bool is_struct = false;   
    for(int i = idx ; i < std::min(idx+3 , (int)token.size()) ; i++)
    {
        if(token[i] == 40)break;
        if(token[i] == 401)is_const=true;
        if(token[i] == 402)is_struct=true;
        if((token[i] == 306 && is_struct && cur_type == "") || (token[i] == 301))cur_type = word[i];
    }

    if(is_const)idx++;
    if(is_struct)idx+=2;
    else idx++;

    //cout << cur_type << endl;
    if(next_match(40))
    {
        Function_declear(cur_type , is_struct);
    }
    else
    {
        Gobal_declear(cur_type , is_const , true , is_struct);
    }

}

void Gobal_declear(string type , bool is_const , bool is_gobal , bool is_struct)
{
    map<string , ident*> *temp;
    map<string , ident*> *struct_temp = NULL;
    if(is_gobal)
        temp = &gobal;
    else
        temp = &local; 

    if(is_struct)
    {
        if(local_struct.find(type) != local_struct.end())
            struct_temp = &local_struct;
        else
            struct_temp = &gobal_struct;
    }
    
    if(type == "void" || type == "")
    {
        int cur_line = line[idx];
        error_output.push("Error in line " + std::to_string(cur_line) + "\n\t can not declear a void type");
    }
        

    do{
        if(is_match(44))idx++;

        int cur_line = line[idx];
        ident* cur = new ident(type);
        cur->isconst = is_const;
        cur->isstruct = is_struct;
        cur->line = cur_line;
        string cur_name = read_cur_word();
        cur->name = cur_name;
        if(*temp->find(cur_name) != *temp->end())
            error_output.push("Error in line "+ std::to_string(cur_line) + "\n\t" + cur_name + " already exist");
        if(is_struct && *struct_temp->find(type) == *struct_temp->end())
            error_output.push("Error in line " + std::to_string(cur_line) + "\n\tstruct " + type + " not defined" );
        (*temp)[cur_name] = cur;
        if(is_match(91))
        {
            cur->isarray = true;
            idx++;
            if(is_match(303))
                cur->array_size = stoi(read_cur_word());
            idx++;
        }

        if(is_match(61))
        {
            idx++;
            string result_type = Expression(false , false);
            if(!same_type(cur->get_type() , result_type))
            {
                string cur_error = "Error in line " + std::to_string(cur_line) + "\n\texpected " + cur->get_type() + 
                "but get " +  result_type;
                error_output.push(cur_error);
            }
        }

        // string cur_output =(is_gobal? "" : "   ") + string("Line ") + std::to_string(cur_line) + (is_gobal? " gobal " : " local ") + (cur->isconst? "const " : "") +(is_struct?"struct " : "") +cur->type + string(" ") + cur_name + (cur->isarray ? cur->get_array_size() : "" ); 
        // output.push(cur_output);

    }while(!is_match(59));
    idx++;
}


void Function_declear(string type , bool is_struct)
{
    function* cur = new function(type);
    string name = read_cur_word();
    cur->name = name;
    cur->isstruct = is_struct;
    if(functions.find(name) != functions.end())
    {
        error_output.push("Error line " + std::to_string(line[idx-1])  + ":\n\t function " + name + " already defined");
    }
    else
    {
        functions[name] = cur;
    }
    
    last_function =cur;

    Function_parameter(cur , name);
    if(!condi_exp.empty())
    {
        need_label = true;
        cur->Expressions.push_back(new Express());
    }
    last_function = NULL;
    result_func.push_back(cur);
    for(auto x : local)
        cur->local[x.first] = x.second;
    local.clear();
    local_struct.clear();
}

void Function_parameter(function* func , string function_name)
{
    idx++;

    int temp = idx;
    bool is_pototype = false;
    while(!is_match(123) && !is_match(59))
        idx++;
    if(is_match(59))
        is_pototype = true;
    idx = temp;
    if(!is_pototype)        
        // output.push("Line " + std::to_string(line[idx-1]) + " : function " + func->get_type() + " " + function_name);

    while(!is_match(41))
    {
        if(is_match(44))idx++;

        string cur_type = "";
        bool is_const = false;
        bool is_struct = false;
            
        for(int i = idx ; i < std::min(idx+3 , (int)token.size()) ; i++)
        {
            if(token[i] == 401)is_const=true;
            if(token[i] == 402)is_struct=true;
            if((token[i] == 306 && is_struct && cur_type == "") || (token[i] == 301))cur_type = word[i];
        }

        if(is_const)idx++;
        if(is_struct)idx+=2;
        else idx++;

        int cur_line = line[idx];
        ident* para = new ident(cur_type);
        para->isconst = is_const;
        para->isstruct = is_struct;
        para->line = cur_line;
        if(is_match(91))
        {
            para->isarray = true;
            idx++;
            if(is_match(303))
            {
                para->array_size= stoi(read_cur_word());
            }
            idx++;
        }

        string ident_name = read_cur_word();

        if(is_match(91))
        {
            para->isarray = true;
            idx++;
            if(is_match(303))
            {
                para->array_size= stoi(read_cur_word());
            }
            idx++;
        }

        if(func->para.find(ident_name) != func->para.end())
        {
            string error = "Error in line " + std::to_string(cur_line) + "\n\tparameter " + ident_name + " exist already";
            error_output.push(error);
        }

        if(is_struct && gobal_struct.find(cur_type) == gobal_struct.end())
            error_output.push("Error in line " + std::to_string(cur_line) + "\n\tstruct " + cur_type + " not defined" );
       
        func->para[ident_name] = para;
        func->idents.push_back(para);
        // string temp = "   Line " + std::to_string(cur_line) + ": parameter " +  (para->isconst? "const " : "")+para->type + " " + ident_name +(para->isarray ? para->get_array_size() : "" );
        // if(!is_pototype)
        //     output.push(temp);
        
    }
    idx++;

    if(is_match(59))
    {
        idx++;
        return;
    }

    block(func->get_type());

}

void block(string type)
{
    if(is_match(123))
    {
        idx++;
        while(!is_match(125))
        {
            Statement(type);
        }
        idx++;
    }
    else
    {
        Statement(type);
    }
    

}

void Statement(string type)
{
    if(is_match(59)){idx++;return;}
    if(is_match(408) || is_match(409))
    {
        idx+=2;
    }
    else if(is_match(410))
    {
        last_function->Expressions.push_back(new Express());
        last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
        string return_type = (is_match(59)?"void" : Expression(false,true));
        last_function->Expressions.back()->tok.front()->ttype = return_type;
        // output.push("   Line " + std::to_string(line[idx-1]) + " : expression has type " + return_type);
        
        if(!same_type((last_function->isstruct? "struct " : "") + type , return_type))
        {
            error_output.push("Error in line " + std::to_string(line[idx-1])  + "\n\tnot match the function return type");
        }
    }
    else if(is_match(406))
    {
        idx++;
        hard_match(40 , "expect ( stmt1");
        
        Express* cur_condi = new Express();
        cur_condi->need_condi = true;
        last_function->Expressions.push_back(cur_condi);
        string cur =Expression(true,true);
        last_function->Expressions.back()->tok.front()->ttype = cur;
        output.push("   Line " + std::to_string(line[idx-1]) + " : expression has type " + cur);
        if(type_N.find(cur) == type_N.end())
            error_output.push("Error in line " + std::to_string(line[idx-1]) + "\n\tinvliad condition " + cur);
        hard_match(41 , "expect ) stmt2");
        
        block(type);
        condi_exp.push(cur_condi);
        need_label =true;
            
        if(is_match(407))
        {
            hard_match(407 , "expect else");
            block(type);
        }
    }
    else if(is_match(403))
    {
        idx++;
        hard_match(40 , "expect (");
        last_function->Expressions.push_back(new Express());
        int cur_line = line[idx];
        string result = Expression(true , true);
        last_function->Expressions.back()->tok.front()->ttype = result;
        output.push("   Line " + std::to_string(cur_line) + " : expression has type " + result);
        hard_match(59 , "expect ;");
        need_label = true;
        Express* cur_condi = new Express();
        cur_condi->need_condi = true;
        last_function->Expressions.push_back(cur_condi);
        string cur =Expression(true,true);
        last_function->Expressions.back()->tok.front()->ttype = cur;
        output.push("   Line " + std::to_string(line[idx-1]) + " : expression has type " + cur);
        if(type_N.find(cur) == type_N.end())
            error_output.push("Error in line " + std::to_string(line[idx-1]) + "\n\tinvliad condition " + cur);
        hard_match(59 , "expect ;");
        last_function->Expressions.push_back(new Express());
        cur_line = line[idx];
        result = Expression(true , true);
        last_function->Expressions.back()->tok.front()->ttype = result;
        output.push("   Line " + std::to_string(cur_line) + " : expression has type " + result);
        hard_match(41 , "expect )");
        block(type);
        Express* end_condi = new Express();
        end_condi->loop_end = true;
        end_condi->des = cur_condi;
        last_function->Expressions.push_back(end_condi);
        need_label=true;
        condi_exp.push(cur_condi);
    }
    else if(is_match(404))
    {
        idx++;
        hard_match(40 , "expect (");
        need_label = true;
        Express* cur_condi = new Express();
        last_function->Expressions.push_back(cur_condi);
        string cur =Expression(true,true);
        cur_condi->need_condi = true;
        last_function->Expressions.back()->tok.front()->ttype = cur;
        output.push("   Line " + std::to_string(line[idx-1]) + " : expression has type " + cur);
        if(type_N.find(cur) == type_N.end())
            error_output.push("Error in line " + std::to_string(line[idx-1]) + "\n\tinvliad condition " + cur);
        hard_match(41 , "expect )");
        block(type);
        Express* end_condi = new Express();
        end_condi->loop_end = true;
        end_condi->des = cur_condi;
        last_function->Expressions.push_back(end_condi);
        need_label=true;
        condi_exp.push(cur_condi);
    }
    else if(is_match(405))
    {
        idx++;
        need_label = true;
        block(type);
        hard_match(404 , "expect while");
        hard_match(40 , "expect (");
        Expression(false,true);
        hard_match(41 , "expect )");
        hard_match(59 , "expect ;");
    }
    else if(is_match(402) || next_match(402))
    {
        if(is_match(402) && token[idx+2] == 123)
        {
            Struct_declear(true);
            idx++;
            return;
        }

        string cur_type = "";
        bool is_const = false;
        bool is_struct = false;
            
        for(int i = idx ; i < std::min(idx+3 , (int)token.size()) ; i++)
        {
            if(token[i] == 401)is_const=true;
            if(token[i] == 402)is_struct=true;
            if((token[i] == 306 && is_struct && cur_type == "") || (token[i] == 301))cur_type = word[i];
        }

        if(is_const)idx++;
        if(is_struct)idx+=2;
        else idx++;

        Gobal_declear(cur_type , is_const , false , is_struct);
    }
    else if(is_match(301) || next_match(301) && !is_match(40))
    {
        string cur_type = "";
        bool is_const = false;
        if(is_match(401) || next_match(401))
        {
            if(is_match(401))
            {
                idx++;
                is_const=true;
                cur_type = read_cur_word();
            }
            else
            {
                cur_type = read_cur_word();
                is_const= true;
                idx++;
            }

        }
        else
        {
            cur_type = read_cur_word();
        }

        Gobal_declear(cur_type,is_const,false , false);
    }
    else
    {
        int cur_line = line[idx];

        last_function->Expressions.push_back(new Express());
        string result = Expression(true , true);
        last_function->Expressions.back()->tok.front()->ttype = result;
        output.push("   Line " + std::to_string(cur_line) + " : expression has type " + result);
        hard_match(59 , "expect ;");
    }
}

string Expression(bool report , bool in_func)
{
    string result = "void";
    bool isconst = false;
    bool is_literal = false;
    if(is_match(306) || (token[idx] >=302 && token[idx] <= 305))
    {
        if(token[idx] >=302 && token[idx] <= 305)
        {
            is_literal = true;
            switch (token[idx])
            {
            case 302:
                if(in_func)
                    last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                else
                    idx++;
                result =  "const char";
                break;
            case 303:
                if(in_func)
                    last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                else
                    idx++;
                result =  "const int";
                break;
            case 304:
                if(in_func)
                    last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                else
                    idx++;
                result =  "const float";
                break;
            case 305:
                if(in_func)
                    last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                else
                    idx++;
                result =  "string";
                break;
            default:
                break;
            }
        }
        else if(next_match(40))
        {
            string func_name = read_cur_word();
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx-1] , func_name));

            function* cur_func;
            if(functions.find(func_name) != functions.end())
                cur_func = functions[func_name];
            else
            {
                if(report)
                    error_output.push("Error in Line " + std::to_string(line[idx-1]) + ":\n\tfunction "+ func_name + " not defined");
                while(!is_match(41))
                {
                    if(in_func)
                        last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                    else
                        idx++;
                }
                return "void";
            }
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;

            vector<string> types;
            
            while(!is_match(41))
            {
                types.push_back(Expression(report,in_func));
                if(!is_match(41))
                {
                    if(in_func)
                        last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
                    else
                        idx++;
                }
                    
            }

            if(types.size() != cur_func->idents.size() && report)
                error_output.push("Error ilne " + std::to_string(line[idx-1]) + ":\n\t incorrect number of paramter for function " + func_name);
            else
            {
                for(int i = 0 ; i< cur_func->idents.size() ; i++)
                {
                    //cout << types[i] << " : " << cur_func->idents[i]->get_type() << endl;
                    string temp = cur_func->idents[i]->get_type();
                    //cout << types[i] << " 1: " << temp << endl;
                    if(!same_type(types[i], temp) && report)
                        error_output.push("Error line " + std::to_string(line[idx-1]) + ":\n\t different type for function " + func_name);
                }
            }
            
            result = cur_func->get_type();
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        }
        else
        {
            result = Lvalue(report , in_func);
        }
        int cur_line = line[idx];
        if((is_match(355) || is_match(356) ) && !is_literal)
        {
            if (result.find("const") != string::npos) {
                error_output.push("Error in line :" + std::to_string(cur_line) + "\n\tcan not modify const");
            }
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            // cout << "result :" << result << endl;
            if(type_N.find(result) == type_N.end() && report)
                error_output.push("error in line :" + std::to_string(cur_line) + "\n\tnot N type in");
        }
        else if(assign.find(token[idx]) != assign.end() && !is_literal)
        {
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            string right = Expression(report,in_func);
            // cout << "result :" << result << endl;
            // cout << "right :" << right << endl;
            if (result.find("const") != string::npos) {
                 error_output.push("Error in line :" + std::to_string(cur_line) + "\n\tcan not modify const");
            }
            if(!same_type(result , right) || (result.find("[") != std::string::npos) || (right.find("[") != std::string::npos) )
                if(report)
                    error_output.push("error in line :" + std::to_string(cur_line) + "\n\tnot N type in");
        }

    }
    else if(is_match(355) || is_match(356))
    {
        int cur_line = line[idx];
        if (result.find("const") != string::npos) {
            error_output.push("Error in line :" + std::to_string(cur_line) + "\n\tcan not modify const");
        }
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        
        result = Expression(report,in_func);
        if(type_N.find(result) == type_N.end() && report)
                error_output.push("error in line :" + std::to_string(cur_line) + "\n\tnot N type in");
    }
    else if(unary.find(token[idx]) != unary.end())
    {
        // TODO unary type change
        string cur_word = word[idx];
        int cur_line = line[idx];
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        result = Expression(report,in_func);
        if(result == "void" ||  (result.find("[") != std::string::npos))
            error_output.push("Error on line : " + std::to_string(cur_line) + "\n\tcan not do operation " + cur_word + " on " + result);
        if(cur_word == "!")
            result = "char";
    }
    else if(is_match(40))
    {
        if(in_func)
            last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
        else
            idx++;
        if(is_match(301))
        {
            int cur_line = line[idx];
            result = "const " + word[idx];
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            string sec = Expression(report,in_func);
            // cout << sec << endl;
            // cout << result << endl;
            if((type_N.find(result) == type_N.end() || type_N.find(sec) == type_N.end()) && report)
                error_output.push("error in line :" + std::to_string(cur_line) + "\n\tnot N type in");

        }
        else
        {
            result = Expression(report,in_func);
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            
        }
    }

    if(binary.find(token[idx]) != binary.end())
    {
        string cur_word = word[idx];
        int str = token[idx];
        int cur_line = line[idx];
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        string right = Expression(report,in_func);
        if((!same_type(right, result) && report) || (result.find("[") != std::string::npos) || (right.find("[") != std::string::npos) || (result == "string") || (right == "string") || ((result == "void" || right == "void")))
                error_output.push("error at line :" + std::to_string(cur_line) + "\n\t can not do the operation " + result + " "  + cur_word + " " + right);
        if(binary2.find(str) != binary2.end())
            return "char";
        return result;

    }
    else if(is_match(63))
    {
        int cur_line = line[idx];
       
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        string first = Expression(report,in_func);
        hard_match(58 , "expect :");
        string second = Expression(report,in_func);

        if((!same_type(first , second) || result.find("[") != std::string::npos)&& report) 
            error_output.push("error at line: " + std::to_string(cur_line) + "\n\t can not do the operation " + result + " ? " + first + " : " + second);

        return second;
    }

    return result;
}

string Lvalue(bool report , bool in_func)
{
    string name = read_cur_word();
    if(in_func)
        last_function->Expressions.back()->tok.push_back(new Token(token[idx-1] , name));
    string result = "void";
    
    string loco = defined_location(name);
    map<string , ident*> *cur = NULL;
    if(loco == "para")
        cur = &(last_function->para);
    else if(loco == "local")
        cur = &local;
    else if(loco == "gobal")
        cur = &gobal;
    else
    {
        if(report)
            error_output.push("error line " + std::to_string(line[idx-1]) + " : \n\t" + name + " not defined");
        while(word[idx] != ";")idx++;
        return result;
        // if(is_match(91))
        // {
        //     if(in_func)
        //         last_function->Expressions.back().push_back(new Token(token[idx] , read_cur_word()));
        //     else
        //         idx++;
        //     string mid = Expression(report , in_func);
        //     cout << mid << endl;
        //     if(!same_type(mid , "const int"))
        //         error_output.push("error line " + std::to_string(line[idx]) + "\n\t array index can only be int");
        //     if(in_func)
        //         last_function->Expressions.back().push_back(new Token(token[idx] , read_cur_word()));
        //     else
        //         idx++;
        // }
    }


    ident* cur_ident = NULL;

    if(cur != NULL)
    cur_ident = (*cur)[name];

    if(is_match(91))
    {
        if(cur_ident != NULL && !(cur_ident->isarray))
            error_output.push("error in line " + std::to_string(line[idx]) + "\n\t" + name + " is not array");
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        string mid = Expression(report,in_func);
       // cout << mid << endl;
            if(!same_type(mid , "const int"))
                error_output.push("error line " + std::to_string(line[idx]) + "\n\t array index can only be int");
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
        result = (cur->find(name) != cur->end()) ? (*cur)[name]->type_with_strcut() : "void";
    }
    else
        result = (cur->find(name) != cur->end()) ? (*cur)[name]->get_type() : "void"; 

    ident* temp = (*cur)[name];
    map<string , ident*> *stemp;
    if(local_struct.find(result) != local_struct.end())
        stemp = &local_struct;
    else
        stemp = &gobal_struct;
    while(is_match(46))
    {
        string next;    
        if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;    
        if(temp == NULL || !temp->isstruct)
        {
            error_output.push("error in line " + std::to_string(line[idx-1]) +"\n\t" +name + " not an struct can not use . ");
        }
        else
        {
            next = read_cur_word();
            
            if((*stemp)[temp->type]->idents.find(next) == (*stemp)[temp->type]->idents.end())
            {
                error_output.push("error in line " + std::to_string(line[idx-1]) +"\n\t" +name + " not have member");
                temp == NULL;
            }
            else
            {
                name = next;
                temp = (*stemp)[temp->type]->idents[next];               
            }
        }

        if(is_match(91))
        {
            cur_ident = temp;
            if(cur_ident != NULL && !cur_ident->isarray)
            error_output.push("error in line " + std::to_string(line[idx]) + "\n\t" + name + " is not array");
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            Expression(report,in_func);
            if(in_func)
                last_function->Expressions.back()->tok.push_back(new Token(token[idx] , read_cur_word()));
            else
                idx++;
            result = (temp != NULL) ? temp->type_with_strcut() : "void";
        }
        else
            result = (temp != NULL) ? temp->get_type() : "void";

    }
        
    return result;
    
}

void Struct_declear(bool is_gobal)
{
    map<string , ident*> *temp;
    
    
    if(is_gobal)
        temp = &gobal_struct;
    else
        temp = &local_struct;
    idx++;
    string type = read_cur_word();
    ident* cur = new ident(type);
    if(temp->find(type) != temp->end())
        error_output.push("error in line " + std::to_string(line[idx]) + "\n\tstruct " + type +" already exist");
    
    // output.push("Line " + std::to_string(line[idx-1]) + (is_gobal? " gobal " : " local ") + cur->get_type());
    (*temp)[type] = cur;

    idx++;

    while(!is_match(125))
    {
        string cur_type = "";
        bool is_const = false;
        bool is_struct = false;
        
        for(int i = idx ; i < idx+3 ; i++)
        {
            if(token[i] == 401)is_const=true;
            if(token[i] == 402)is_struct=true;
            if((token[i] == 306 && is_struct && cur_type == "") || (token[i] == 301))cur_type = word[i];
        }

        if(is_const)idx++;
        if(is_struct)idx+=2;
        else idx++;

        while(!is_match(59))
        {
            ident* cur_ident = new ident(cur_type);
            string name = read_cur_word();
            cur_ident->isstruct = is_struct;
            cur_ident->isconst = is_const;
            if(is_match(91))
            {
                cur_ident->isarray = true;
                idx++;
                if(is_match(303))
                    cur_ident->array_size = stoi(read_cur_word());
                idx++;
            }
            if(cur->idents.find(name) != cur->idents.end())
                error_output.push("error in line " + std::to_string(line[idx-1]) + "\n\t" + name + " already defined in struct");
            if(is_struct && temp->find(cur_type) == temp->end())
                error_output.push("error in line " + std::to_string(line[idx-1]) + "\n\tundefined type " + cur_ident->get_type());
            cur->idents[name] = cur_ident;
            // output.push("   Line " + std::to_string(line[idx-1]) + ": member " + (is_const? "const " : "")  +cur_ident->type_with_strcut() + " " + name  + (cur_ident->isarray? cur_ident->get_array_size() : ""));
            if(is_match(44))
                idx++;
        }
        idx++;
    }
    idx++;
}

