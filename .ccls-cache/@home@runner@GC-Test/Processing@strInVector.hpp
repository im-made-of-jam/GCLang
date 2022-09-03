#pragma once

#ifndef _STRINVECTOR_HPP
#define _STRINVECTOR_HPP

#include <string>
#include <vector>

bool strInVector(const std::string& string, const std::vector<std::string>& vector){
    for(std::string s : vector){
        if(s == string){
            return true;
        }
    }

    return false;
}

#endif /* _STRINVECTOR_HPP */