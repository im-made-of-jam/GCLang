#pragma once

#ifndef ASMGEN_HPP
#define ASMGEN_HPP

#include <string>
#include <vector>
#include <fstream>

#include "tokens.hpp"
#include "Misc/loadFile.hpp"

bool generateASM(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup
    oFile << "format elf64 executable 3\n";
    oFile << "\n";
    oFile << "entry __main\n";
    oFile << "\n";

    std::vector<std::string> stdLib;

    if(includeStdlib){
        if(!loadFile(stdLib, stdLibPath)){
            std::cout << "could not load standard library, continuing anyways\n";
        }
    }

    for(std::string line : stdLib){
        oFile << line << "\n";
    }

    // every token type from now *should* be executable
    oFile << "segment readable executable\n";
    oFile << "__main:\n";
    oFile << "\n\n\n";

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_num_combo:{
                oFile << "push " << tk.content << "\n";

                break;
            }
            case TOK_call_extern:{
                oFile << "call " << tk.content << "\n";
                oFile << "add rsp, 8\n";
                break;
            }
            case TOK_separator:{
                continue;
            }
            default:{
                std::cout << "no assembly generation exists for token of type " << getTokenName(tk.type) << "\n";

                return false;
            }
        }
    }

    oFile.close();

    return true;
}

#endif