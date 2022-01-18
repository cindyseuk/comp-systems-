// convert Hack VM Code into Hack Assembler

#include "iobuffer.h"
#include "tokeniser.h"
#include "assembler-internal.h"

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

////////////////////////////////////////////////////////////////
/************     MODIFY CODE BETWEEN HERE  
   **************/

 string className ; 
 static string funcName ;
 static int labelCount = 0 ;
 static int returnCount = 0 ;

  // ***************  Symbol table mapping strings to int values  *********************

//helper function to translate vm operator
static void vm_operator_helper()
{
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;
    output_assembler("D=M") ;
    output_assembler("A=A-1") ;
       
}

//function to push values onto the stack
static void push_stack()
{
    output_assembler("D=M") ;  
    output_assembler("@SP") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("M=M+1") ;    
}

//function to pop values off the stack
static void pop_stack()
{
    output_assembler("D=D+A") ;
    output_assembler("@R13") ;
    output_assembler("M=D") ;
    output_assembler("@SP") ;
    output_assembler("AM=M-1") ;   
    output_assembler("D=M") ;
    output_assembler("@R13") ;
    output_assembler("A=M") ;
    output_assembler("M=D") ;   
}
// translate vm operator command into assembly language
static void translate_vm_operator(TokenKind the_op)
{
    start_of_vm_operator_command(the_op) ;

    switch (the_op)
    {
        case tk_add: 
            vm_operator_helper() ;
            output_assembler("M=D+M") ;  
            break ;

        case tk_sub:
            vm_operator_helper() ;
            output_assembler("M=M-D") ;  
            break ;  

        case tk_and:
            vm_operator_helper() ;
            output_assembler("M=D&M") ;  
            break ;

        case tk_or:
            vm_operator_helper() ; 
            output_assembler("M=D|M") ;  
            break ; 

        case tk_eq:
                 
            break ;    


         case tk_return:
            //endFrame = LCL
            output_assembler("@LCL") ;
            output_assembler("D=M") ;
            output_assembler("@R13") ;
            output_assembler("M=D") ;
            output_assembler("@R13") ;
            output_assembler("D=M") ;           

            //retAddr = *(endFrame - 5)
            output_assembler("@5") ;
            output_assembler("D=D-A") ;
            output_assembler("A=D") ;
            output_assembler("D=M") ;
            output_assembler("@R14") ;
            output_assembler("M=D") ;

            //*ARG = pop()
            output_assembler("@SP") ;
            output_assembler("A=M-1") ;
            output_assembler("D=M") ;
            output_assembler("@ARG") ;
            output_assembler("A=M") ;
            output_assembler("M=D") ;

            //SP = ARG + 1
            output_assembler("@ARG") ;
            output_assembler("D=M+1") ;
            output_assembler("@SP") ;
            output_assembler("M=M-1") ;
            output_assembler("M=D") ;

            //THAT = *(endFrame - 1)
            output_assembler("@R13") ;
            output_assembler("A=M-1") ;
            output_assembler("D=M") ;
            output_assembler("@THAT") ;
            output_assembler("M=D") ;

            //THIS = *(endFrame - 2)
            output_assembler("@R13") ;
            output_assembler("D=M") ;
            output_assembler("@2") ;
            output_assembler("A=D-A") ;
            output_assembler("D=M") ;
            output_assembler("@THIS") ;
            output_assembler("M=D") ;           

            //ARG = *(endFrame - 3)   
            output_assembler("@R13") ;
            output_assembler("D=M") ;
            output_assembler("@3") ;
            output_assembler("A=D-A") ;
            output_assembler("D=M") ;
            output_assembler("@ARG") ;
            output_assembler("M=D") ;           

            //LCL = *(endFrame - 4)   
            output_assembler("@R13") ;
            output_assembler("D=M") ;
            output_assembler("@4") ;
            output_assembler("A=D-A") ;
            output_assembler("D=M") ;
            output_assembler("@LCL") ;
            output_assembler("M=D") ;           

            //goto retAddr
            output_assembler("@R14") ;
            output_assembler("A=M") ;
            output_assembler("0;JMP") ;
            break ;    

        case tk_neg:
            
            output_assembler("@SP") ;
            output_assembler("A=M-1") ;
            output_assembler("M=-M") ;
            break ;  

         case tk_gt: 
        /*    output_assembler("@SP") ;
            output_assembler("A=M-1") ;
            output_assembler("D=M") ;
            output_assembler("A=A-1") ;
            output_assembler("M=D-A") ;
            output_assembler("0;JGT") ; */
            break ;    

         case tk_lt:
                   
            break ; 

        case tk_not:
            output_assembler("@SP") ;
            output_assembler("A=M-1") ;
            output_assembler("M=!M") ;
            break ;

        default: break ;          

    }

    end_of_vm_command() ;
}

// translate vm jump command into assembly language
static void translate_vm_jump(TokenKind jump, string label)
{
    start_of_vm_jump_command(jump,label) ;

    // ... your code goes here ...

    switch ( jump )
    {
        case tk_goto:
            output_assembler( "@" + funcName + "$" +  label ) ;
            output_assembler("0;JMP") ;
            break ;

        case tk_if_goto:
            output_assembler("@SP") ;
            output_assembler("AM=M-1") ;
            output_assembler("D=M") ; 
            output_assembler( "@" + funcName + "$" +  to_string(labelCount)) ;            
            output_assembler("D;JEQ") ;            
            output_assembler( "@" + funcName + "$" +  label ) ;
            output_assembler("0;JMP") ;
            output_assembler( "(" + funcName + "$" + to_string(labelCount) + ")" ) ;

            labelCount++ ;
            break ;

        case tk_label:
           output_assembler( "(" + funcName + "$" + label + ")" ) ;
            break ;

        default: break ; 
    }
  

    end_of_vm_command() ;
}



// translate vm call or function command into assembly language
static void translate_vm_function(TokenKind func, string label, int n)
{

    start_of_vm_func_command(func,label,n) ;

 
   // ... your code goes here ...
    if ( func == tk_function )
    {
        //get the class name
        size_t pos = label.find(".");
        className = label.substr(0,pos);
        funcName = label ;
        

        output_assembler( "(" + label + ")" ) ;

        for (int i=0; i<n; i++){

            //push n 0's onto the stack
            output_assembler("D=0") ;
            output_assembler("@SP") ;
            output_assembler("AM=M+1") ;
            output_assembler("A=A-1") ;
            output_assembler("M=D") ;
        }

    } 
    else if ( func == tk_call) 
    {

        //push retAdd01 
        output_assembler("@" + funcName + "$retAdd01" + to_string( returnCount ) ) ;
        output_assembler("D=A") ;  
        output_assembler("@SP") ;
        output_assembler("A=M") ;
        output_assembler("M=D") ;
        output_assembler("@SP") ;
        output_assembler("M=M+1") ;   

        //push LCL
        output_assembler( "@LCL" ) ;
        push_stack() ; 

        //push ARG
        output_assembler("@ARG") ;           
        push_stack() ;
          

        //push THIS
        output_assembler("@THIS") ;
        push_stack() ; 
 
        //push THAT
        output_assembler("@THAT") ;
        push_stack() ;

        //ARG = SP - 5 - nArgs
        output_assembler("@SP") ;
        output_assembler("D=M") ;
        output_assembler("@5") ;
        output_assembler("D=D-A") ;
        output_assembler("@" + to_string(n)) ;
        output_assembler("D=D-A") ;
        output_assembler("@ARG") ;
        output_assembler("M=D") ;

        //LCL = SP
        output_assembler("@SP") ;
        output_assembler("D=M") ;
        output_assembler("@LCL") ;
        output_assembler("M=D") ;

        //goto functionName
        output_assembler("@" + label) ;
        output_assembler("0;JMP") ;

        //(retAdd01) : (function$ret.1)
        output_assembler( "(" + funcName + "$retAdd01" + to_string( returnCount ) + ")" ) ;

        returnCount++ ;

    }

    end_of_vm_command() ;
}

// translate vm push or pop command into assembly language
static void translate_vm_stack(TokenKind stack,TokenKind segment,int offset)
{
    start_of_vm_stack_command(stack,segment,offset) ;

  // ... your code goes here ...
    if ( stack == tk_push )
    {
        switch ( segment )
        {
            case tk_argument:
                output_assembler("@" + to_string(offset)) ;
                output_assembler("D=A") ;
                output_assembler("@ARG") ;
                output_assembler("A=D+M") ;
                push_stack() ;
                break ;

            case tk_local:
                output_assembler("@" + to_string(offset)) ;
                output_assembler("D=A") ;
                output_assembler("@LCL") ;
                output_assembler("A=D+M") ;
                push_stack();   
                break;

            case tk_constant:           
                output_assembler("@" + to_string(offset)) ;
                output_assembler("D=A") ;
                output_assembler("@SP") ;
                output_assembler("AM=M+1") ;
                output_assembler("A=A-1") ;
                output_assembler("M=D") ; 
                break;

            case tk_static:
                output_assembler("@" + className + "." + to_string(offset)) ;
                push_stack();
                break ;

            case tk_temp:
                output_assembler("@R" + to_string(5 + offset));
                push_stack();
                break;

            case tk_this:
                output_assembler("@" + to_string(offset)) ;
                output_assembler("D=A") ;
                output_assembler("@THIS") ;
                output_assembler("A=D+M") ; 
                push_stack();   
                break;

            case tk_that:
                output_assembler("@" + to_string(offset)) ;
                output_assembler("D=A") ;
                output_assembler("@THAT") ;
                output_assembler("A=D+M") ; 
                push_stack();   
                break;  

            case tk_pointer:
                output_assembler("@R" + to_string(3 + offset)) ;
                output_assembler("D=M") ;
                output_assembler("@SP") ;
                output_assembler("A=M") ;
                output_assembler("M=D") ;
                output_assembler("@SP") ;
                output_assembler("AM=M+1") ;
                break ;

            default: break;

        }

    }
 
    if ( stack == tk_pop )
    {
        switch ( segment )
        {
            case tk_argument:
                output_assembler("@ARG") ;
                output_assembler("D=M");
                output_assembler("@" + to_string(offset)) ;
                pop_stack() ;  
                break ;

            case tk_pointer:
                output_assembler("@SP") ;
                output_assembler("AM=M-1") ;
                output_assembler("D=M") ;
                output_assembler("@R" + to_string(3 + offset)) ;
                output_assembler("M=D") ;
            break;


            case tk_local:
                output_assembler("@LCL") ;
                output_assembler("D=M") ;
                output_assembler("@" + to_string(offset)) ;
                pop_stack() ;                         
             break;


            case tk_temp:
                output_assembler("@5") ;
                output_assembler("D=A") ;
                output_assembler("@" + to_string(offset)) ;
                pop_stack() ;
            break;

            case tk_this:
                output_assembler("@THIS") ;
                output_assembler("D=M") ;
                output_assembler("@" + to_string(offset)) ;
                pop_stack() ;    
                break ;

            case tk_that:
                output_assembler("@THAT") ;
                output_assembler("D=M") ;
                output_assembler("@" + to_string(offset)) ;
                pop_stack() ;    
                break ;                       
 

            case tk_static:
                output_assembler("@SP") ;
                output_assembler("A=M-1") ;
                output_assembler("D=M") ;
                output_assembler("@" + className + "." + to_string(offset)) ;
                output_assembler("M=D") ;
                output_assembler("@SP") ;
                output_assembler("M=M-1") ;
                break ;

            default: break ;
        }
    }

    end_of_vm_command() ;
}


/************            AND HERE              **************/
///////////////////////////////////////////////////////////////

// grammar to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= "add" | "and" | "eq" | "gt" | "lt" | "neg" | "not" | "or" | "sub" | "return"
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

// vm_operator ::= "add" | "and" | "eq" | "gt" | "lt" | "neg" | "not" | "or" | "sub" | "return"
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

