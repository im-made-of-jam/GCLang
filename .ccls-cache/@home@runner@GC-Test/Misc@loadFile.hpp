#pragma once

#ifndef _LOADFILE_HPP
#define _LOADFILE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

bool loadFile(std::vector<std::string>& lines, const std::string& filename){
    std::ifstream file;

    file.open(filename);

    if(!std::filesystem::exists(filename)){
        return false;
    }

    if (file.bad()){
        return false;
    }

    std::string line;
    while(std::getline(file, line)){
        lines.push_back(line);
    }

    return true;
}

#endif /* _LOADFILE_HPP */