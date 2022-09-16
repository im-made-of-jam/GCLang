#pragma once

#ifndef VERIFICATION_HPP
#define VERIFICATION_HPP

#include <vector>
#include <iostream>
#include <string>

#include "tokens.hpp"

#warning the verification functions acting upon a string break now that keywords and token replacements exist. please fix

// ensure there is an even number of quotes so that the program does not contain a malformed string
bool verifyStringQuotes(const std::string& str){
    uint64_t count = 0;

    for(char c : str){
        if(c == '"'){
            ++count;
        }
    }

    if(count % 2){
        return false;
    }

    return true;
}

// verify that the function brackets are closed properly, and that there is never a function definition from within another function definition
bool verifyFuncBrackets(const std::string& str){
    bool inFunc = false;

    for(char c : str){
        if(c == '{'){
            // another definition from within a function
            if(inFunc){
                return false;
            }

            inFunc = true;
        }
        if(c == '}'){
            // closing a function while not inside of a function
            if(!inFunc){
                return false;
            }

            inFunc = false;
        }
    }

    return !inFunc;
}

// ensure that all loops are closed, and that there is never a break outside of a loop
bool verifyLoopBrackets(const std::string& str){
    int32_t depth = 0;

    for(char c : str){
        if(c == '('){
            ++depth;
        }
        if(c == ')'){
            --depth;
        }
        if(c == '^'){
            if(depth == 0){
                std::cout << "\"break\" outside of loop\n";
                return false;
            }
        }

        if(depth < 0){
            return false;
        }
    }

    return !depth;
}

// ensure that a number fits inside of 64 bits, and all of the chars in its digits are valid
bool verifyNumber(const std::string& n){
    try{
        std::stoll(n);
    }
    catch(std::out_of_range const e){
        std::cout << "number too long\n";

        std::cout << "    " << n << std::endl;

        return false;
    }

    for(char c : n){
        if(!(c >= '0' && c <= '9')){
            return false;;
        }
    }

    return true;
}

bool verifyTokens(std::vector<Token>& tkList){ // TODO verify loop tokens, function tokens, names, numbers etc
    bool inFunc = false;
    int64_t loopDepth = 0;
    uint64_t strCount = 0;

    for(Token tk : tkList){
        switch(tk.type){
            case TOK_loop_start:{
                loopDepth++;
                break;
            }

            case TOK_loop_end:{
                if(loopDepth == 0){
                    return false;
                }

                loopDepth--;
                break;
            }

            case TOK_func_begin:{
                if(inFunc){
                    return false;
                }

                inFunc = true;
                break;
            }

            case TOK_func_end:{
                if(!inFunc){
                    return false;
                }

                inFunc = false;
                break;
            }

            case TOK_string_begin:
            case TOK_string_end:{
                strCount++;
                break;
            }
        }
    }

    if(loopDepth != 0){
        std::cout << "loop bracket mismatch\n";

        return false;
    }

    if(inFunc){
        std::cout << "function bracket mismatch\n";

        return false;
    }

    if(strCount % 2){
        std::cout << "string quote mismatch\n";

        return false;
    }
    
    return true;
}

#endif /* VERIFICATION_HPP */
