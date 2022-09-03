#pragma once

#ifndef _VERIFICATION_HPP
#define _VERIFICATION_HPP

#include <vector>
#include <iostream>
#include <string>

#include "../tokens.hpp"

// ensure there is n even number of quotes so that the program does not contain a malformed string
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

bool verifyFuncBrackets(const std::string& str){
    bool inFunc = false;

    for(char c : str){
        if(c == '{'){
            if(inFunc){
                return false;
            }

            inFunc = true;
        }
        if(c == '}'){
            if(!inFunc){
                return false;
            }

            inFunc = false;
        }
    }

    return !inFunc;
}

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

#endif /* _VERIFICATION_HPP */