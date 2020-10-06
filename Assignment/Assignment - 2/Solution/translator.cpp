// convert Hack VM Code into Hack Assembler

#include "iobuffer.h"
#include "tokeniser.h"
#include "assembler-internal.h"
#include <string>

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

////////////////////////////////////////////////////////////////
/************     MODIFY CODE BETWEEN HERE     **************/

string className ="";
string funcName = "";
int eqNum = 0;
int ltNum = 0;
int gtNum = 0;
int funcNum = 0;
// translate vm operator command into assembly language
static void translate_vm_operator(TokenKind the_op)
{
    start_of_vm_operator_command(the_op) ;
    if(the_op == tk_add) {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D+M");
    }
    else if(the_op == tk_sub) {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=M-D");
    }
    else if(the_op == tk_or) {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D|M");
    }
    else if(the_op == tk_and) {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D&M");
    }
    else if(the_op == tk_not) {
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=!M");
    }
    else if(the_op == tk_neg) {
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=-M");
    }
    else if(the_op == tk_eq) {
        eqNum++;
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("A=A-1");
        output_assembler("D=M");
        output_assembler("A=A+1");
        output_assembler("D=D-M");
        output_assembler("@SP");
        output_assembler("M=M-1");
        output_assembler("@" + funcName + "$EQ" + to_string(eqNum));
        output_assembler("D;JEQ");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=0");
        output_assembler("@" + funcName + "$ENDEQ" + to_string(eqNum));
        output_assembler("0;JMP");
        output_assembler("(" + funcName + "$EQ" + to_string(eqNum) + ")");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=-1");
        output_assembler("(" + funcName + "$ENDEQ" + to_string(eqNum) + ")");
        output_assembler("@SP");
        output_assembler("M=M+1");
    }
    else if(the_op == tk_lt) {
        ltNum++;
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("A=A-1");
        output_assembler("D=M");
        output_assembler("A=A+1");
        output_assembler("D=D-M");
        output_assembler("@SP");
        output_assembler("M=M-1");
        output_assembler("@" + funcName + "$LT" + to_string(ltNum));
        output_assembler("D;JLT");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=0");
        output_assembler("@" + funcName + "$ENDLT" + to_string(ltNum));
        output_assembler("0;JMP");
        output_assembler("(" + funcName + "$LT" + to_string(ltNum) + ")");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=-1");
        output_assembler("(" + funcName + "$ENDLT" + to_string(ltNum) + ")");
        output_assembler("@SP");
        output_assembler("M=M+1");
        
    }
    else if(the_op == tk_gt) {
        gtNum++;
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("A=A-1");
        output_assembler("D=M");
        output_assembler("A=A+1");
        output_assembler("D=D-M");
        output_assembler("@SP");
        output_assembler("M=M-1");
        output_assembler("@" + funcName + "$GT" + to_string(gtNum));
        output_assembler("D;JGT");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=0");
        output_assembler("@" + funcName + "$ENDGT" + to_string(gtNum));
        output_assembler("0;JMP");
        output_assembler("(" + funcName + "$GT" + to_string(gtNum) + ")");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=-1");
        output_assembler("(" + funcName + "$ENDGT" + to_string(gtNum) + ")");
        output_assembler("@SP");
        output_assembler("M=M+1");
        
    }
    else if(the_op == tk_return) {
        output_assembler("@LCL");
        output_assembler("D=M");
        output_assembler("@R13");
        output_assembler("M=D");
        output_assembler("@5");
        output_assembler("A=D-A");
        output_assembler("D=M");
        output_assembler("@R14");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("D=M");
        output_assembler("@ARG");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@ARG");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("M=A");
        output_assembler("M=D+1");

        output_assembler("@R13");
        output_assembler("AMD=M-1");
        output_assembler("D=M");
        output_assembler("@THAT");
        output_assembler("M=D");

        output_assembler("@R13");
        output_assembler("AMD=M-1");
        output_assembler("D=M");
        output_assembler("@THIS");
        output_assembler("M=D");

        output_assembler("@R13");
        output_assembler("AMD=M-1");
        output_assembler("D=M");
        output_assembler("@ARG");
        output_assembler("M=D");

        output_assembler("@R13");
        output_assembler("AMD=M-1");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");

        output_assembler("@R14");
        output_assembler("A=M");
        output_assembler("0;JMP");
    }

    // ... your code goes here ...

    end_of_vm_command() ;
}

// translate vm jump command into assembly language
static void translate_vm_jump(TokenKind jump, string label)
{
    start_of_vm_jump_command(jump,label) ;
    if(jump ==tk_goto) {
        output_assembler("@" + funcName + "$" + label);
        output_assembler("0;JMP");    
    }
    else if(jump == tk_if_goto) {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("@" + funcName + "$" + label);
        output_assembler("D;JNE");
    }
    else if(jump == tk_label) {
        output_assembler("(" + funcName + "$" + label + ")");
    }
    // ... your code goes here ...

    end_of_vm_command() ;
}

// translate vm call or function command into assembly language
static void translate_vm_function(TokenKind func, string label, int n)
{
    start_of_vm_func_command(func,label,n) ;
    int temp = n;
    if(func == tk_function) {
        output_assembler("(" + label + ")");
        while(temp>0) {
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=0");
            temp--;
        }
    }
    else if(func == tk_call) {
        funcNum++;
        output_assembler("@" + funcName + "$retAddr" + to_string(funcNum));
        output_assembler("D=A");
        output_assembler("@SP");
        output_assembler("AM=M+1");
        output_assembler("A=A-1");
        output_assembler("M=D");
        output_assembler("@LCL");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("AM=M+1");
        output_assembler("A=A-1");
        output_assembler("M=D");
        output_assembler("@ARG");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("AM=M+1");
        output_assembler("A=A-1");
        output_assembler("M=D");
        output_assembler("@THIS");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("AM=M+1");
        output_assembler("A=A-1");
        output_assembler("M=D");
        output_assembler("@THAT");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("AM=M+1");
        output_assembler("A=A-1");
        output_assembler("M=D");
        int position = n+5;
        output_assembler("@" + to_string(position));
        output_assembler("D=A");
        output_assembler("@R13");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@R13");
        output_assembler("D=D-M");
        output_assembler("@ARG");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");
        output_assembler("@" +label);
        output_assembler("0;JMP");
        output_assembler("(" + funcName + "$retAddr" + to_string(funcNum) + ")");
    }
    

    // ... your code goes here ...

    end_of_vm_command() ;
}

// translate vm push or pop command into assembly language
static void translate_vm_stack(TokenKind stack,TokenKind segment,int offset)
{
    start_of_vm_stack_command(stack,segment,offset) ;

    if(stack == tk_push) {
        if(segment == tk_constant) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@SP");
            output_assembler("A=M");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("M=M+1");
        }
        else if (segment == tk_static) {
            output_assembler("@" + className + "." + to_string(offset));
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_local) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@LCL");
            output_assembler("A=D+M");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_argument) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@ARG");
            output_assembler("A=D+M");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_pointer) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@R3");
            output_assembler("A=D+A");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_temp) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@R5");
            output_assembler("A=D+A");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_this) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@THIS");
            output_assembler("A=D+M");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }
        else if(segment == tk_that) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@THAT");
            output_assembler("A=D+M");
            output_assembler("D=M");
            output_assembler("@SP");
            output_assembler("AM=M+1");
            output_assembler("A=A-1");
            output_assembler("M=D");
        }

    }
    else if(stack == tk_pop) {
        if(segment == tk_local) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@LCL");
            output_assembler("D=D+M");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if(segment == tk_argument) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@ARG");
            output_assembler("D=D+M");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if(segment == tk_this) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@THIS");
            output_assembler("D=D+M");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if(segment == tk_that) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@THAT");
            output_assembler("D=D+M");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if(segment == tk_pointer) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@R3");
            output_assembler("D=D+A");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if(segment == tk_temp) {
            output_assembler("@" + to_string(offset));
            output_assembler("D=A");
            output_assembler("@R5");
            output_assembler("D=D+A");
            output_assembler("@R13");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@R13");
            output_assembler("A=M");
            output_assembler("M=D");
        }
        else if (segment == tk_static) {
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");
            output_assembler("@" + className + "." + to_string(offset));
            output_assembler("M=D");
            
        }

    }

    // ... your code goes here ...

    end_of_vm_command() ;
}


/************            AND HERE              **************/
///////////////////////////////////////////////////////////////

// grammar to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void parse_class() ;
static void parse_command() ;
static void parse_op() ;
static void parse_jump() ;
static void parse_func() ;
static void parse_stack() ;

// Note: always read one token after the one recognised

// vm_class ::= command* eoi
static void parse_class()
{
    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    while ( have(tk_vm_command) ) parse_command() ;
    mustbe(tk_eoi) ;

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;
}

// command ::= vm_operator | vm_jump | vm_func | vm_stack
static void parse_command()
{
    if ( have(tk_vm_operator) ) parse_op() ; else
    if ( have(tk_vm_jump) )     parse_jump() ; else
    if ( have(tk_vm_func) )     parse_func() ; else
    if ( have(tk_vm_stack) )    parse_stack() ; else
    mustbe(tk_vm_command) ;
}

// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
static void parse_op()
{
    TokenKind the_op = token_kind(mustbe(tk_vm_operator)) ;

    translate_vm_operator(the_op) ;
}

// vm_jump ::= jump label
// jump command - label, goto label or if-goto label
static void parse_jump()
{
    TokenKind command = token_kind(mustbe(tk_vm_jump)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;

    translate_vm_jump(command,label) ;
}

// vm_func ::= func label number
// call function-name #args or function function-name #locals
static void parse_func()
{
    TokenKind command = token_kind(mustbe(tk_vm_func)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;
    std::size_t found = label.find(".");
    if (found!=std::string::npos)
    {
        className = label.substr(0,found);
    }
    if(command == tk_function) {
        funcName = label;
        eqNum = 0;
        ltNum = 0;
        gtNum = 0;
        funcNum = 0;
    }
    

    translate_vm_function(command,label,number) ;
}

// vm_stack ::= stack segment number
// push segment offset or pop segment offset
static void parse_stack()
{
    TokenKind stack = token_kind(mustbe(tk_vm_stack)) ;
    TokenKind segment = token_kind(mustbe(tk_vm_segment)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_stack(stack,segment,number) ;
}

// main program
int main(int argc,char **argv)
{
    // initialise the tokeniser by reading the first token
    next_token() ;

    // parse a class and output VM code
    parse_class() ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}

