#pragma once

#ifndef _REMOVECOMMENTS_HPP
#define _REMOVECOMMENTS_HPP

#include <stdnoreturn.h>
#include <vector>
#include <string>

bool removeComments(const std::vector<std::string>& inputList, std::vector<std::string>& outputList, char commentChar){
    std::string working;

    for(std::string s : inputList){
        working = "";

        for(char c : s){
            if(c == commentChar){
                break;
            }

            working += c;
        }

        outputList.push_back(working);
    }

    return true;
}

#endif /* _REMOVECOMMENTS_HPP */