#pragma once

#ifndef KEYWORDS_HPP
#define KEYWORDS_HPP

#include "Processing/strPair.hpp"

bool isKeyword(const std::string& str, std::vector<strPair>& keywords){
    for(strPair s : keywords){
        if(s.first == str){
            return true;
        }
    }

    return false;
}

#endif /* KEYWORDS_HPP */
