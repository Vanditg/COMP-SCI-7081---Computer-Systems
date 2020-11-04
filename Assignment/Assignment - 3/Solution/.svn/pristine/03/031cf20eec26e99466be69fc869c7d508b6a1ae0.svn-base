/*
Subject Name: Computer Systems
Student: Vandit Jyotindra Gajjar
Student ID: a1779153
Semester: 2
Year: 2020
Assignment: 3
*/

#include <string>
#include <vector>
#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to parse a Jack class and construct the equivalent abstract syntax tree, ast.
//        The main function will print the ast formatted as XML
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// NOTE: use the fatal_token_context() function to print fatal error messages with the current
//       input file position marked.
//
// The skeleton code has one function per non-terminal in the Jack grammar
//  - comments describing how to create relevant ast nodes prefix most functions
//  - ast nodes are immutable - you must create all the sub-trees before creating a new node
//
// The structure of the skeleton code is just a suggestion
//  - it matches the grammar but does not quite match the ast structure
//  - you will need to change the parameters / results / functions to suit your own logic
//  - you can change it as much as you like
//
// NOTE: the best place to start is to write code to parse the input without attempting to create
//       any ast nodes. If correct this parser will report errors when it finds syntax errors but
//       correct programs will fail because the tree printer will not be provided with a valid ast.
//       Once the parsing works modify your code to create the ast nodes.
//
// NOTE: the return create_empty() ; statements are only present so that this skeleton code will compile.
//       these statements need to be replaced by code that creates the correct ast nodes.
//

// The Jack grammar to be recognised:
// program          ::= One or more classes, each class in a separate file named <class_name>'.Jack'
// class            ::= 'class' identifier '{' class_var_decs subr_decs '}'
// class_var_decs   ::= (static_var_dec | field_var_dec)*
// static_var_dec   ::= 'static' type identifier (',' identifier)* ';'
// field_var_dec    ::= 'field' type identifier (',' identifier)* ';'
// type             ::= 'int' | 'char' | 'boolean' | identifier
// vtype            ::= 'void' | type
// subr_decs        ::= (constructor | function | method)*
// constructor      ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// function         ::= 'function' vtype identifier '(' param_list ')' subr_body
// method           ::= 'method' vtype identifier '(' param_list ')' subr_body
// param_list       ::= ((type identifier) (',' type identifier)*)?
// subr_body        ::= '{' var_decs statements '}'
// var_decs         ::= var_dec*
// var_dec          ::= 'var' type identifier (',' identifier)* ';'

// statements       ::= statement*
// statement        ::= let | if | while | do | return
// let              ::= 'let' identifier index? '=' expr ';'
// if               ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// while            ::= 'while' '(' expr ')' '{' statements '}'
// do               ::= 'do' identifier (id_call | call) ';'
// return           ::= 'return' expr? ';'

// expr             ::= term (infix_op term)*
// term             ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// var_term         ::= identifier (index | id_call | call)?
// id_call          ::= '.' identifier call
// call             ::= '(' expr_list ')'
// expr_list        ::= (expr (',' expr)*)?
// infix_op         ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// unary_op         ::= '-' | '~'


// forward declarations of one function per non-terminal rule in the Jack grammar - except for program
// NOTE: parse_type(), parse_vtype(), parse_unary_op() all return a Token not an ast
//
ast parse_class() ;
ast parse_class_var_decs() ;
ast parse_static_var_dec() ;
ast parse_field_var_dec() ;
ast parse_subr_decs() ;
ast parse_constructor() ;
ast parse_function() ;
ast parse_method() ;
ast parse_param_list(string) ;
ast parse_subr_body() ;
ast parse_var_decs() ;
vector<ast> parse_var_dec() ;

ast parse_statements() ;
ast parse_statement() ;
ast parse_let() ;
ast parse_if() ;
ast parse_while() ;
ast parse_do() ;
ast parse_return() ;

ast parse_expr() ;
ast parse_term() ;
ast parse_var_term() ;
ast parse_index() ;
ast parse_id_call() ;
ast parse_call() ;
ast parse_expr_list() ;
ast parse_infix_op() ;


static symbols classVariable;
static symbols localVariable;

static string className;
static int fieldOffset = 0;
static int staticOffset = 0;

static void fatal(string message)
{
    fatal_error(0,"***** Error:\n" + token_context() + message) ;
}


static st_variable lookup_variable (string key) 
{
    if ( lookup_variables(localVariable,key).name == "" )
    {
        return lookup_variables(classVariable,key);
    }
    else
    {
        return lookup_variables(localVariable,key);
    }
}

// class ::= 'class' identifier '{' class_var_decs subr_decs '}'
// create_class(myclassname,class_var_decs,class_subrs)
ast parse_class()
{
    push_error_context("parse_class()") ;

    classVariable = create_variables();
    mustbe(tk_class);
    className = token_spelling(mustbe(tk_identifier));
    
    mustbe(tk_lcb);
    ast class_var_decs = parse_class_var_decs();
    ast class_subrs = parse_subr_decs();
    mustbe(tk_rcb);
    
    delete_variables(classVariable);
    pop_error_context() ;
    return create_class(className,class_var_decs,class_subrs) ;
}

// class_var_decs ::= (static_var_dec | field_var_dec)*
// returns ast_class_var_decs: create_class_var_decs(vector<ast> decs)
// create_class_var_decs must be passed a vector of ast_var_decs
//
ast parse_class_var_decs()
{
    push_error_context("parse_class_var_decs()") ;
    vector<ast> decs;
    while ( have(tg_starts_class_var) )
    {
        if ( have(tk_static) )
        {
            decs.push_back(parse_static_var_dec());
        }
        else if ( have(tk_field) )
        {
            decs.push_back(parse_field_var_dec());
        }
        else 
        {
            fatal("Unexpected kind");
        }
    }

    pop_error_context() ;
    return create_class_var_decs(decs) ;
    //return create_empty();
}

// static_var_dec ::= 'static' type identifier (',' identifier)* ';'
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_static_var_dec()
{
    push_error_context("parse_class()") ;

    vector<ast> decs;
    string name;
    string segment = "static";
    st_variable variable;
    
    mustbe(tk_static);
    string type = token_spelling(mustbe(tg_starts_type));
    name = token_spelling(mustbe(tk_identifier));
    decs.push_back(create_var_dec(name,segment,staticOffset,type));
    variable = st_variable(name,type,segment,staticOffset);
    staticOffset++;
    
    if ( lookup_variables(classVariable,name).name =="" )
    {
        insert_variables(classVariable,name,variable);
    }
    
    while ( have(tk_comma) )
    {
        mustbe(tk_comma);
        name = token_spelling(mustbe(tk_identifier));
        variable = st_variable(name,type,segment,staticOffset);
        
        if ( lookup_variables(classVariable,name).name =="" )
        {
            insert_variables(classVariable,name,variable);
        }
        decs.push_back(create_var_dec(name,segment,staticOffset,type));
        staticOffset++;
    }
    mustbe(tk_semi);

    pop_error_context() ;
    return create_var_decs(decs) ;
}

// field_var_dec ::= 'field' type identifier (',' identifier)* ';'
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_field_var_dec()
{
    push_error_context("parse_class()") ;

    vector<ast> decs;
    string name;
    string segment = "this";
    st_variable variable;
    
    mustbe(tk_field);
    string type = token_spelling(mustbe(tg_starts_type));
    name = token_spelling(mustbe(tk_identifier));
    decs.push_back(create_var_dec(name,segment,fieldOffset,type));
    variable = st_variable(name,type,segment,fieldOffset);
    fieldOffset++;
    
    if ( lookup_variables(classVariable,name).name =="" )
    {
        insert_variables(classVariable,name,variable);
    }
    
    while ( have(tk_comma) )
    {
        mustbe(tk_comma);
        name = token_spelling(mustbe(tk_identifier));
        variable = st_variable(name,type,segment,fieldOffset);
        
        if ( lookup_variables(classVariable,name).name =="" )
        {
            insert_variables(classVariable,name,variable);
        }
        decs.push_back(create_var_dec(name,segment,fieldOffset,type));
        fieldOffset++;
    }
    mustbe(tk_semi);

    pop_error_context() ;
    return create_var_decs(decs) ;
}



// subr_decs ::= (constructor | function | method)*
// returns ast_subr_decs: create_subr_decs(vector<ast> subrs)
// create_subr_decs must be passed an vector of ast_subr
//
// ast_subr: create_subr(ast subr)
// create_subr must be passed one of: ast_constructor, ast_function or ast_method
//
ast parse_subr_decs()
{
    push_error_context("parse_subr_decs()") ;

    vector<ast> decs;
    while ( have(tg_starts_subroutine) )
    {
        if ( have(tk_constructor) )
        {
            decs.push_back(create_subr(parse_constructor()));
        }
        else if ( have(tk_function) )
        {
            decs.push_back(create_subr(parse_function()));
        }
        else if ( have(tk_method) )
        {
            decs.push_back(create_subr(parse_method()));
        }
        else
        {
            fatal("Unexpected kind");
        }

    }

    pop_error_context() ;
    return create_subr_decs(decs) ;
}

// constructor ::= 'constructor' identifier identifier '(' param_list ')' subr_body
// returns ast_constructor: create_constructor(string vtype,string name,ast params,ast body)
// . vtype: the constructor's return type, this must be it's class name
// . name: the constructor's name within its class
// . params: ast_param_list - the constructor's parameters
// . body: ast_subr_body - the constructor's body
//
ast parse_constructor()
{
    push_error_context("parse_constructor()") ;
    
    localVariable = create_variables();
    mustbe(tk_constructor);
    string vtype = token_spelling(mustbe(tk_identifier));
    string name = token_spelling(mustbe(tk_identifier));
    
    mustbe(tk_lrb);
    ast params = parse_param_list("constructor");
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    
    delete_variables(localVariable);
    
    pop_error_context() ;
    return create_constructor(vtype,name,params,body) ;
}

// function ::= 'function' vtype identifier '(' param_list ')' subr_body
// returns ast_function: create_function(string vtype,string name,ast params,ast body)
// . vtype: the function's return type
// . name: the function's name within its class
// . params: ast_param_list - the function's parameters
// . body: ast_subr_body - the function's body
//
ast parse_function()
{
    push_error_context("parse_function()") ; 

    localVariable  = create_variables();
    mustbe(tk_function);
    
    string vtype = token_spelling(mustbe(tg_starts_vtype));
    string name = token_spelling(mustbe(tk_identifier));
    
    mustbe(tk_lrb);
    ast params = parse_param_list("function");
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    
    delete_variables(localVariable);

    pop_error_context() ;
    return create_function(vtype,name,params,body);
}

// method ::= 'method' vtype identifier '(' param_list ')' subr_body
// returns ast_method: create_method(string vtype,string name,ast params,ast body)
// . vtype: the method's return type
// . name: the method;s name within its class
// . params: ast_param_list - the method's explicit parameters
// . body: ast_subr_body - the method's body
//
ast parse_method()
{
    push_error_context("parse_method()") ;

    localVariable = create_variables();
    mustbe(tk_method);
    
    string vtype = token_spelling(mustbe(tg_starts_vtype));
    string name = token_spelling(mustbe(tk_identifier));
    
    mustbe(tk_lrb);
    ast params = parse_param_list("method");
    mustbe(tk_rrb);
    ast body = parse_subr_body();
    
    delete_variables(localVariable);

    pop_error_context() ;
    return create_method(vtype,name,params,body) ;
}

// param_list ::= ((type identifier) (',' type identifier)*)?
// returns ast_param_list: create_param_list(vector<ast> params)
// create_param_list must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
ast parse_param_list(string parameter)
{
    push_error_context("parse_param_list()") ;

    vector<ast> params;
    string name;
    string segment = "argument";
    string type;
    st_variable variable;
    int offset = 0;
    
    if ( parameter == "method" )
    {
        offset++;   
    }
    
    if ( have(tg_starts_type) )
    {
        type = token_spelling(mustbe(tg_starts_type));
        name = token_spelling(mustbe(tk_identifier));
        variable = st_variable(name,type,segment,offset);
        
        if ( lookup_variables(localVariable,name).name =="" )
        {
            insert_variables(localVariable,name,variable);
        }
        params.push_back(create_var_dec(name,segment,offset,type));
    }
    
    while ( have(tk_comma) ) 
    {
        offset++;
        mustbe(tk_comma);
        type = token_spelling(mustbe(tg_starts_type));
        name = token_spelling(mustbe(tk_identifier));
        variable = st_variable(name,type,segment,offset);
        
        if ( lookup_variables(localVariable,name).name =="" )
        {
            insert_variables(localVariable,name,variable);
        }
        params.push_back(create_var_dec(name,segment,offset,type));
    }

    pop_error_context() ;
    return create_param_list(params) ;
}

// subr_body ::= '{' var_decs statements '}'
// returns ast_subr_body: create_subr_body(ast decs,ast body)
// create_subr_body must be passed:
// . decs: ast_var_decs - the subroutine's local variable declarations
// . body: ast_statements - the statements within the body of the subroutinue
//
ast parse_subr_body()
{
    push_error_context("parse_subr_body()") ;

    mustbe(tk_lcb);
    ast decs = parse_var_decs();
    ast body = parse_statements();
    mustbe(tk_rcb);
    
    pop_error_context() ;
    return create_subr_body(decs,body) ;
}

// var_decs ::= var_dec*
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_decs
//
ast parse_var_decs()
{
    push_error_context("parse_var_decs()") ;

    vector<ast> decs;
    
    if ( have(tk_var) )
    {
        decs = parse_var_dec();
    }

    pop_error_context() ;
    return create_var_decs(decs) ;
}

// var_dec ::= 'var' type identifier (',' identifier)* ';'
// returns ast_var_decs: create_var_decs(vector<ast> decs)
// create_var_decs must be passed a vector of ast_var_dec
//
// ast_var_dec: create_var_dec(string name,string segment,int offset,string type)
// create_var_dec must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
vector<ast> parse_var_dec()
{
    vector<ast> decs ;
    push_error_context("parse_var_dec()") ;

    string name;
    string segment = "local";
    st_variable variable;
    int localOffset = -1;
    string type;
    
    while ( have(tk_var) )
    {
        mustbe(tk_var);
        type = token_spelling(mustbe(tg_starts_type));
        name = token_spelling(mustbe(tk_identifier));
        localOffset++;
        variable = st_variable(name,type,segment,localOffset) ;
        
        if ( lookup_variables(localVariable,name).name =="" )
        {
            insert_variables(localVariable,name,variable);
        }
        decs.push_back(create_var_dec(name,segment,localOffset,type));
       
        while ( have(tk_comma) )
        {
            mustbe(tk_comma);
            name = token_spelling(mustbe(tk_identifier));
            localOffset++;
            variable = st_variable(name,type,segment,localOffset) ;
            
            if ( lookup_variables(localVariable,name).name =="" )
            {
                insert_variables(localVariable,name,variable);
            }
            decs.push_back(create_var_dec(name,segment,localOffset,type));
        }
        mustbe(tk_semi);
    }
    
    pop_error_context() ;
    return decs ;
}

// statements ::= statement*
// create_statements(vector<ast> statements)
// create_statements must be passed a vector of ast_statement
//
ast parse_statements()
{
    push_error_context("parse_statements()") ;

    vector<ast> statements;
    
    while ( have(tg_starts_statement) )
    {
        statements.push_back(parse_statement());
    }

    pop_error_context() ;
    return create_statements(statements) ;
}

// statement ::= let | if | while | do | return
// create_statement(ast statement)
// create_statement initialiser must be one of: ast_let, ast_let_array, ast_if, ast_if_else, ast_while, ast_do, ast_return or ast_return_expr
//
ast parse_statement()
{
    push_error_context("parse_statement()") ;

    ast statement  = nullptr;
    if ( have(tg_starts_statement) )
    {
        if ( have(tk_let) )
        {
            statement = parse_let();
        }
        else if ( have(tk_if) )
        {
            statement = parse_if();
        }
        else if ( have(tk_while) )
        {
            statement = parse_while();
        }
        else if ( have(tk_do) )
        {
            statement = parse_do();
        }
        else if ( have(tk_return) )
        {
            statement = parse_return();
        }
        else
        {
            fatal("Unexpected kind");
        }
    } 

    pop_error_context() ;
    return create_statement(statement) ;
}

// let ::= 'let' identifier index? '=' expr ';'
// return one of: ast_let or ast_let_array
//
// create_let(ast var,ast expr)
// . var: ast_var - the variable being assigned to
// . expr: ast_expr - the variable's new value
//
// create_let_array(ast var,ast index,ast expr)
// . var: ast_var - the variable for the array being assigned to
// . index: ast_expr - the array index
// . expr: ast_expr - the array element's new value
//
ast parse_let()
{
    push_error_context("parse_let()") ;

    mustbe(tk_let);
    st_variable variable = lookup_variable(token_spelling(mustbe(tk_identifier)));
    ast var = create_var(variable.name,variable.segment,variable.offset,variable.type);
    ast expr;
    
    if ( have(tk_lsb) )
    {
        ast index = parse_index();
        mustbe(tk_eq);
        expr = parse_expr();
        mustbe(tk_semi);
        return create_let_array(var,index,expr);
    }
    
    mustbe(tk_eq);
    expr = parse_expr();
    mustbe(tk_semi);

    pop_error_context() ;
    return create_let(var,expr) ;
}

// if ::= 'if' '(' expr ')' '{' statements '}' ('else' '{' statements '}')?
// return one of: ast_if or ast_if_else
//
// create_if(ast condition,ast if_true)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
//
// create_if_else(ast condition,ast if_true,ast if_false)
// . condition: ast_expr - the if condition
// . if_true: ast_statements - the if true branch
// . if_false: ast_statements - the if false branch
//
ast parse_if()
{
    push_error_context("parse_if()") ;

    mustbe(tk_if);
    
    mustbe(tk_lrb);
    ast condition = parse_expr();
    mustbe(tk_rrb);
    
    mustbe(tk_lcb);
    ast if_true = parse_statements();
    mustbe(tk_rcb);
    
    if ( have(tk_else) )
    {
        mustbe(tk_else);
        mustbe(tk_lcb);
        ast if_false = parse_statements();
        mustbe(tk_rcb);
        return create_if_else(condition,if_true,if_false);
    }

    pop_error_context() ;
    return create_if(condition,if_true) ;
}

// while ::= 'while' '(' expr ')' '{' statements '}'
// returns ast_while: create_while(ast condition,ast body)
// . condition: ast_expr - the loop condition
// . body: ast_statements - the loop body
//
ast parse_while()
{
    push_error_context("parse_while()") ;

    mustbe(tk_while);
    mustbe(tk_lrb);
    ast condition = parse_expr();
    mustbe(tk_rrb);
   
    mustbe(tk_lcb);
    ast body = parse_statements();
    mustbe(tk_rcb);

    pop_error_context() ;
    return create_while(condition,body) ;
}

// do ::= 'do' identifier (call | id_call) ';'
// returns ast_do: create_do(ast call)
// create_do must be passed one of: ast_call_as_function or ast_call_as_method
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_do()
{
    push_error_context("parse_do()") ;

    mustbe(tk_do);
    ast call = nullptr;

    string name = token_spelling(mustbe(tk_identifier));
    st_variable variable = lookup_variable(name);
    ast var = create_var(variable.name,variable.segment,variable.offset,variable.type);
    
    if ( variable.name!="" && have(tk_stop) )
    {
        ast id_call = parse_id_call();
        call = create_call_as_method(variable.type,var,id_call);
    }
    else if ( have(tk_stop) )
    {
        ast id_call = parse_id_call();
        call = create_call_as_function(name,id_call);
    }
    else if ( variable.name=="" && !have(tk_stop) )
    {
        ast expr_list = parse_call();
        call = create_call_as_method(className,create_this(),create_subr_call(name,expr_list));
    }
    else 
    {
        fatal("Unexpected kind");
    }
    
    mustbe(tk_semi);
    pop_error_context() ;
    return create_do(call) ;
}

// return ::= 'return' expr? ';'
// returns one of: ast_return or ast_return_expr
//
// ast_return: create_return()
//
// ast_return_expr: create_return_expr(ast expr)
// create_return_expr must be passed an ast_expr
//
ast parse_return()
{
    push_error_context("parse_return()") ;

    mustbe(tk_return);
    
    if ( have(tg_starts_term) )
    {
        ast expr = parse_expr();
        mustbe(tk_semi);
        return create_return_expr(expr); 
    }
    
    mustbe(tk_semi);
    pop_error_context() ;
    return create_return() ;
}

// expr ::= term (infix_op term)*
// returns ast_expr: create_expr(vector<ast> expr)
// the vector passed to create_expr:
// . must be an odd length of at least 1, ie 1, 3, 5, ...
// . must be  a vector alternating between ast_term and ast_infix_op nodes, ending with an ast_term
//
ast parse_expr()
{
    push_error_context("parse_expr()") ;

    vector<ast> expr;
    expr.push_back(parse_term());
    
    if ( have(tg_infix_op) )
    {
        while ( have(tg_infix_op) )
        {
            expr.push_back(parse_infix_op());
            expr.push_back(parse_term());
        }
    }

    pop_error_context() ;
    return create_expr(expr) ;
}

// term ::= integer_constant | string_constant | 'true' | 'false' | 'null' | 'this' | '(' expr ')' | unary_op term | var_term
// returns ast_term: create_term(ast term)
// create_term must be passed one of: ast_int, ast_string, ast_bool, ast_null, ast_this, ast_expr,
//                                    ast_unary_op, ast_var, ast_array_index, ast_call_as_function, ast_call_as_method
//
// ast_int: create_int(int _constant)
// create_int must be passed an integer value in the range 0 to 32767
//
// ast_string: create_string(string _constant)
// create_string must be passed any legal Jack string literal
//
// ast_bool: create_bool(bool t_or_f)
// create_bool must be passed true or false
//
// ast_null: create_null()
//
// ast_this: create_this()
//
// ast_unary_op: create_unary_op(string op,ast term)
// create_unary_op must be passed:
// . op: the unary op
// . term: ast_term
//
ast parse_term()
{
    push_error_context("parse_term()") ;

    ast term = nullptr;
    if ( have(tg_starts_term) )
    {
        if ( have(tk_integerConstant) )
        {
            term = create_int(token_ivalue(mustbe(tk_integerConstant)));
        }
        else if ( have(tk_stringConstant) )
        {
            term = create_string(token_spelling(mustbe(tk_stringConstant)));
        }
        else if ( have(tk_true) )
        {
            term = create_bool(mustbe(tk_true));
        }
        else if ( have(tk_false) )
        {
            term = create_bool(false);
            mustbe(tk_false);
        }
        else if ( have(tk_null) )
        {
            term = create_null();
            mustbe(tk_null);
        }
        else if ( have(tk_this) )
        {
            term = create_this();
            mustbe(tk_this);
        }
        else if ( have(tk_lrb) )
        {
            mustbe(tk_lrb);
            term = parse_expr();
            mustbe(tk_rrb);
        }
        else if ( have(tg_unary_op) )
        {
           string op = token_spelling();
           mustbe(tg_unary_op);
           ast temp = parse_term();
           term = create_unary_op(op,temp);
        }
        else if ( have(tk_identifier) )
        {
            term = parse_var_term();
        }
        else 
        {
            fatal("Unexpected kind");
        }
    }

    pop_error_context() ;
    return create_term(term) ;
}

// var_term ::= identifier (index | id_call | call)?
// returns one of: ast_var, ast_array_index, ast_call_as_function or ast_call_as_method
//
// ast_var: create_var(string name,string segment,int offset,string type)
// create_ast_var must be passed:
// . name: the variable's name
// . segment: the variable's segment
// . offset: the variable's position in it's segment
// . type: the variable's type
//
// ast_array_index: create_array_index(ast var,ast index)
// create_ast_array_index must be passed:
// . var: ast_var - the array variable
// . index: ast_expr - the index into the array
//
// ast_call_as_function: create_call_as_function(string class_name,ast subr_call)
// create_ast_call_as_function must be passed:
// . class_name: name of the function's class
// . subr_call: ast_subr_call - the function's name within it's class and it's explicit arguments
//
// ast_call_as_method: create_call_as_method(string class_name,ast object,ast subr_call)
// create_ast_call_as_method must be passed:
// . class_name: name of the method's class
// . object: ast_expr - the object the method is applied to
// . subr_call: ast_subr_call - the method's name within it's class and it's explicit arguments
//
ast parse_var_term()
{
    push_error_context("parse_var_term()") ;

    string name = token_spelling(mustbe(tk_identifier));
    st_variable variable = lookup_variable(name);
    ast var = create_var(variable.name,variable.segment,variable.offset,variable.type);
    
    if ( have(tk_lsb) )
    {
        ast index = parse_index();
        return create_array_index(var,index);
    }
    else if ( variable.name!="" && have(tk_stop) )
    {
        ast id_call = parse_id_call();
        return create_call_as_method(variable.type,var,id_call);
    }
    else if ( have(tk_stop) )
    {
        ast id_call = parse_id_call();
        return create_call_as_function(name,id_call);
    }
    else if ( variable.name=="" && !have(tk_stop) )
    {
        ast expr_list = parse_call();
        return create_call_as_method(className,create_this(),create_subr_call(name,expr_list));
    }

    pop_error_context() ;
    return var ;
}

// index ::= '[' expr ']'
// returns ast_expr
ast parse_index()
{
    push_error_context("parse_index()") ;

    mustbe(tk_lsb);
    ast expr = parse_expr();
    mustbe(tk_rsb);

    pop_error_context() ;
    return expr ;
}

// id_call ::= '.' identifier call
// returns ast_subr_call: create_subr_call(string subr_name,ast expr_list)
// create_subr_call must be passed:
// . subr_name: the constructor, function or method's name within its class
// . expr_list: ast_expr_list - the explicit arguments to the call
//
ast parse_id_call()
{
    push_error_context("parse_id_call()") ;

    mustbe(tk_stop);
    string subr_name = token_spelling(mustbe(tk_identifier));
    ast expr_list = parse_call();

    pop_error_context() ;
    return create_subr_call(subr_name,expr_list) ;
}

// call ::= '(' expr_list ')'
// returns ast_expr_list
//
ast parse_call()
{
    push_error_context("parse_call()") ;
    
    mustbe(tk_lrb);
    ast expr_list = parse_expr_list();
    mustbe(tk_rrb);

    pop_error_context() ;
    return expr_list ;
}

// expr_list ::= (expr (',' expr)*)?
// returns ast_expr_list: create_expr_list(vector<ast> exprs)
// create_expr_list must be passed: a vector of ast_expr
//
ast parse_expr_list()
{
    push_error_context("parse_expr_list()") ;

    vector<ast> expr;
    if ( have(tg_starts_term) )
    {
        expr.push_back(parse_expr());
        
        while ( have(tk_comma) )
        {
            mustbe(tk_comma);
            expr.push_back(parse_expr());
        }
    }

    pop_error_context() ;
    return create_expr_list(expr) ;
}

// infix_op ::= '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
// returns ast_op: create_infix_op(string infix_op)
// create_infix_op must be passed:
// infix_op: the infix op
//
ast parse_infix_op()
{
    push_error_context("parse_infix_op()") ;

    string op = token_spelling(mustbe(tg_infix_op));

    pop_error_context() ;
    return create_infix_op(op) ;
}


ast jack_parser()
{
    // read the first token to get the tokeniser initialised
    next_token() ;

    // construct tree and return as result
    return parse_class() ;
}

// main program
int main(int argc,char **argv)
{
    // parse a Jack class and print the abstract syntax tree as XML
    ast_print_as_xml(jack_parser(),4) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
