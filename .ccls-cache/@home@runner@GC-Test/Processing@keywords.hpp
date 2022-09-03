#pragma once

#ifndef _KEYWORDS_HPP
#define _KEYWORDS_HPP

#include "strPair.hpp"

bool isKeyword(const std::string& str, std::vector<strPair>& keywords){
    for(strPair s : keywords){
        if(s.first == str){
            return true;
        }
    }

    return false;
}

#endif /* _KEYWORDS_HPP */