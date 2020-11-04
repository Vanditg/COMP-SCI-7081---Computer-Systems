/*
Subject Name: Computer Systems
Student: Vandit Jyotindra Gajjar
Student ID: a1779153
Semester: 2
Year: 2020
Assignment: 3
*/

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
// MODIFY the skeleton code below to make an optimised copy an abstract syntax tree, ast, of a Jack class.
//
// NOTE: abstract syntax trees are immutable - they cannot change - and cannot contain cycles so sub-trees
//       can be safely shared across multiple trees. As a result copying a node only requires the creation
//       of a new tree node if one of its fields changes or one of its sub-trees changes.
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate copy_* function to copy any sub-trees
//  - they each return the original tree if none of the fields changed or none of the sub-trees were copied
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
ast copy_class(ast t) ;
ast copy_class_var_decs(ast t) ;
ast copy_var_dec(ast t) ;
ast copy_subr_decs(ast t) ;
ast copy_subr(ast t) ;
ast copy_constructor(ast t) ;
ast copy_function(ast t) ;
ast copy_method(ast t) ;
ast copy_param_list(ast t) ;
ast copy_subr_body(ast t) ;
ast copy_var_decs(ast t) ;
ast copy_statements(ast t) ;
ast copy_statement(ast t) ;
ast copy_let(ast t) ;
ast copy_let_array(ast t) ;
ast copy_if(ast t) ;
ast copy_if_else(ast t) ;
ast copy_while(ast t) ;
ast copy_do(ast t) ;
ast copy_return(ast t) ;
ast copy_return_expr(ast t) ;
ast copy_expr(ast t) ;
ast copy_term(ast t) ;
ast copy_int(ast t) ;
ast copy_string(ast t) ;
ast copy_bool(ast t) ;
ast copy_null(ast t) ;
ast copy_this(ast t) ;
ast copy_unary_op(ast t) ;
ast copy_var(ast t) ;
ast copy_array_index(ast t) ;
ast copy_call_as_function(ast t) ;
ast copy_call_as_method(ast t) ;
ast copy_subr_call(ast t) ;
ast copy_expr_list(ast t) ;
ast copy_infix_op(ast t) ;


static symbols varUsed;

// copy an ast class node with fields:
// class_name - a string
// var_decs   - ast vector of variable declarations
// subr_decs  - ast vector of subroutine declarations
//
ast copy_class(ast t)
{
    varUsed = create_variables();
    string myclassname = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;

    ast subr_decs_copy = copy_subr_decs(subr_decs) ;
    ast var_decs_copy = copy_class_var_decs(var_decs) ;

    if ( var_decs_copy == var_decs && subr_decs_copy == subr_decs ) return t ;

    return create_class(get_ann(t),myclassname,var_decs_copy,subr_decs_copy) ;
}

// copy an ast class var decs node
// it is an ast vector of variable declarations
//
ast copy_class_var_decs(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        ast deci = get_class_var_decs(t,i) ;
        ast copy = copy_var_dec(deci) ;
        if ( deci != copy ) copied = true ;
        
        string name = get_var_dec_name(copy);
        string segment = get_var_dec_segment(copy);
        st_variable variable = lookup_variables(varUsed,name);
        if ( variable.name =="" &&  segment !="this")
        {
            copied = true;
        }
        else
        {
            decs.push_back(copy) ;    
        }
    }

    if ( !copied ) return t ;

    return create_class_var_decs(get_ann(t),decs) ;
}

// copy an ast variable declaration with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
// this is used for statics, fields, parameters and local variables
//
ast copy_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    int offset = get_var_dec_offset(t) ;

    return t ;
}

// copy an ast class var decs node
// it is an ast vector of subroutine declarations
//
ast copy_subr_decs(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_subr_decs(t,i) ;
        ast copy = copy_subr(deci) ;
        if ( deci != copy ) copied = true ;

        decs.push_back(copy) ;
    }

    if ( !copied ) return t ;

    return create_subr_decs(get_ann(t),decs) ;
}

// copy an ast subroutine node with a single field
// subr - an ast constructor, ast function or ast method node
//
ast copy_subr(ast t)
{
    ast subr = get_subr_subr(t) ;
    ast copy ;

    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        copy = copy_constructor(subr) ;
        break ;
    case ast_function:
        copy = copy_function(subr) ;
        break ;
    case ast_method:
        copy = copy_method(subr) ;
        break ;
    default:
        fatal_error(0,"bad subroutine dec found") ;
        copy = nullptr ;
        break ;
    }

    if ( subr == copy ) return t ;

    return create_subr(get_ann(t),copy) ;
}

// copy an ast constructor node with fields
// vtype - a string - the name of the class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
ast copy_constructor(ast t)
{
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    if ( param_list_copy == param_list && subr_body_copy == subr_body ) return t ;

    return create_constructor(get_ann(t),vtype,name,param_list_copy,subr_body_copy) ;
}

// copy an ast function node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
ast copy_function(ast t)
{
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    if ( param_list_copy == param_list && subr_body_copy == subr_body ) return t ;

    return create_function(get_ann(t),vtype,name,param_list_copy,subr_body_copy) ;
}

// copy an ast method node with fields
// vtype - a string - one of "void", "int", "char", "boolean" or the name of a class
// name - a string
// param list - an ast vector of variable declarations
// subr body - an ast subr body node
//
ast copy_method(ast t)
{
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;

    ast param_list_copy = copy_param_list(param_list) ;
    ast subr_body_copy = copy_subr_body(subr_body) ;

    if ( param_list_copy == param_list && subr_body_copy == subr_body ) return t ;

    return create_method(get_ann(t),vtype,name,param_list_copy,subr_body_copy) ;
}

// copy an ast param list node
// it is an ast vector of variable declarations
//
ast copy_param_list(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_param_list(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_param_list(t,i) ;
        ast copy = copy_var_dec(deci) ;
        if ( deci != copy ) copied = true ;

        decs.push_back(copy) ;
    }

    if ( !copied ) return t ;

    return create_param_list(get_ann(t),decs) ;
}

// copy an ast subr body node with fields
// decs - an ast vector of variable declarations
// body - an ast vector of statement nodes
//
ast copy_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    ast body_copy = copy_statements(body) ;
    ast decs_copy = copy_var_decs(decs) ;

    if ( decs_copy == decs && body_copy == body ) return t ;

    return create_subr_body(get_ann(t),decs_copy,body_copy) ;
}

// copy an ast param list node
// it is an ast vector of variable declarations
//
ast copy_var_decs(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_var_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_var_decs(t,i) ;
        ast copy = copy_var_dec(deci) ;
        if ( deci != copy ) copied = true ;

        string name = get_var_dec_name(copy);
        if ( lookup_variables(varUsed,name).name == "" )
        {
            copied = true;
        }
        else
        {
            decs.push_back(copy) ;
        }
        
    }

    if ( !copied ) return t ;

    return create_var_decs(get_ann(t),decs) ;
}

// copy an ast statements node
// it is an ast vector of statement nodes
//
ast copy_statements(ast t)
{
    vector<ast> decs ;

    bool copied = false ;
    int size = size_of_statements(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast deci = get_statements(t,i) ;
        ast copy = copy_statement(deci) ;
        if ( deci != copy ) copied = true ;

        if (ast_have_kind(copy,ast_statements))
        {
            for (int j = 0; j < size_of_statements(copy); j++)
            {
                decs.push_back(get_statements(copy,j)) ;
            }
            continue;
        }

        decs.push_back(copy) ;
    }

    if ( !copied ) return t ;

    return create_statements(get_ann(t),decs) ;
}

// copy an ast statement node with a single field
// statement - one of the following ast nodes, let, let array, if, if else, while, do, return, return expr or statements
//
ast copy_statement(ast t)
{
    ast statement = get_statement_statement(t) ;
    ast copy ;

    switch(ast_node_kind(statement))
    {
    case ast_let:
        copy = copy_let(statement) ;
        break ;
    case ast_let_array:
        copy = copy_let_array(statement) ;
        break ;
    case ast_if:
        copy = copy_if(statement) ;
        if ( ast_have_kind(copy,ast_empty) )
        {
            return create_empty();
        }
        if ( ast_have_kind(copy,ast_statements) )
        {
            return copy;
        }
        break ;
    case ast_if_else:
        copy = copy_if_else(statement) ;
        if ( ast_have_kind(copy,ast_statements) )
        {
            return copy;
        }
        break ;
    case ast_while:
        copy = copy_while(statement) ;
        if ( ast_have_kind(copy,ast_empty) )
        {
            return create_empty();
        }
        break ;
    case ast_do:
        copy = copy_do(statement) ;
        break ;
    case ast_return:
        copy = copy_return(statement) ;
        break ;
    case ast_return_expr:
        copy = copy_return_expr(statement) ;
        break ;
    case ast_statements:
        copy = copy_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        copy = nullptr ;
        break ;
    }

    if ( copy == statement ) return t ;

    return create_statement(get_ann(t),copy) ;
}

// copy an ast let node with fields
// var  - an ast variable
// expr - an ast expr node
//
ast copy_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;

    ast var_copy = copy_var(var) ;
    ast expr_copy = copy_expr(expr) ;

    if ( var_copy == var && expr_copy == expr ) return t ;

    return create_let(get_ann(t),var_copy,expr_copy) ;
}

// copy an ast let array node with fields
// var    - an ast variable
// index  - an ast expr node
// expr   - an ast expr node
//
ast copy_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    ast var_copy = copy_var(var) ;
    ast index_copy = copy_expr(index) ;
    ast expr_copy = copy_expr(expr) ;

    if ( var_copy == var && index_copy == index && expr_copy == expr ) return t ;

    return create_let_array(get_ann(t),var_copy,index_copy,expr_copy) ;
}

// copy an ast if node with fields
// condition - an ast expr node
// if true   - an ast statements node
//
ast copy_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;

    ast condition_copy = copy_expr(condition) ;
    ast if_true_copy = copy_statements(if_true) ;
    int size = size_of_expr(condition_copy);
    if (  size == 1 )
    {
        ast expr = get_expr(condition_copy,0);
        if ( ast_have_kind(expr, ast_term) )
        {
            if ( ast_have_kind(get_term_term(expr),ast_bool) )
            {
                bool _constant = get_bool_t_or_f(get_term_term(expr)) ;
                if ( !_constant )
                {
                    return create_empty();
                }
                else
                {
                    return if_true_copy;
                }
            }
        }
    }

    if ( condition_copy == condition && if_true_copy == if_true ) return t ;

    return create_if(get_ann(t),condition_copy,if_true_copy) ;
}

// copy an ast if else node with fields
// condition - an ast expr node
// if true   - an ast statements node
// if else   - an ast statements node
//
ast copy_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    ast condition_copy = copy_expr(condition) ;
    ast if_true_copy = copy_statements(if_true) ;
    ast if_false_copy = copy_statements(if_false) ;

    int size = size_of_expr(condition_copy);
    if (  size == 1 )
    {
        ast expr = get_expr(condition_copy,0);
        if ( ast_have_kind(expr, ast_term) )
        {
            if ( ast_have_kind(get_term_term(expr),ast_bool) )
            {
                bool _constant = get_bool_t_or_f(get_term_term(expr)) ;
                if ( !_constant )
                {
                    return if_false_copy;
                }
                else
                {
                    return if_true_copy;
                }
            }
        }
    }

    if ( condition_copy == condition && if_true_copy == if_true && if_false_copy == if_false ) return t ;

    return create_if_else(get_ann(t),condition_copy,if_true_copy,if_false_copy) ;
}

// copy an ast while node with fields
// condition - an ast expr node
// body      - an ast statements node
//
ast copy_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;

    ast condition_copy = copy_expr(condition) ;
    ast body_copy = copy_statements(body) ;
    int size = size_of_expr(condition_copy);
    if (  size == 1 )
    {
        ast expr = get_expr(condition_copy,0);
        if ( ast_have_kind(expr, ast_term) )
        {
            if ( ast_have_kind(get_term_term(expr),ast_bool) )
            {
                bool _constant = get_bool_t_or_f(get_term_term(expr)) ;
                if ( !_constant ){
                    return create_empty();
                }
            }
        }
    }

    if ( condition_copy == condition && body_copy == body ) return t ;

    return create_while(get_ann(t),condition_copy,body_copy) ;
}

// copy an ast do node with a single field
// call - an ast call as function node or an ast call as method node
//
ast copy_do(ast t)
{
    ast call = get_do_call(t) ;
    ast copy ;

    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        copy = copy_call_as_function(call) ;
        break ;
    case ast_call_as_method:
        copy = copy_call_as_method(call) ;
        break ;
    default:
        fatal_error(0,"Unexpected call kind\n") ;
        copy = nullptr ;
        break ;
    }

    if ( copy == call ) return t ;

    return create_do(get_ann(t),copy) ;
}

// copy an ast return node, it has not fields
// nothing to do ...
//
ast copy_return(ast t)
{
    return t ;
}

// copy an ast return expr node with a single field
// expr - an ast expr node
//
ast copy_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    ast copy = copy_expr(expr) ;

    if ( copy == expr ) return t ;

    return create_return_expr(get_ann(t),copy) ;
}

// copy an ast param list node
// it is an ast vector of ast term and infix op nodes
// it must be of odd length > 0, ie 1, 3, 5, 7, ...
// all elements at even indices are an ast term node
// all elements at odd indices are an ast infix op
//
ast copy_expr(ast t)
{
    vector<ast> terms ;

    bool copied = false ;
    int size = size_of_expr(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast termop = get_expr(t,i) ;
        ast copy = i % 2 == 0 ? copy_term(termop) : copy_infix_op(termop) ;
        if ( termop != copy ) copied = true ;

        terms.push_back(copy) ;
    }

    if ( !copied ) return t ;

    return create_expr(get_ann(t),terms) ;
}

// copy an ast term node with a single field
// term - one of the following ast nodes:
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
ast copy_term(ast t)
{
    ast term = get_term_term(t) ;
    ast copy ;

    switch(ast_node_kind(term))
    {
    case ast_int:
        copy = copy_int(term) ;
        break ;
    case ast_string:
        copy = copy_string(term) ;
        break ;
    case ast_bool:
        copy = copy_bool(term) ;
        break ;
    case ast_null:
        copy = copy_null(term) ;
        break ;
    case ast_this:
        copy = copy_this(term) ;
        break ;
    case ast_expr:
        copy = copy_expr(term) ;
        break ;
    case ast_unary_op:
        copy = copy_unary_op(term) ;
        break ;
    case ast_var:
        copy = copy_var(term) ;
        break ;
    case ast_array_index:
        copy = copy_array_index(term) ;
        break ;
    case ast_call_as_function:
        copy = copy_call_as_function(term) ;
        break ;
    case ast_call_as_method:
        copy = copy_call_as_method(term) ;
        break ;
    default:
        fatal_error(0,"Unexpected term kind\n") ;
        copy = nullptr ;
        break ;
    }
    
    if ( copy == term ) return t ;

    return create_term(get_ann(t),copy) ;
}

// copy an ast int node with a single field
// constant - an integer in the range -32,768 to 32,767
//
ast copy_int(ast t)
{
    //int _constant = get_int_constant(t) ;

    return t ;
}

// copy an ast string node with a single field
// constant - a string
//
ast copy_string(ast t)
{
    //string _constant = get_string_constant(t) ;

    return t ;
}

// copy an ast bool node with a single field
// constant - either true or false
//
ast copy_bool(ast t)
{
    //bool _constant = get_bool_t_or_f(t) ;

    return t ;
}

// copy an ast null node, it has not fields
// nothing to do ...
//
ast copy_null(ast t)
{
    return t ;
}

// copy an ast this node, it has not fields
// nothing to do ...
//
ast copy_this(ast t)
{
    return t ;
}

// copy an ast unary op node with fields
// op   - a string, either "-" or "~"
// term - an ast term node
//        int, string, bool, null, this, expr, unary op,
//        variable, array index, call as function or call as method
//
ast copy_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;

    ast copy = copy_term(term) ;

    if ( copy == term ) return t ;

    return create_unary_op(get_ann(t),uop,copy) ;
}

// copy an ast variable node with fields
// name - a string
// type - a string - "int", "char", "boolean" or a class name
// segment - a string
// offset - an int
//
ast copy_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;

    st_variable variable = st_variable(name,type,segment,offset);
    if ( lookup_variables(varUsed,name).name == "" )
    {
        insert_variables(varUsed,name,variable) ;
    }
    return t ;
}

// copy an ast array index node with fields
// var   - an ast variable node
// index - an ast expr node
//
ast copy_array_index(ast t)
{

    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    ast var_copy = copy_var(var) ;
    ast index_copy = copy_expr(index) ;

    if ( var_copy == var && index_copy == index ) return t ;

    return create_array_index(get_ann(t),var_copy,index_copy) ;
}

// copy an ast subr call as method with fields
// class name - a string
// call       - an ast subr call node
//
ast copy_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;

    ast subr_call_copy = copy_subr_call(subr_call) ;

    if ( subr_call_copy == subr_call ) return t ;

    return create_call_as_function(get_ann(t),class_name,subr_call_copy) ;
}

// copy an ast subr call as method with fields
// class name - a string
// var        - an ast variable or ast this node, hidden first parameter of the call
// call       - an ast subr call node
//
ast copy_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;

    ast var_copy = ast_node_kind(var) == ast_var ? copy_var(var) : copy_this(var) ;
    ast subr_call_copy = copy_subr_call(subr_call) ;

    if ( var_copy == var && subr_call_copy == subr_call ) return t ;

    return create_call_as_method(get_ann(t),class_name,var_copy,subr_call_copy) ;
}

// copy an ast subr call node with fields
// name      - a string
// expr list - a vector of ast expr nodes
//
ast copy_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;

    ast copy = copy_expr_list(expr_list) ;

    if ( copy == expr_list ) return t ;

    return create_subr_call(get_ann(t),subr_name,copy) ;
}

// copy an ast expr list node
// it is an ast vector of ast expr nodes
//
ast copy_expr_list(ast t)
{
    vector<ast> exprs ;

    bool copied = false ;
    int size = size_of_expr_list(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        ast expri = get_expr_list(t,i) ;
        ast copy = copy_expr(expri) ;
        if ( expri != copy ) copied = true ;

        exprs.push_back(copy) ;
    }

    if ( !copied ) return t ;

    return create_expr_list(get_ann(t),exprs) ;
}

// copy an ast infix op node with a single field
// op - a string - one of "+", "-", "*", "/", "&", "|", "<", ">", "="
//
ast copy_infix_op(ast t)
{
    //string op = get_infix_op_op(t) ;

    return t ;
}

// main program
int main(int argc,char **argv)
{
    // walk an AST in XML and print VM code
    ast_print_as_xml(copy_class(ast_parse_xml()),4) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

