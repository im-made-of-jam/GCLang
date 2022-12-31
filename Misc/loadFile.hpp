#pragma once

#ifndef LOADFILE_HPP
#define LOADFILE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// loads a file by filename into a std::vector<std::string>
// requires c++20 or above due to std::filesystem
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

#endif /* LOADFILE_HPP */