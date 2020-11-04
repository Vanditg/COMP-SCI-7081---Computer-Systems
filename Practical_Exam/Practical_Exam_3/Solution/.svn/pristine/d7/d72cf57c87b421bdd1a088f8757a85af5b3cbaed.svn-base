#include "iobuffer.h"
#include "symbols.h"
#include "tokeniser.h"
#include "abstract-syntax-tree.h"

using namespace std ;

using namespace CS_IO_Buffers ;
using namespace CS_Symbol_Tables ;
using namespace Workshop_Compiler ;

// ignore unused-function warnings in this source file
#pragma clang diagnostic ignored "-Wunused-function"

// the grammar we are recognising
// rules containing text literals are written using the matching tk_* or tg_* names
// TERM: DEFINITION
// program: declarations statement tk_eoi
// declarations: declaration*
// declaration: vardec | function
// vardec: tk_var tk_identifier tk_identifier tk_semi
// function: tk_identifier tk_identifier tk_lrb parameters? tk_rrb functionBody
// parameters: parameter (tk_comma parameter)*
// parameter: tk_identifier tk_identifier
// functionBody: tk_lcb vardec* statement* tk_rcb
// statement:  whileStatement | letStatement | returnStatement | statementSequence | tryStatement
// whileStatement: tk_while tk_lrb expression tk_rrb statement
// letStatement: tk_let tk_identifier tk_assign expression tk_semi
// returnStatement: tk_return expression? tk_semi
// statementSequence: tk_lcb statement* tk_rcb
// tryStatement: tk_try statement tk_catch tk_lrb tk_identifier tk_identifier tk_rrb statement
// expression: term (tg_operator term)?
// term: tk_identifier | call | tk_integer
// call: tk_call tk_identifier tk_lrb expressions? tk_rrb
// expressions: expression (tk_comma expression)*
//
// Token groups for use with have()/have_next()/mustbe()/did_not_find():
// tg_starts_declaration - matches any token that can start a declaration
// tg_starts_statement - matches any token that can start a statement
// tg_starts_expressions - matches any token that can start expressions
// tg_starts_expression - matches any token that can start expression
// tg_operator - matches any token that can be used as an operator in the rule expression
// tg_starts_term - matches any token that can start a term


// every parse function returns an abstract syntax tree representing what it has parsed
static ast parseProgram() ;
static ast parseDeclarations() ;
static ast parseDeclaration() ;
static ast parseVarDec() ;
static ast parseFunction() ;
static ast parseFunctionBody() ;
static ast parseParameters() ;
static ast parseParameter() ;
static ast parseStatement() ;
static ast parseWhileStatement() ;
static ast parseLetStatement() ;
static ast parseReturnStatement() ;
static ast parseStatementSequence() ;
static ast parseTryStatement() ;
static ast parseExpressions() ;
static ast parseExpression() ;
static ast parseTerm() ;
static ast parseCall() ;

// note: we have added push/pop_error_context() calls so that 
//       you can see part of the call chain when an error occurs
// note: in a declaration the first identifier is the variable type


// symbol table creation / deletion functions
static void push_symbol_table(string segment) ;
static void pop_symbol_table() ;

// *****           DO NOT EDIT THE CODE ABOVE           *****


/////////////////////////////////////////////////////////////////////////////
// ***** COMPLETE THE CODE BELOW TO COMPLETE THE PARSER *****

// In each parse_*() function write code to correctly initialise
// the variables passed to the tree node create*() function



// program: declarations statement tk_eoi
static ast parseProgram()
{
    push_error_context("parseProgram()") ;

    // a declaration node and a statement node
    ast decls = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    decls = parseDeclarations();
    stat = parseStatement();
    mustbe(tk_eoi);

    // return a program node
    ast ret = create_program(decls,stat) ;
    pop_error_context() ;
    return ret ;
}

// declarations: declaration*
static ast parseDeclarations()
{
    push_error_context("parseDeclarations()") ;

    // a vector of declaration nodes
    vector<ast> decls ;

    // add parsing code here ...
    while(have(tg_starts_declaration)) 
    {
        decls.push_back(parseDeclaration());
    }

    // return a declarations node
    ast ret = create_declarations(decls) ;
    pop_error_context() ;
    return ret ;
}

// declaration: vardec | function
static ast parseDeclaration()
{
    push_error_context("parseDeclaration()") ;

    // a declaration node
    ast decl = nullptr ;

    // add parsing code here ...
    if(have(tk_var))
    {
        decl = parseVarDec();
    }
    if(have(tk_identifier))
    {
        decl = parseFunction();
    }

    // return the declaration node
    pop_error_context() ;
    return decl ;
}

// declare a variable of the given type and return the variable's tree node
static ast declare_variable(Token identifier,Token type) ;

// vardec: tk_var tk_identifier tk_identifier tk_semi
// the first identifier is the type
static ast parseVarDec()
{
    push_error_context("parseVarDec()") ;

    // a variable node returned by declare_variable()
    ast var = nullptr ;

    // add parsing code here ...
    mustbe(tk_var);
    Token type = mustbe(tk_identifier);
    var = declare_variable(mustbe(tk_identifier),type);
    mustbe(tk_semi); 

    // return a declaration node
    ast ret = create_declaration(var) ;
    pop_error_context() ;
    return ret ;
}

// function: tk_identifier tk_identifier tk_lrb parameters? tk_rrb functionBody
// the first identifer is the function's return type
// push/pop_symbol_table() calls added so parameters only exist inside the function being parsed
static ast parseFunction()
{
    push_error_context("parseFunction()") ;
    push_symbol_table("argument") ;

    string rtype = "" ;                 // the return type
    string name = "" ;                  // the function name
    ast parameters = nullptr ;          // the parameters
    ast body = nullptr ;                // the body

    // add parsing code here ...
    rtype = token_spelling(mustbe(tk_identifier));
    name = token_spelling(mustbe(tk_identifier));
    
    mustbe(tk_lrb);
    if(have(tk_identifier))
    {
        parameters = parseParameters();
    }
    mustbe(tk_rrb);
    body = parseFunctionBody();
    

    // return a function node wrapped in a declaraion node
    ast ret = create_declaration(create_function(rtype,name,parameters,body)) ;
    pop_symbol_table() ;
    pop_error_context() ;
    return ret ;
}

// functionBody: tk_lcb vardec* statement* tk_rcb
// push/pop_symbol_table() calls added so variables only exist inside the function being parsed
static ast parseFunctionBody()
{
    push_error_context("parseFunctionBody()") ;
    push_symbol_table("local") ;

    vector<ast> vardecs ;               // the local variables
    vector<ast> statements ;            // the body 

    // add parsing code here ...
    mustbe(tk_lcb);
    while(have(tk_var))
    {
        vardecs.push_back(parseVarDec());

    }
    while(have(tg_starts_statement))
    {
        statements.push_back(parseStatement());
    }
    mustbe(tk_rcb);

    // return a function body node
    ast ret = create_function_body(create_declarations(vardecs),create_statements(statements)) ;
    pop_symbol_table() ;
    pop_error_context() ;
    return ret ;
}

// To declare a variable of the given type and return the variable's tree node, use:
// ast declare_variable(Token identifier,Token type) ;

// parameters: parameter (tk_comma parameter)*
static ast parseParameters()
{
    push_error_context("parseParameters()") ;
    push_symbol_table("argument") ;

    // vector of declaration nodes
    vector<ast> parameters ;

    // add parsing code here ...
    parameters.push_back(parseParameter());
    while(have(tk_comma))
    {
        mustbe(tk_comma);
        parameters.push_back(parseParameter());
    }

    // return a declarations node
    ast ret = create_declarations(parameters) ;
    pop_error_context() ;
    return ret ;
}

// parameter: tk_identifier tk_identifier
// the first identifier is the type
static ast parseParameter()
{
    push_error_context("parseParameter()") ;

    // a variable node returned by declare_variable
    ast var = nullptr ;

    // add parsing code here ...
    Token type = mustbe(tk_identifier);
    var = declare_variable(mustbe(tk_identifier),type);

    // return a declaration node
    ast ret = create_declaration(var) ;
    pop_error_context() ;
    return ret ;
}

// statement:  whileStatement | letStatement | statementSequence | returnStatement | tryStatement
static ast parseStatement()
{
    push_error_context("parseStatement()") ;

    // a statement node
    ast stat = nullptr ;

    // add parsing code here ...
    if(have(tk_while))
    {
        stat = parseWhileStatement();
    }
    if(have(tk_let))
    {
        stat = parseLetStatement();
    }
    if(have(tk_lcb))
    {
        stat = parseStatementSequence();
    }
    if(have(tk_return))
    {
        stat = parseReturnStatement();
    }
    if(have(tk_try))
    {
        stat = parseTryStatement();
    }

    // return a statement node
    stat = create_statement(stat) ;
    pop_error_context() ;
    return stat ;
}

// whileStatement: tk_while tk_lrb expression tk_rrb statement
static ast parseWhileStatement()
{
    push_error_context("parseWhileStatement()") ;

    // an expression node and a statement node
    ast expr = nullptr ;
    ast stat = nullptr ;

    // add parsing code here ...
    mustbe(tk_while);
    mustbe(tk_lrb);
    expr = parseExpression();
    mustbe(tk_rrb);
    stat = parseStatement();

    // return a while node
    ast ret = create_while(expr,stat) ;
    pop_error_context() ;
    return ret ;
}

// return the variable's tree node or report a fatal error if it was not previously declared
static ast lookup_variable(Token identifier) ;

// letStatement: tk_let tk_identifier tk_assign expression tk_semi
static ast parseLetStatement()
{
    push_error_context("parseLetStatement()") ;

    // a variable node and an expression node
    ast id = nullptr ;
    ast expr = nullptr ;

    // add parsing code here ...
    mustbe(tk_let);
    id = lookup_variable(mustbe(tk_identifier));
    mustbe(tk_assign);
    expr = parseExpression();
    mustbe(tk_semi);

    // return a let node
    ast ret = create_let(id,expr) ;
    pop_error_context() ;
    return ret ;
}

// statementSequence: tk_lcb statement* tk_rcb
static ast parseStatementSequence()
{
    push_error_context("parseStatementSequence()") ;

    // a vector of statemnt nodes
    vector<ast> seq ;

    // add parsing code here ...
    mustbe(tk_lcb);
    while(have(tg_starts_statement))
    {
        seq.push_back(parseStatement());
    }
    mustbe(tk_rcb);

    // return a statements node
    ast ret = create_statements(seq) ;
    pop_error_context() ;
    return ret ;
}

// returnStatement: tk_return expression? tk_semi
static ast parseReturnStatement()
{
    push_error_context("parseReturnStatement()") ;

    // an expression node
    ast expr = nullptr ;

    // add parsing code here ...
    mustbe(tk_return);
 
    // if there is an expression
    if(have(tg_starts_expression))
    {
        expr = parseExpression();
        // return a return expression node
        ast ret = create_return_expr(expr) ;
        mustbe(tk_semi);
        pop_error_context() ;
        return ret ;
    }
    mustbe(tk_semi);

    // return a return node
    ast ret = create_return() ;
    pop_error_context() ;
    return ret ;
}

// To declare a variable of the given type and return the variable's tree node, use:
// ast declare_variable(Token identifier,Token type) ;

// tryStatement: tk_try statement tk_catch tk_lrb tk_identifier tk_identifier tk_rrb statement
// the first identifier is the type of the exception variable
static ast parseTryStatement()
{
    push_error_context("parseTryStatement()") ;

    // two statement nodes and a variable node
    ast tryS = nullptr ;
    ast evar = nullptr ;
    ast catchS = nullptr ;


    // add parsing code here ...
    mustbe(tk_try);
    tryS = parseStatement();
    mustbe(tk_catch);
    mustbe(tk_lrb);
    Token type = mustbe(tk_identifier);
    evar = declare_variable(mustbe(tk_identifier),type);
    mustbe(tk_rrb);
    catchS = parseStatement();
 
    // return a try node
    ast ret = create_try(tryS,evar,catchS) ;
    pop_error_context() ;
    return ret ;
}

// expressions: expression (tk_comma expression)*
static ast parseExpressions()
{
    push_error_context("parseExpressions()") ;

    // a vector of expression nodes
    vector<ast> exprs ;

    // add parsing code here ...
    exprs.push_back(parseExpression());
    while(have(tk_comma))
    {
        mustbe(tk_comma);
        exprs.push_back(parseExpression());
    }

    // return an expressions node
    ast ret = create_expressions(exprs) ;
    pop_error_context() ;
    return ret ;
}

// expression: term (tg_operator term)?
static ast parseExpression()
{
    push_error_context("parseExpression()") ;

    // two term nodes and a Token
    ast lhs = nullptr ;
    ast rhs = nullptr ;
    Token infix_op = nullptr ;

    // add parsing code here ...
    lhs = parseTerm();

    // if the expression has an infix operator
    if(have(tg_operator))
    {
        infix_op = mustbe(tg_operator);
        rhs = parseTerm();
        // return an infix expression node
        ast ret = create_expression(create_infix_op(lhs,token_spelling(infix_op),rhs)) ;
        pop_error_context() ;
        return ret ;
    }

    // return a single term expression node
    ast ret = create_expression(lhs) ;
    pop_error_context() ;
    return ret ;
}

// the following helper functions can be used in parseTerm()

// turn an integer token into an ast node
static ast integer_to_ast(Token integer)
{
    return create_int(token_ivalue(integer)) ;
}

// To return the variable's tree node or report a fatal error if it was not previously declared, use:
// ast lookup_variable(Token identifier) ;

// term: tk_identifier | call | tk_integer
static ast parseTerm()
{
    push_error_context("parseTerm()") ;

    // a variable node, a call node or an integer node
    ast term = nullptr ;

    // add parsing code here ...
    if(have(tk_identifier))
    {
        term = lookup_variable(mustbe(tk_identifier));
    }
    if(have(tk_call))
    {
        term = parseCall();
    }
    if(have(tk_integer))
    {
        term = integer_to_ast(mustbe(tk_integer));
    }

    // return the term parsed wrapped in a term node
    ast ret = create_term(term) ;
    pop_error_context() ;
    return ret ;
}

// call: tk_call tk_identifier tk_lrb expressions? tk_rrb
// the identifier is a function name, do not use lookup_variable()
static ast parseCall()
{
    push_error_context("parseCall()") ;

    // a string and an expressions node
    string fn = "" ;
    ast exprs = nullptr ;

    // add parsing code here ...
    mustbe(tk_call);
    fn = token_spelling(mustbe(tk_identifier));
    mustbe(tk_lrb);
    if(have(tg_starts_expressions))
    {
        exprs = parseExpressions();
    }
    mustbe(tk_rrb);
    // return a call node
    ast ret = create_call(fn,exprs) ;
    pop_error_context() ;
    return ret ;
}


/////////////////////////////////////////////////////////////////////////////
// ***** DO NOT CHANGE CODE AFTER THIS POINT *****


// ****  SYMBOL TABLES  ****

// scope levels each have their own symbol tables, segment and next free location
// the offset to the next free location must start at 0
class scope
{
public:
    scope(string segment) : segment(segment), offset(0)
    {
        symbol_table = create_variables() ;
    }

    symbols symbol_table ;      // symbol table for this scope level
    string segment ;            // name of segment where variables are stored
    int offset ;                // offset into the segment where the next variable is stored
} ;

// symbol table management so we can lookup declared variables
static vector<scope> *scopes ;

// initialise the symbol tables
static void initialise_symbol_tables()
{
    scopes = new vector<scope>() ;
}

// push a new symbol table onto the stack of symbol tables
static void push_symbol_table(string segment)
{
    scopes->push_back(scope(segment)) ;
}

// pop a symbol table from the stack of symbol tables
static void pop_symbol_table()
{
    // first check there is something to pop
    int ntables = scopes->size() ;
    if ( ntables == 0 )
    {
        fatal_error(0,"attempted to pop a symbol table from an empty stack of symbol tables") ;
    }

    // remove the top scope from the stack and delete it
    symbols popped = scopes->back().symbol_table ;
    delete_variables(popped) ;
    scopes->pop_back() ;
}

// this function adds an identifier to the top symbol table on the symbol table stack
// it allocates the variable the next free offset in the current segment
// it returns a tree node representing the variable
static ast declare_variable(Token identifier,Token type)
{
    scope current = scopes->back() ;

    string varname = token_spelling(identifier) ;
    string tname = token_spelling(type) ;
    st_variable var(varname,tname,current.segment,scopes->back().offset++) ;

    if ( !insert_variables(current.symbol_table,varname,var) ) // it is an error to declare something twice
    {
        fatal_error(0,"\n" + token_context(identifier) + "Variable:  " + varname +  " has already been declared") ;
    }

    return create_variable(var.name,var.segment,var.offset,var.type) ;
}

// lookup the identifier, it is an error if it was not previously declared
// it returns a tree node representing the variable
static ast lookup_variable(Token identifier)
{
    // search symbol tables from top to bottom of the symbol table stack
    string varname = token_spelling(identifier) ;
    for ( int i = scopes->size() - 1 ; i >= 0 ; i-- )
    {
        st_variable var = lookup_variables(scopes->at(i).symbol_table,varname) ;
        if ( var.name == varname )
        {
            return create_variable(var.name,var.segment,var.offset,var.type) ;
        }
    }

    // variables not found - report a fatal error - the return is just so that the function compiles
    fatal_error(0,"\n" + token_context(identifier) + "Found undeclared variable:  " + varname) ;
    return nullptr ;
}

// **** MAIN PROGRAM ****

// main program for workshop 10/11 parser to XML
int main(int argc,char **argv)
{
    config_output(iob_immediate) ;      // make all output and errors appear immediately
    config_errors(iob_immediate) ;

    initialise_symbol_tables() ;        // initialise symbol tables
    push_symbol_table("static") ;       // push a new symbol table to hold global declarations

    next_token() ;                      // read first token to initialise tokeniser
    ast program = parseProgram() ;      // parse a Program to get a parse tree

    pop_symbol_table() ;                // delete the symbol table

    ast_print_as_xml(program,4) ;       // write the tree to standard output formatted as XML

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

