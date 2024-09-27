#pragma once

#ifndef MAIN_TOKENS_HPP
#define MAIN_TOKENS_HPP

#include "Tokens/combinations.hpp"
#include "tokens.hpp"
#include "Tokens/tokenVerifications.hpp"

bool tokenise(std::vector<Token>& tkList, const std::string& source){
    // whether or ont the tokens being processed are part of a string and should be string_char or not
    bool inString = false;
    uint64_t strLength = 0;

    // first we verify various things so we know the source is not a bad file
    if(!verifyFuncBrackets(source)){
        tkList.push_back(Token(TOK_error, "function brackets mismatch"));

        return false;
    }

    if(!verifyLoopBrackets(source)){
        tkList.push_back(Token(TOK_error, "loop brackets mismatch"));

        return false;
    }

    if(!verifyStringQuotes(source)){
        tkList.push_back(Token(TOK_error, "string literal quotes mismatch"));

        return false;
    }

    // now we go through every single character one by one
    for(char c : source){
        // if the current char is inside of a string then we handle it differently
        if(inString){
            if(c == '"'){ // end o the string
                inString = false; // tell the rest of the function we are no longer in a string

                // not used for anything as of yet but it is here for when it is used
                tkList.push_back(Token(TOK_string_end,
                                       std::string{std::to_string(strLength)}
                                      ));

                // strings are pointers so we add a lenth and a make_pointer token
                tkList.push_back(Token(TOK_num_combo, std::to_string(strLength)));
                tkList.push_back(Token(TOK_make_pointer, std::string{'<'}));

                strLength = 0; // and now reset the length
            }
            else{ // still in string but not at the end of the string
                strLength++;
                tkList.push_back(Token(TOK_string_char, std::string{c}));
            }

            continue;
        }

        // chars are just abstracted ints so we can switch on them for their value
        switch(c){
            // each token added of od the structure Token(type, content) where type is the specific type of token to be added
            //   and content is a string constructed from the char used to determine the token type

            // these should be self explanatory based on token names

            case '+':{
                tkList.push_back(Token(TOK_add, std::string{c}));
                break;
            }

            case '-':{
                tkList.push_back(Token(TOK_sub, std::string{c}));
                break;
            }

            case '[':{
                tkList.push_back(Token(TOK_if_begin, std::string{c}));
                break;
            }

            case '|':{
                tkList.push_back(Token(TOK_if_else, std::string{c}));
                break;
            }

            case ']':{
                tkList.push_back(Token(TOK_if_end, std::string{c}));
                break;
            }

            case '(':{
                tkList.push_back(Token(TOK_loop_start, std::string{c}));
                break;
            }

            case ')':{
                tkList.push_back(Token(TOK_loop_end, std::string{c}));
                break;
            }

            case '<':{
                tkList.push_back(Token(TOK_dereference, std::string{c}));
                break;
            }

            case '>':{
                tkList.push_back(Token(TOK_make_pointer, std::string{c}));
                break;
            }

            case '$':{
                tkList.push_back(Token(TOK_stack_swap, std::string{c}));
                break;
            }

            case '^':{
                tkList.push_back(Token(TOK_loop_break, std::string{c}));
                break;
            }

            case '!':{
                tkList.push_back(Token(TOK_bool_not, std::string{c}));
                break;
            }

            case '"':{
                tkList.push_back(Token(TOK_string_begin, std::string{c}));
                inString = true;
                break;
            }

            case '\'':{
                tkList.push_back(Token(TOK_string_begin, std::string{c}));
                break;
            }

            case '{':{
                tkList.push_back(Token(TOK_func_begin, std::string{c}));
                break;
            }

            case '}':{
                tkList.push_back(Token(TOK_func_end, std::string{c}));
                break;
            }

            case '@':{
                tkList.push_back(Token(TOK_call_extern, std::string{c}));
                break;
            }

            case '&':{
                tkList.push_back(Token(TOK_switch_stack, std::string{c}));
                break;
            }

            case '~':{
                tkList.push_back(Token(TOK_data_move, std::string{c}));
                break;
            }

            case '=':{
                tkList.push_back(Token(TOK_equals, std::string{c}));
                break;
            }

            case '*':{
                tkList.push_back(Token(TOK_mul, std::string{c}));
                break;
            }

            case '/':{
                tkList.push_back(Token(TOK_div, std::string{c}));
                break;
            }

            case '%':{
                tkList.push_back(Token(TOK_mod, std::string{c}));
                break;
            }

            // treat all whitespace the same
            case ' ':
            case '\n':
            case '\v':
            case '\t':{
                tkList.push_back(Token(TOK_separator, std::string{c}));
                break;
            }

            // also treat numbers similarly the only difference being the content in the token
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':{
                tkList.push_back(Token(TOK_num_digit, std::string{c}));
                break;
            }

            // anything else might not be an error, so that is handled later
            default:{
                tkList.push_back(Token(TOK_misc, std::string{c}));
                break;
            }
        }
    }

    return true;
}

// human readable version of a token name
std::string getTokenName(uint64_t token){
    switch(token){
        case TOK_misc:
            return std::string{"miscellaneous"};

        case TOK_misc_combo:
            return std::string{"miscellaneous combination"};

        case TOK_error:
            return std::string{"error"};

        case TOK_add:
            return std::string{"add"};

        case TOK_sub:
            return std::string{"subtract"};

        case TOK_mul:
            return std::string{"multiply"};

        case TOK_div:
            return std::string{"divide"};

        case TOK_equals:
            return std::string{"equals"};

        case TOK_bool_not:
            return std::string{"not"};

        case TOK_if_begin:
            return std::string{"if begin"};

        case TOK_if_end:
            return std::string{"if end"};

        case TOK_loop_start:
            return std::string{"loop begin"};

        case TOK_loop_end:
            return std::string{"loop end"};

        case TOK_loop_break:
            return std::string{"loop break"};

        case TOK_func_begin:
            return std::string{"function begin"};

        case TOK_func_end:
            return std::string{"function end"};

        case TOK_func_decl:
            return std::string{"function declaration"};

        case TOK_func_call:
            return std::string{"function call"};

        case TOK_separator:
            return std::string{"separator / whitespace"};

        case TOK_num_digit:
            return std::string{"numeral digit"};

        case TOK_num_combo:
            return std::string{"numeral combination"};

        case TOK_stack_dup:
            return std::string{"dup"};

        case TOK_stack_drop:
            return std::string{"drop"};

        case TOK_stack_swap:
            return std::string{"swap"};

        case TOK_call_extern:
            return std::string{"call extern"};

        case TOK_switch_stack:
            return std::string{"switch active stack"};

        case TOK_string_begin:
            return std::string{"string begin"};

        case TOK_string_char:
            return std::string{"string character"};

        case TOK_string_end:
            return std::string{"string end"};

        case TOK_string_crun:
            return std::string{"string character run"};
    }

    // no match should not be an error
    return std::string{"other"};
}

#endif /* MAIN_TOKENS_HPP */
