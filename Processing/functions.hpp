#pragma once

#ifndef _FUNCTIONS_HPP
#define _FUNCTIONS_HPP

#include "../tokens.hpp"
#include "strPair.hpp"
#include "keywords.hpp"

#include <vector>

bool makeFunctions(std::vector<Token>& inputList, std::vector<Token>& tkList, std::vector<strPair>& keywords){
    for(uint64_t i = 0; i < (inputList.size() - 1); ++i){
        std::cout << inputList.at(i).content << "\n";

        if(inputList.at(i).type != TOK_misc_combo){
            tkList.push_back(inputList.at(i));

            continue;
        }

        if(isKeyword(inputList.at(i).content, keywords)){
            std::cout << "keyword defined as function name\n";

            return false;
        }
        
    }

    return true;
}

#endif /* _FUNCTIONS_HPP */