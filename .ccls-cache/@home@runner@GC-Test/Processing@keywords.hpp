#pragma once

#ifndef _KEYWORDS_HPP
#define _KEYWORDS_HPP

bool isKeyword(const std::string& str, std::vector<std::string>& keywords){
    for(std::string s : keywords){
        if(s == str){
            return true;
        }
    }

    return false;
}

#endif /* _KEYWORDS_HPP */