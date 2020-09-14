/*
Subject Name: Computer Systems
Student: Vandit Jyotindra Gajjar
Student ID: a1779153
Semester: 2
Year: 2020
Assignment: 1
*/

// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    static TokenKind new_token_kind ;

    static void parse_extra(TokenKind kind)
    {
        new_token_kind = kind ;
        nextch() ;
    }

    //Function to parse white space and newline tokens
    static void parse_wspace(TokenKind kind) 
    {
            parse_extra(kind) ;
    }

    //Function to parse integer/double tokens
    static void parse_integer() 
    {
        new_token_kind = tk_integer;
        do 
        {
            nextch() ;
            if(ch=='0') 
            {
                //To replace 000 with 0
                if(c_have('0')) 
                {
                    break;
                }
            }
        } while ( c_have(cg_digit) ) ;
        if(c_have(cg_start_of_fraction)) 
        {
            c_mustbe(cg_start_of_fraction);
            do 
            {
            nextch() ;
            if(ch=='0') 
            {
                if(c_have('0')) 
                {
                    break;
                }
            }
        } while ( c_have(cg_digit) ) ;
        c_mustbe(cg_start_of_exponent);
        }
        
    }

    //Function to parse identifier tokens
    static void parse_identifier() 
    {
        new_token_kind = tk_identifier;
        do nextch() ; while ( c_have(cg_extends_identifier) ) ;
    }

    //Function to parse keyword tokens
    static Token parse_keyword(Token token) 
    {
        if(token_spelling(token) == "this")
        {
            set_token_kind(token,tk_this);
        }
        if(token_spelling(token) == "done") 
        {
            set_token_kind(token,tk_done);
        }
        if(token_spelling(token) == "if-goto") 
        {
            set_token_kind(token,tk_if_goto);
        }
        if(token_spelling(token) == "procedure") 
        {
            set_token_kind(token,tk_procedure);
        }
        if(token_spelling(token) == "while") 
        {
            set_token_kind(token,tk_while);
        }
        return token;
    }

    //Function to parse eol comment tokens
    static void parse_eol() 
    {
        new_token_kind = tk_eol_comment;
        do nextch() ; while ( c_have(cg_eol_comment_char) ) ;
        c_mustbe('\n');
    }

    //Function to parse adhoc comment tokens 
    static void parse_adhoc() 
    {
        new_token_kind = tk_adhoc_comment;
        do 
        {
            nextch() ;
            if(c_have('*')) 
            {
                c_mustbe('*');
                if(c_have('/'))
                {
                    c_mustbe('/');
                    break;
                }
            }
        } while ( c_have(cg_adhoc_comment_char) ) ;
    }

    //Function to identify the symbol token and match it with the grammar
    static void parse_symbol() 
    {
        switch(ch) 
        {
            case '.': 
            {
                nextch();
                new_token_kind = tk_stop;
                break;
            }
            case '@': 
            {
                nextch();
                new_token_kind = tk_at;
                break;
            }
            case '{': 
            {
                nextch();
                new_token_kind = tk_lcb;
                break;
            }
            case '}': 
            {
                nextch();
                new_token_kind = tk_rcb;
                break;
            }
            case '(': {
                nextch();
                new_token_kind = tk_lrb;
                break;
            }
            case ')': 
            {
                nextch();
                new_token_kind = tk_rrb;
                break;
            }
            case '[': 
            {
                nextch();
                new_token_kind = tk_lsb;
                break;
            }
            case ']': 
            {
                nextch();
                new_token_kind = tk_rsb;
                break;
            }
            case '-': 
            {
                nextch();
                c_mustbe('=');
                new_token_kind = tk_sub_assign;
                break;
            }
            case '+': 
            {
                nextch();
                c_mustbe('=');
                new_token_kind = tk_add_assign;
                break;
            }
            case '*': 
            {
                nextch();
                c_mustbe('=');
                new_token_kind = tk_mult_assign;
                break;
            }
            case '/': 
            {
                nextch();
                if(c_have('/')) 
                {
                    c_mustbe('/');
                    parse_eol();
                }
                else if(c_have('*')) 
                {
                    c_mustbe('*');
                    parse_adhoc();
                }
                else 
                {
                    c_mustbe('=');
                    new_token_kind = tk_div_assign;
                }
                break;
            }
            case '!': 
            {
                nextch();
                c_mustbe('=');
                new_token_kind = tk_not_eq;
                break;
            }
            case '=': 
            {
                nextch();
                c_mustbe('=');
                new_token_kind = tk_eq;
                break;
            }
            case '<': 
            {
                nextch();
                c_mustbe('<');
                if(c_have('<')) 
                {
                    c_mustbe('<');
                    new_token_kind = tk_lshift_l;
                }
                else 
                {
                    new_token_kind = tk_lshift;    
                }
                break;
            }
            case '>': 
            {
                nextch();
                c_mustbe('>');
                if(c_have('>')) 
                {
                    c_mustbe('>');
                    new_token_kind = tk_rshift_l;
                }
                else 
                {
                    new_token_kind = tk_rshift;    
                }
                break;
            }

        }
    }
    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    Token next_token()
    {
        switch(ch)      // ch is always the next char to read
        {
                        // this should follow the style used in the workshops ...
                        // but remember that the token grammar is different in this assignment
                        //
                        // add additional case labels here for characters that can start tokens
                        // call a parse_*() function to parse the token
                        //
        //space token
        case ' ':
                parse_wspace(tk_space);
                break;
        case '\n':
                parse_wspace(tk_newline);
                break;

        // case to identify a number
        case '0'...'9':
                parse_integer();
                break;

        // cases to determine identifiers
        case 'a'...'z':
                parse_identifier();
                break;
        case 'A'...'Z':
                parse_identifier();
                break;                 
        case '$':
                parse_identifier();
                break;

        //cases to determine symbol tokens               
        case '@':
                parse_symbol();
                break;
        case '-':
                parse_symbol();
                break;
        case '=':
                parse_symbol();
                break;
        case '+':
                parse_symbol();
                break;
        case '*':
                parse_symbol();
                break;
        case '/':
                parse_symbol();
                break;
        case '!':
                parse_symbol();
                break;
        case '<':
                parse_symbol();
                break;
        case '>':
                parse_symbol();
                break;
        case '{':
                parse_symbol();
                break;
        case '}':
                parse_symbol();
                break;
        case '[':
                parse_symbol();
                break;
        case ']':
                parse_symbol();
                break;
        case '.':
                parse_symbol();
                break;
        case '(':
                parse_symbol();
                break;
        case ')':
                parse_symbol();
                break;
        
        case EOF:
            new_token_kind = tk_eoi ; // End of Inptut

            break ;
        default:
            c_did_not_find(cg_start_of_token) ;
        }

        Token token = new_token(new_token_kind) ;

        //if the identifier is a keyword
        if(keyword_or_identifier(token_spelling(token)) != tk_identifier) 
        {
            token = parse_keyword(token);
        }

        //code to modify the spelling of the token for eol comment
        if(token_kind(token) == tk_eol_comment) 
        {
            string modifySpelling = token_spelling(token);
            modifySpelling = modifySpelling.erase(0,2);
            modifySpelling.pop_back();
            set_token_spelling(token,modifySpelling);
        }

        //code to modify the spelling of the token for adhoc comment
        if(token_kind(token) == tk_adhoc_comment) 
        {
            string modifySpelling = token_spelling(token);
            modifySpelling = modifySpelling.erase(0,2);
            modifySpelling.pop_back();
            modifySpelling.pop_back();
            set_token_spelling(token,modifySpelling);
        }
        return token ;
    }
}
