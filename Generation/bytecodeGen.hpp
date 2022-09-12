#pragma once

#ifndef BYTECODEGEN_HPP
#define BYTECODEGEN_HPP


#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

#include "tokens.hpp"



bool generateBytecode(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup lines for registers, the four stacks, the GCPointer struct etc.
}



#endif /* BYTECODEGEN_HPP */