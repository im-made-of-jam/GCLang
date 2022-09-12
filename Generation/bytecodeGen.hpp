#pragma once

#ifndef BYTECODEGEN_HPP
#define BYTECODEGEN_HPP


#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

#include "tokens.hpp"

class _raw{
enum AllInstrs{
    add = 0,
    sub,
    mul,
    div,
    mod,

    invert,
    equals,

    push,
    dup,
    drop,
    swap,

    move,

    size,
    active,

    ifBegin,
    ifEnd,

    loopBegin,
    loopEnd,
    loopBreak,

    fnCall,
    fnReturn,

    external,
    builtin,

    ptrMake,
    ptrDeref,

    _dbg_cout_size,
};

enum AllStacks{
    Main,
    Second,
    Third,
    Fourth
};

};

bool generateBytecode(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_num_combo:{
                
            }

            // all of the invalid token types
            case TOK_num_digit:
            default:{
                std::cout << "invalid token of type: " << getTokenName(tk.type) << "\n";
                std::cout << "         with content: " << tk.content << "\n";

                return false;
            }
        }
    }

    return true;
}

#endif /* BYTECODEGEN_HPP */