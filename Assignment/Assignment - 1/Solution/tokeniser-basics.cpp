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
#include <string>
#include <iostream>

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    // the current input character
    // the eof marker can be confused with a legal character but not one accepted by our tokeniser
    char ch ;

    int line_number = 0;
    int column_number = 0;
    int old_col_value = 0;
    int char_number = 0;
    string data[10000];

    // to create a new token we need the characters read since the last token was created
    // the tokeniser always reads one character past a token so ch is never part of a new token
    // when nextch() is called the old ch value must be part of the token currently being parsed
    // this minimal implementation just remembers the spelling of the current token
    static string spelling ;

    // create a new token using characters remembered since the last token was created
    // in the final submission tests new_token() will require the correct line and column numbers
    // this will require keeping an entire history of the input, not just the latest spelling
    // NOTE: the first character in the input is in column 1 of line 1
    Token new_token(TokenKind kind)
    {
        // create a new token object and reset spelling
        
        //generate token for the newline character
        if(kind==tk_newline) 
        {
            Token token = new_token(kind,spelling,line_number-1,old_col_value);
            spelling = "";
            return token;   
        }

        //generate token for the eol comments
        if(kind==tk_eol_comment) 
        {
            Token token = new_token(kind,spelling,line_number-1,column_number);
            spelling = "";
            return token;   
        }

        //generate token for the adhoc comments
        if(kind==tk_adhoc_comment) 
        {
            Token token = new_token(kind,spelling,line_number-2,column_number-3);
            spelling = "";
            return token;   
        }

        //generate new other tokens
        Token token = new_token(kind,spelling,line_number,column_number-spelling.length()) ;

        // reset the spelling
        spelling = "" ;
        return token ;
    }

    //insert line number with spaces
    string insertLineNum(string number) 
    {
        string output = "";
        if (number.length() == 1)
        {
            output = "   " + number;
        }

        else if (number.length() == 2)
        {
            output = "  " + number;
        }

        else if (number.length() == 3)
        {
            output = " " + number;
        }

        output+= ": ";
        return output;
    }

    // generate a context string for the given token
    // all lines are shown after the preprocssing of special characters
    // it shows the line before the token, with a '$' marking the end of line,
    // the line containing the token up to the end of the token, and
    // a line of spaces ending with a ^ under the token's first character to mark its position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ": ", eg "  45: "
    // NOTE: if a token includes newline character(s), only display the token before the first newline followed by a '$'
    // NOTE: this function is not required for the milestone submission
    // NOTE: it is a good idea to build a data structure to remember where each line starts
    string token_context(Token token)
    {
        string context = "";
        int startLine = token_line(token); //line number of the token
        int startColumn = token_column(token); //column number of the token
        if(startLine!=1) 
        {
            context+=insertLineNum(std::to_string(startLine-1));
            context+=data[startLine-1];
            context.pop_back();
            if(data[startLine-1][data[startLine-1].length()-1]=='\n') 
            {
                //replace newline character with '$' symbol
                context+='$';
                context+='\n';
            }
        }

        context+=insertLineNum(std::to_string(startLine));
        context+=data[startLine].substr(0,startColumn-1);

        if(token_kind(token)==tk_eol_comment) 
        {
            context+= "//" + token_spelling(token) + '$';
        } 

        else if (token_kind(token) == tk_adhoc_comment) 
        {
            string temp = token_spelling(token);
            for (int i = 0; i < temp.length(); i++) 
            {
                if (temp[i] == '\n') 
                {
                    temp[i] = '$';
                    temp = temp.substr(0, i + 1);
                    context += "/*" + temp;
                    goto out;                                     // exit if adhoc comment is more than 1 line
                }
            }
            context += "/*" + temp + "*/";
        }

        else
        { 
            context+=token_spelling(token);
        }

        if(context[context.length()-1]=='\n') 
        {
            //if token read is a newline character
            context.pop_back();
            context+='$';
        }
        out:
        context+='\n';

        //add spaces to accomodate '^' below  the start of token
        for (int i = 0; i < startColumn+5; i++)
        {
            context+=' ';
        }

        context += "^\n";
        return context ;
    }

    // read next character if not at the end of input and update the line and column numbers
    // additional code will be required here to handle preprocessing of '\t' and '\r'
    // in some cases you may wish to remember a character to use next time instead of calling read_char()
    bool char_read = true;
    int count = 0;
    char remembered;
    void nextch()
    {
        if ( ch == EOF ) return ;           // stop reading once we have read EOF

        //move to next line and reset the column
        if(ch=='\n') 
        {
            line_number++;
            old_col_value = column_number;
            column_number = 0;
        }

        spelling += ch ;                    // remember the old ch, it is part of the current token being parsed
        
        if(char_read)
        {
            ch = read_char() ;              // read the next character
        }                  
        column_number++;

        //condition to check when to read the new character
        if(count==3) 
        {
            count =0;
            char_read = true;
        }

        //handling '/t' character
        if((ch=='\t') || !char_read) 
        {
                count ++;
                char_read = false;
                ch = ' ';            
        }

        //handling '/t' character
        if(ch=='\r') 
        {
            remembered = read_char();
            if(remembered =='\n') 
            {
                ch = remembered;
            }

            else 
            {
                ch = '\n';
                count =3;
                char_read = false;
            }
        }
        data[line_number] +=ch;

    }

    // initialise the tokeniser
    void initialise_tokeniser()
    {
        ch = '\n' ;                         // initialise ch to avoid accidents
        nextch() ;                          // make first call to nextch to initialise ch using the input
        spelling = "" ;                     // discard the initial '\n', it is not part of the input
    }
}
