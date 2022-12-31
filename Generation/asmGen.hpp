#pragma once

#ifndef ASMGEN_HPP
#define ASMGEN_HPP

#include <string>
#include <vector>
#include <fstream>

#include "tokens.hpp"
#include "Misc/mainArgs.hpp"
#include "Misc/loadFile.hpp"

bool generateASM(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath, MainArgs args){
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
    oFile << "\n\n\n";
    oFile << "__main:\n";

    uint64_t labelNumber = 0; // labels probably need their own unique value, so we create this number to make them unique

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_num_combo:{
                oFile << "push QWORD " << tk.content << "\n";

                break;
            }

            case TOK_add:{
                if(args.debug){
                    oFile << "; TOK_add\n";
                }
                oFile << "pop r15\n";
                oFile << "pop r14\n";
                oFile << "add r14, r15\n";
                oFile << "push r14\n";
                break;
            }
            case TOK_sub:{
                if(args.debug){
                    oFile << "; TOK_sub\n";
                }
                oFile << "pop r14\n";
                oFile << "pop r15\n";
                oFile << "sub r15, r14\n";
                oFile << "push r15\n";
                break;
            }
            case TOK_mul:{
                if(args.debug){
                    oFile << "; TOK_mul\n";
                }
                oFile << "pop r15\n";
                oFile << "pop rax\n";
                oFile << "xor rdx, rdx\n";
                oFile << "mul r15\n";
                oFile << "push rax\n";
                break;
            }
            case TOK_div:{
                if(args.debug){
                    oFile << "; TOK_mul\n";
                }
                oFile << "pop r15\n";
                oFile << "pop rax\n";
                oFile << "xor rdx, rdx\n";
                oFile << "div r15\n";
                oFile << "push rax\n";
                break;
            }
            case TOK_mod:{
                if(args.debug){
                    oFile << "; TOK_mul\n";
                }
                oFile << "pop r15\n";
                oFile << "pop rax\n";
                oFile << "xor rdx, rdx\n";
                oFile << "div r15\n";
                oFile << "push rdx\n";
                break;
            }
            case TOK_equals:{
                if(args.debug){
                    oFile << "; TOK_equals\n";
                }
                oFile << "pop r15\n";
                oFile << "pop r14\n";
                oFile << "sub r14, r15\n";

                uint64_t firstLabel = labelNumber++;
                uint64_t secondLabel = labelNumber++;

                oFile << "jz __L" << firstLabel << "\n";
                oFile << "push QWORD 0\n";
                oFile << "jmp __L" << secondLabel << "\n";
                oFile << "__L" << firstLabel << ":\n";
                oFile << "push QWORD 1\n";
                oFile << "__L" << secondLabel << ":\n";
                break;
            }
            case TOK_call_extern:{
                oFile << "call " << tk.content << "\n";
                oFile << "add rsp, 8\n";
                break;
            }


            case TOK_string_begin:
            case TOK_string_end:
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