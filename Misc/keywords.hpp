#pragma once

#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include "Processing/strPair.hpp"

#include <vector>
#include <string>

// every built in keyword
const std::vector<std::string> builtInKeywords = {"if", 
                                                  "else",
                                                  "endif", 
                                                  "break",
                                                  "dup",
                                                  "loop",
                                                  "endloop",
                                                  "drop",
                                                  "swap"
};

bool isKeyword(const std::string& str, std::vector<strPair>& keywords){
    for(std::string s : builtInKeywords){
        if(s == str){
            return true;
        }
    }

    for(strPair s : keywords){
        if(s.first == str){
            return true;
        }
    }

    return false;
}

#endif /* KEYWORDS_HPP */
