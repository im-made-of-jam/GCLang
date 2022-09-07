#pragma once

#ifndef STR_IN_VECTOR_HPP
#define STR_IN_VECTOR_HPP

#include <string>
#include <vector>

bool strInVector(const std::string& inVector, const std::vector<std::string> vec){
    for(std::string s : vec){
        if(s == inVector){
            return true;
        }
    }

    return false;
}

#endif /* STR_IN_VECTOR_HPP */