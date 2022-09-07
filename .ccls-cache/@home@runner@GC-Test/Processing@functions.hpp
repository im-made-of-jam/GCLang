#pragma once

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "tokens.hpp"
#include "Processing/strPair.hpp"
#include "Processing/keywords.hpp"
#include "Processing/strInVector.hpp"

#include "Tokens/MainTokens.hpp"

#include <vector>

bool makeFunctions(std::vector<Token>& inputList, std::vector<Token>& tkList, std::vector<strPair>& keywords){
    // name of every function ever declared / called
    std::vector<std::string> funcNames;

    // for between declarations and calls
    std::vector<Token> intermediate;

    // first find the names of all functions and turn them into declarations
    for(uint64_t i = 0; i < inputList.size(); ++i){
        if(inputList.at(i).type == TOK_misc_combo){
            // invalid index errors shouldnt be a problem because function brackets have already been verified
            if(inputList.at(i + 1).type == TOK_func_begin){
                if(strInVector(inputList.at(i).content, funcNames)){
                    std::cout << "cannot re-declare function\n";
                    std::cout << "with name \"" << inputList.at(i).content << "\"\n";
                }
                
                funcNames.push_back(inputList.at(i).content);

                intermediate.push_back(Token(TOK_func_decl, inputList.at(i).content));

                i += 1;
                continue;
            }
        }

        intermediate.push_back(inputList.at(i));
    }

    // then find the calls of functions afterwards
    for(uint64_t i = 0; i < intermediate.size(); ++i){
        if(intermediate.at(i).type != TOK_misc_combo){
            tkList.push_back(intermediate.at(i));

            continue;
        }

        if(intermediate.at(i).type == TOK_func_begin){
            continue;
        }

        if(!strInVector(intermediate.at(i).content, funcNames)){
            tkList.push_back(intermediate.at(i));
            
            continue;
        }

        tkList.push_back(Token(TOK_func_call, intermediate.at(i).content));
    }
    
    for(Token tk : intermediate){
        std::cout << "Token of type " << getTokenName(tk.type);
        std::cout << "\n with content " << tk.content << "\n\n";
    }
    
    return true;
}

#endif /* FUNCTIONS_HPP */
