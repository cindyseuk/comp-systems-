// a skeleton implementation of a tokeniser

#include "tokeniser-extras.h"

// to shorten the code
using namespace std ;

/////////////////////////////////////////////////////////////////////////

namespace Assignment_Tokeniser
{

    static TokenKind new_token_kind ;


     static void parse_extra(TokenKind kind){
        new_token_kind = kind ;
        nextch() ;
    }


    //function to parse whitespaces 
    static void parse_wspace(TokenKind kind){
        parse_extra(kind);
    }

    //function to parse identifiers
    static void parse_identifier() {
        new_token_kind = tk_identifier;
        do nextch(); while(c_have(cg_extends_identifier));

    }

    //function to parse integers
    static void parse_integer(){
        new_token_kind = tk_integer;
        if (c_have(cg_digit19)){
            do nextch(); while (c_have(cg_digit));{
            } 
        } else {
            c_have('0'); nextch();
        }
    }

    static void parse_eol_comment(){
        new_token_kind = tk_eol_comment;
        if (c_have_next('/')){
            c_mustbe('/');
        } 
        do nextch(); while (c_have(cg_eol_comment_char));
        c_mustbe('\n');
    }

    





    //function to parse symbols 
    static void parse_symbol(TokenKind kind){
        parse_extra(kind); 
    }

    //function to parse eol comment 


    static void parse_more_symbols(){

        if (c_have('!')){
            nextch(); c_mustbe('=');
            new_token_kind = tk_not_eq;
        } 
        if (ch == '-'){
           nextch(); c_mustbe('=');
            new_token_kind = tk_sub_assign;
        } 

        if (ch == '+'){
            nextch(); c_mustbe('=');
            new_token_kind = tk_add_assign;
        } 

        if (ch == '*'){
            nextch(); c_mustbe('=');
            new_token_kind = tk_mult_assign;
        } 

        if (ch == '/'){
            nextch(); c_mustbe('=');
            new_token_kind = tk_div_assign;
        } 

        if (ch == '='){
            nextch(); c_mustbe('=');
            new_token_kind = tk_eq;
        } 

        if (ch == '<'){
            nextch(); c_have_next('<');
            new_token_kind = tk_lshift;
            if (c_have('<')){
                new_token_kind = tk_lshift_l;
            }
        } 

        if (ch == '>'){
            nextch(); c_have_next('>');
            new_token_kind = tk_rshift;
            if (c_have('>')){
                new_token_kind = tk_rshift_l;
            }
        } 

    }
//switch t\

    // * scientific ::= integer fraction? exponent
    //   - fraction ::= '.' digit*
    //   - exponent ::= ('e'|'E') ('+'|'-')? digits0123
    //   - digits0123 ::= zero | digits123
    //   - digits123 ::= digit19 digit? digit?










/*
    static void parse_scientific(){

        c_mustbe(tk_integer);

        if (c_have('.') || )
    }

    static void parse_number(){

    }

*/
 


    // return the next Token object by reading more of the input
    // you must read input using the nextch() function
    // the last character read is in the static variable ch
    // always read one character past the end of the token being returned
    Token next_token()
    {
        switch(ch)      // ch is always the next char to read
        {
            case ' ':
                parse_wspace(tk_space); break;
            case '\n':
                parse_wspace(tk_newline); break;

            case 'a'...'z':
            case 'A'...'Z':
            case '$':
           // case ''
                parse_identifier(); break;

            case '0':
            case '1'...'9':
                parse_integer(); break;

            case '@':
                parse_symbol(tk_at); break;
            case '.':
                parse_symbol(tk_stop); break;
            case '{':
                parse_symbol(tk_lcb); break;
            case '}':
                parse_symbol(tk_rcb); break;               
            case '(':
                parse_symbol(tk_lrb); break;
            case ')':
                parse_symbol(tk_rrb); break;
            case '[':
                parse_symbol(tk_lsb); break;               
            case ']':
                parse_symbol(tk_rsb); break;  

            case '-':
                parse_more_symbols(); break;
            case '+':
                parse_more_symbols(); break;
            case '*':
                 parse_more_symbols(); break;
            case '/':
                 parse_more_symbols(); break;

            case '!': //check for !=
                parse_more_symbols(); break;

            case '=':
                parse_more_symbols(); break;

            case '<':
                parse_more_symbols(); break;

            case '>':
                parse_more_symbols(); break;


                        // this should follow the style used in the workshops ...
                        // but remember that the token grammar is different in this assignment
                        //
                        // add additional case labels here for characters that can start tokens
                        // call a parse_*() function to parse the token
                        //

                        // End of Inptut
        case EOF:
            new_token_kind = tk_eoi; break ;

        default:
            c_did_not_find(cg_start_of_token) ;
        }

        Token token = new_token(new_token_kind) ;

                        // before returning a token check if the kind or spelling needs updating
                        // ...

        return token ;
    }
}
