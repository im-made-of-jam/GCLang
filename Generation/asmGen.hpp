#pragma once

#ifndef ASMGEN_HPP
#define ASMGEN_HPP

#include <string>
#include <vector>
#include <fstream>

#include "tokens.hpp"
#include "Misc/mainArgs.hpp"
#include "Misc/loadFile.hpp"

bool generateASM(const std::string filename, std::vector<Token>& writeToFile, MainArgs args){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup
    oFile << "format elf64 executable 3\n";
    oFile << "\n";
    oFile << "entry __main\n";
    oFile << "\n";

    std::vector<std::string> stdLib;

    if(args.includeASMStdLib){
        if(!loadFile(stdLib, args.ASMStdLibPath)){
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
    oFile << "\n; auto-generated code below, reading it ay be difficult\n";

    if(!args.debug){
        oFile << "; build using -D flag to add some potentially helpful comments\n";
    }

    uint64_t labelNumber = 1; // labels probably need their own unique value, so we create this number to make them unique

    // if blocks require their own labels, and as such we have a stack for generating them
    // this struct is made so that if there is no else, the label can simply be discarded
    struct IfLabelPair{
        uint64_t elseLabel;
        uint64_t endLabel;

        IfLabelPair(uint64_t Else, uint64_t End){
            elseLabel = Else;
            endLabel = End;
        }
    };

    std::vector<IfLabelPair> ifLabels;

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_num_combo:{
                if(args.debug){
                    oFile << "\n; TOK_num_combo\n";
                }
                // push imm64 does not exist
                // push r/m64 does though, so we use this with "mov, r/m64, imm64"
                oFile << "mov rax, " << tk.content << "\n";
                oFile << "push rax\n";
                break;
            }

            case TOK_add:{
                if(args.debug){
                    oFile << "\n TOK_add\n";
                }
                oFile << "pop r15\n";
                oFile << "pop r14\n";
                oFile << "add r14, r15\n";
                oFile << "push r14\n";
                break;
            }
            case TOK_sub:{
                if(args.debug){
                    oFile << "\n; TOK_sub\n";
                }
                oFile << "pop r14\n";
                oFile << "pop r15\n";
                oFile << "sub r15, r14\n";
                oFile << "push r15\n";
                break;
            }
            case TOK_mul:{
                if(args.debug){
                    oFile << "\n; TOK_mul\n";
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
                    oFile << "\n; TOK_mul\n";
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
                    oFile << "\n; TOK_mul\n";
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
                    oFile << "\n; TOK_equals\n";
                }
                oFile << "pop r15\n";
                oFile << "pop r14\n";
                oFile << "sub r14, r15\n";

                uint64_t firstLabel = labelNumber++;
                uint64_t secondLabel = labelNumber++;

                oFile << "jz __L" << firstLabel << "\n";
                oFile << "push BYTE 0\n";
                oFile << "jmp __L" << secondLabel << "\n";
                oFile << "__L" << firstLabel << ":\n";
                oFile << "push BYTE 1\n";
                oFile << "__L" << secondLabel << ":\n";
                break;
            }
            case TOK_not:{
                if(args.debug){
                    oFile << "\n; TOK_not\n";
                }
                oFile << "pop r15\n";
                oFile << "cmp r15, 0\n";

                uint64_t firstLabel = labelNumber++;
                uint64_t secondLabel = labelNumber++;

                oFile << "jz __L" << firstLabel << "\n";
                oFile << "push BYTE 0\n";
                oFile << "jmp __L" << secondLabel << "\n";
                oFile << "__L" << firstLabel << ":\n";
                oFile << "push BYTE 1\n";
                oFile << "__L" << secondLabel << ":\n";
                break;
            }

            case TOK_call_extern:{
                if(args.debug){
                    oFile << "\n; TOK_call_extern\n";
                }
                // in ASM mode externs always have exactly one argument
                oFile << "call " << tk.content << "\n";
                oFile << "add rsp, 8\n";
                // this means we can always clean up the stack in the same way, by adding to the stack pointer
                break;
            }

            case TOK_stack_dup:{
                if(args.debug){
                    oFile << "\n; TOK_stack_dup\n";
                }
                oFile << "pop rax\n";
                oFile << "push rax\n";
                oFile << "push rax\n";
                break;
            }
            case TOK_stack_drop:{
                if(args.debug){
                    oFile << "\n; TOK_stack_drop\n";
                }
                oFile << "add rsp, 8\n";
                break;
            }
            case TOK_stack_swap:{
                if(args.debug){
                    oFile << "\n; TOK_stack_swap\n";
                }
                oFile << "pop r15\n";
                oFile << "pop r14\n";
                oFile << "push r15\n";
                oFile << "push r14\n";
                break;
            }

            case TOK_if_begin:{
                if(args.debug){
                    oFile << "\n; TOK_if_begin\n";
                }
                ifLabels.push_back(IfLabelPair(labelNumber++, labelNumber++));

                oFile << "pop r15\n";
                oFile << "cmp r15, 0\n";
                oFile << "jz __L" << ifLabels.back().elseLabel << "\n";
                break;
            }
            case TOK_if_else:{
                if(args.debug){
                    oFile << "\n; TOK_if_else\n";
                }
                oFile << "jmp __L" << ifLabels.back().endLabel << "\n";
                oFile << "__L" << ifLabels.back().elseLabel << ":\n";

                ifLabels.back().elseLabel = 0;

                break;
            }
            case TOK_if_end:{
                if(args.debug){
                    oFile << "\n; TOK_if_end\n";
                }
                if(ifLabels.back().elseLabel){
                    oFile << "__L" << ifLabels.back().elseLabel << ":\n";
                }
                oFile << "__L" << ifLabels.back().endLabel << ":\n";

                ifLabels.pop_back();
                break;
            }

            case TOK_switch_stack:
            case TOK_data_move:{
                std::cout << "multiple stacks not supported in assembly mode\n";

                return false;
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

    const std::string windDownPath = args.ASMWindDownPath;

    std::vector<std::string> stdWindDown;

    if(args.includeASMStdLib){
        if(!loadFile(stdWindDown, args.ASMWindDownPath)){
            std::cout << "could not load assembly stdlib wind down, continuing anyways\n";
            std::cout << "    path given - " << args.ASMWindDownPath << "\n";
        }
    }

    for(std::string s : stdWindDown){
        oFile << s << "\n";
    }

    oFile.close();

    return true;
}

#endif