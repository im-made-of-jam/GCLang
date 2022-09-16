#pragma once

#ifndef BYTECODEGEN_HPP
#define BYTECODEGEN_HPP


#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <bitset>
#include <ios>

#include "tokens.hpp"

class _raw{
public:
enum AllInstrs{
    add = 0,
    sub = 1,
    mul = 2,
    div = 3,
    mod = 4,

    invert = 5,
    equals = 6,

    push = 7,
    dup = 8,
    drop = 9,
    swap = 10,

    move = 11,

    size = 12,
    active = 13,

    ifBegin = 14,
    ifEnd = 15,

    loopBegin = 16,
    loopEnd = 17,
    loopBreak = 18,

    fnCall = 19,
    fnReturn = 20,

    external = 21,
    builtin = 22,

    ptrMake = 23,
    ptrDeref = 24,

    noOp = 32767,

    // beginning of debug

    _dbg_cout_size = 32769,
};

enum AllStacks{
    Main = 0,
    Second = 1,
    Third = 2,
    Fourth = 3,
};

static std::vector<uint32_t> makeInstr(uint32_t type, uint32_t stack, std::vector<uint32_t> additionals){
    std::vector<uint32_t> ret;

    // see explanations.txt
    // get type in right position
    uint32_t Type = (type & 0xFFFF) << 8;

    // get stack in right position
    uint32_t Stack = stack & 0b11;

    // get total length of instruction
    uint32_t Length = ((additionals.size() + 1) & 0xFF) << 24;

    ret.push_back((Type | Stack | Length)); // add instruction

    // add additionals
    for(uint32_t t : additionals){
        ret.push_back(t);
    }

    return ret;
}

static std::vector<uint32_t> makeInstr(uint32_t type, uint32_t stack = 0){
    std::vector<uint32_t> empty;

    return makeInstr(type, stack, empty);
}

};

bool generateBytecode(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    // stacks for keeping track of pointers for ifs, loops, and functions
    std::vector<uint64_t> ifStack;
    std::vector<uint64_t> loopStack;
    std::vector<uint64_t> fnStack;

    // the actoal output file itself
    std::ofstream oFile;
    oFile.open(filename, std::ios::out | std::ios::binary);

    // the content of a token
    std::vector<uint32_t> content;
    // the raw instruction to be written to the file
    std::vector<uint32_t> instr;

    auto clearContent = [&](){
        content.clear();
        content.shrink_to_fit();
        instr.clear();
        instr.shrink_to_fit();
    };

    auto output = [&](uint32_t outputThis){
        oFile << static_cast<char>(outputThis >> 24);
        oFile << static_cast<char>(outputThis >> 16 & 0xFF);
        oFile << static_cast<char>(outputThis >> 8  & 0xFF);
        oFile << static_cast<char>(outputThis & 0xFF);
    };

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_num_combo:{
                try{
                    clearContent();

                    uint64_t value = stoll(tk.content);

                    content.push_back(value >> 32);
                    content.push_back(value);

                    instr = _raw::makeInstr(_raw::push, 3, content);

                    for(uint32_t t : instr){
                        output(t);

                        std::cout << std::hex << t << '\n';
                    }

                    break;
                }
                catch(std::out_of_range e){
                    return false;
                }
            }

            // token types we do nothing for
            case TOK_separator:{
                break;
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

    oFile.close();

    return true;
}

#endif /* BYTECODEGEN_HPP */