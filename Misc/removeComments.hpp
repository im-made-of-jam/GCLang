#pragma once

#ifndef REMOVECOMMENTS_HPP
#define REMOVECOMMENTS_HPP

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

#endif /* REMOVECOMMENTS_HPP */