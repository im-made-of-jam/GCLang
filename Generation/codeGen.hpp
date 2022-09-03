#pragma once

#ifndef _CODEGEN_HPP
#define _CODEGEN_HPP

#include <filesystem>
#include <fstream>

#include "../tokens.hpp"
#include "../Misc/loadFile.hpp"
#include "../Tokens/MainTokens.hpp"

bool generateCpp(const std::string& filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup lines for registers, the four stacks, the GCPointer struct etc.

    oFile << "#include <vector>\n";                // needed for stack
    oFile << "#include <cstdint>\n";               // needed for 'register' types
    oFile << "#include <iostream>\n";              // as much as i dont want it, needed by GCPointer
    oFile << "\n";
    oFile << "bool exitFlag = false;\n";           // goes true when time to exit
    oFile << "uint64_t R_A = 0;\n";                // 'register' A
    oFile << "uint64_t R_B = 0;\n";                // 'register' B
    oFile << "\n";
    oFile << "std::vector<uint64_t> stacks[4] = {std::vector<uint64_t>(262144),\n";
    oFile << "                                   std::vector<uint64_t>(65536),\n";
    oFile << "                                   std::vector<uint64_t>(65536),\n";
    oFile << "                                   std::vector<uint64_t>(65536)};\n"; // the main stacks
    oFile << "uint64_t activeStack = 0;\n";

    // see Processing/GCPointer for the deminified struct
    oFile << "\n";
    oFile << "struct GCPointer{uint64_t* data;GCPointer(uint64_t count, std::vector<uint64_t>* stack){if((stack->size() == 0) || (count == 0)){data = new uint64_t[1]{0};return;}data = new uint64_t[count + 1];data[0] = count;for(uint64_t i = 0; i < count; ++i){if(!stack->size()){std::cout << \"stack underflow upon creating pointer struct\\n\";}data[i + 1] = stack->back();stack->pop_back();}}~GCPointer(){delete[] data;}uint64_t getCount(){return data[0];}};\n";
    oFile << "GCPointer* GC_P;\n"; // this is for easier processing of pointer operators

    // TODO implement function writing

    if(includeStdlib){
        oFile << "// __BEGIN_STDLIB\n"; // "pointer" for debugging

        // the entire stdLib in cpp form
        std::vector<std::string> stdLib;

        if(!loadFile(stdLib, stdLibPath)){
            std::cout << "could not open C++ stdlib at " << stdLibPath << '\n';

            return false;
        }

        for(std::string s : stdLib){
            oFile << s << '\n';
        }
    }

    oFile << "\n";
    oFile << "// __BEGIN_MAIN\n";
    oFile << "int main(int argc, char* argv[]){\n";

    oFile << std::flush;

    std::cout << "\nbegin code gen\n";
    
    for(Token tk : writeToFile){
        std::cout << "Token of type: " << tk.type << "\n with name of '" << getTokenName(tk.type) << "\n with content of '" << tk.content << "'\n\n";
        switch(tk.type){
            case TOK_num_digit:
            case TOK_num_combo:{
                oFile << "  stacks[activeStack].push_back(" + tk.content + ");\n";
                oFile << "\n";
                break;
            }

            case TOK_add:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A + R_B);\n";
                oFile << "\n";
                break;
                }
            case TOK_sub:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B - R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_mul:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A * R_B);\n";
                oFile << "\n";
                break;
            }
            case TOK_div:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B / R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_mod:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B % R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_not:{
                oFile << "  if(stacks[activeStack].back() == 0){\n";
                oFile << "   stacks[activeStack].pop_back();\n";
                oFile << "   stacks[activeStack].push_back(1);\n";
                oFile << "  }\n";
                oFile << "  else{\n";
                oFile << "   stacks[activeStack].pop_back();\n";
                oFile << "   stacks[activeStack].push_back(0);\n";
                oFile << "  }\n";
                oFile << "\n";
                break;
            }
            case TOK_equals:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  if(R_A == R_B){\n";
                oFile << "    stacks[activeStack].push_back(1);\n";
                oFile << "  }\n";
                oFile << "  else{\n";
                oFile << "    stacks[activeStack].push_back(0);\n";
                oFile << "  }\n";
                oFile << "\n";
                break;
            }

            case TOK_call_extern:{
                oFile << "  _extern_" + tk.content + "();\n";
                oFile << "\n";
                break;
            }

            case TOK_stack_dup:{
                oFile << "  stacks[activeStack].push_back(stacks[activeStack].back());\n";
                oFile << "\n";
                break;
            }
            case TOK_stack_drop:{
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "\n";
                break;
            }
            case TOK_stack_swap:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A)\n;";
                oFile << "  stacks[activeStack].push_back(R_B)\n;";
                break;
            }
            case TOK_switch_stack:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  activeStack = R_A;\n";
                oFile << "\n";
                break;
            }
            case TOK_data_move:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[R_A].push_back(stacks[R_B].back());\n";
                oFile << "  stacks[R_B].pop_back();\n";
                oFile << "\n";
                break;
            }

            case TOK_loop_start:{
                oFile << "  while(1){\n";
                oFile << "\n";
                break;
            }
            case TOK_loop_end:{
                oFile << "  }\n";
                oFile << "\n";
                break;
            }
            case TOK_loop_break:{
                oFile << "  break;";
                oFile << "\n";
                break;
             }

            case TOK_if_begin:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  if(R_A){\n";
                oFile << "\n";
                break;
            }
            case TOK_if_else:{
                oFile << "  }\n";
                oFile << "  else{\n";
                break;
            }
            case TOK_if_end:{
                oFile << "  }\n";
            }

            case TOK_separator:{
                break; // do nothing
            }

            case TOK_make_pointer:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = reinterpret_cast<uint64_t>(new GCPointer(R_A, &stacks[activeStack]));\n";
                oFile << "  stacks[activeStack].push_back(reinterpret_cast<uint64_t>(R_B));\n";
                oFile << "\n";
                break;
            }
            case TOK_dereference:{
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  GC_P = reinterpret_cast<GCPointer*>(R_A);\n";
                oFile << "  for(uint64_t i = 0; i < GC_P->data[0]; ++i){\n";
                oFile << "    stacks[activeStack].push_back(GC_P->data[GC_P->data[0] - (i)]);\n";
                oFile << "  }\n";
                oFile << "  stacks[activeStack].push_back(GC_P->data[0]);\n";
                oFile << "  delete GC_P;\n";
                break;
            }

            case TOK_string_begin:{
                break; // nothing yet until f-strings
            }

            case TOK_string_char:{
                oFile << "  stacks[activeStack].push_back('" << tk.content << "');\n";
                break;
            }
            case TOK_string_end:{
                break; // nothing as of yet
            }

            case TOK_func_decl:{
                oFile << "  auto " << tk.content << " = [&](){\n";
                
                break;
            }
            case TOK_func_end:{
                oFile << "  };\n";

                break;
            }
            case TOK_func_call:{
                oFile << "  " << tk.content << "();\n";

                break;
            }
            
            case TOK_error:{
                std::cout << "error token with content: " + tk.content + "\n";
                [[fallthrough]];
            }
            case TOK_func_begin:
            case TOK_misc_combo:
            case TOK_misc:
            default:{
                std::cout << "unreachable token\n";
                std::cout << "type: " << tk.type << "\n";
                std::cout << "content: '" << tk.content << "'\n";
                std::cout << "name: '" << getTokenName(tk.type) << "'\n";
                return false;
                break;
            }
        }

        oFile << std::flush;
    }

    if(includeStdlib){
        std::vector<std::string> stdLibWindDown;

        // wind down standard library
        oFile << '\n';

        if(!loadFile(stdLibWindDown, windDownPath)){
            std::cout << "could not open stdlib wind down at " << windDownPath << '\n';
            std::cout << "will continue anyways\n\n";
        }

        oFile << "// __BEGIN_EXTERN_WIND_DOWN\n";
        for(std::string s : stdLibWindDown){
            oFile << s << '\n';
        }
    }

    // close off int main
    oFile << "}";

    oFile.close();

    return true;
}

bool generatePyASM(const std::string& filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::vector<uint64_t> ifStack;
    std::vector<uint64_t> loopStack;

    auto ifPush = [&](uint64_t value){
        ifStack.push_back(value);
    };

    auto ifPop = [&](){
        uint64_t a = ifStack.back();
        ifStack.pop_back();
        return a;
    };

    auto loopPush = [&](uint64_t value){
        loopStack.push_back(value);
    };

    auto loopPop = [&](){
        uint64_t a = loopStack.back();
        loopStack.pop_back();
        return a;
    };

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup

    oFile << "jmp __init\n"; // jump to initialisation of what matters

    // Stack 1 - 0x0000 - 0x0FFF
    // Stack 2 - 0x1000 - 0x1FFF
    // Stack 3 - 0x2000 - 0x2FFF
    // Stack 4 - 0x3000 - 0x3FFF

    // active stack counter - 0x4000

    // Stack 1 pointer - 0x4001
    // Stack 2 pointer - 0x4002
    // Stack 3 pointer - 0x4003
    // Stack 4 pointer - 0x4004

    oFile << "__overflow:      \n";
    oFile << "  lda 10         \n"; // '\n'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 83         \n"; // 'S'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 116        \n"; // 't'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 97         \n"; // 'a'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 99         \n"; // 'c'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 107        \n"; // 'k'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 32         \n"; // ' '
    oFile << "  _debugprintc a \n";
    oFile << "  mov a 0x4000   \n"; // move active stack counter
    oFile << "  _debugprint a  \n";
    oFile << "  lda 32         \n"; // ' '
    oFile << "  _debugprintc a \n";
    oFile << "  lda 79         \n"; // 'O'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 118        \n"; // 'v'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 101        \n"; // 'e'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 114        \n"; // 'r'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 102        \n"; // 'f'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 108        \n"; // 'l'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 111        \n"; // 'o'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 119        \n"; // 'w'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 10         \n"; // '\n'
    oFile << "  _debugprintc a \n";
    oFile << "  halt           \n";

    oFile << "__underflow:     \n";
    oFile << "  lda 10         \n";
    oFile << "  _debugprintc a \n";
    oFile << "  lda 83         \n"; // 'S'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 116        \n"; // 't'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 97         \n"; // 'a'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 99         \n"; // 'c'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 107        \n"; // 'k'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 32         \n"; // ' '
    oFile << "  _debugprintc a \n";
    oFile << "  mov a 0x4000   \n"; // move active stack counter
    oFile << "  _debugprint a  \n";
    oFile << "  lda 32         \n"; // ' '
    oFile << "  _debugprintc a \n";
    oFile << "  lda 85         \n"; // 'U'
    oFile << " _debugprintc a  \n";
    oFile << "  lda 110        \n"; // 'n'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 100        \n"; // 'd'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 101        \n"; // 'e'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 114        \n"; // 'r'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 102        \n"; // 'f'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 108        \n"; // 'l'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 111        \n"; // 'o'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 119        \n"; // 'w'
    oFile << "  _debugprintc a \n";
    oFile << "  lda 10         \n"; // '\n'
    oFile << "  _debugprintc a \n";
    oFile << "  halt           \n";

    oFile << "__invalidStack:\n";
    oFile << "  halt\n";

    // push a value to the active stack
    oFile << "__push:          \n";
    oFile << "  push a         \n"; // keep value to push
    oFile << "  mov b 0x4000   \n";
    oFile << "  mov b *b       \n"; // b = active stack number
    oFile << "  lda 16384      \n";
    oFile << "  add            \n"; // convert to active stack pointer pointer
    oFile << "  mov a *a       \n"; // convert from pointer pointer to just normal stack pointer
    oFile << "  pop b          \n"; // retrieve value to push
    oFile << "  mov *a b       \n"; // store value at active stack pointer

    oFile << "  push a         \n";
    oFile << "  lda 1          \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  pop a          \n"; // basically, b = 1

    oFile << "  add            \n"; // add one to active stack pointer
    oFile << "  push a         \n";
    oFile << "  mov b 0x4000   \n";
    oFile << "  mov b *b       \n"; // b = active stack number
    oFile << "  lda 16384      \n";
    oFile << "  add            \n"; // a = active stack pointer pointer
    oFile << "  pop b          \n"; // b = incremented stack pointer
    oFile << "  mov *a b       \n"; // store incremented stack pointer

    oFile << "  jsr __verifySP \n"; // verify
    oFile << "  ret            \n";

    // pop a value off of a given stack into A
    oFile << "__pop:           \n";
    oFile << "  mov b 0x4000   \n";
    oFile << "  mov b *b       \n"; // b is active stack number
    oFile << "  lda 16384      \n";
    oFile << "  add            \n"; // a is active stack pointer pointer

    oFile << "  push a         \n";
    oFile << "  lda 1          \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  pop a          \n"; // b = 1
    oFile << "  sub            \n"; // subtract 1 from active stack pointer

    oFile << "  push a         \n";

    oFile << "  mov b 0x4000   \n";
    oFile << "  mov b *b       \n";
    oFile << "  lda 16384      \n";
    oFile << "  add            \n"; // again we now have active stack pointer pointer

    oFile << "  pop b          \n";
    oFile << "  mov *a b       \n"; // and we store the edcremented one

    oFile << "  mov a *a       \n"; // so now we can retrieve the popped value
    oFile << "  push a         \n";

    oFile << "  jsr __verifySP \n";
    oFile << "  pop a          \n";
    oFile << "  ret            \n";

    // verify that the active stack pointer is valid
    oFile << "__verifySP:          \n";
    oFile << "  mov a 0x4000       \n";
    oFile << "  mov a *a           \n";
    oFile << "  jz __verifySPOne   \n";
    oFile << "  push a             \n";
    oFile << "  lda 1              \n";
    oFile << "  push a             \n";
    oFile << "  pop b              \n";
    oFile << "  pop a              \n";
    oFile << "  sub                \n";
    oFile << "  push a             \n";
    oFile << "  jz __verifySPTwo   \n";
    oFile << "  sub                \n";
    oFile << "  push a             \n";
    oFile << "  jz __verifySPThree \n";
    oFile << "  sub                \n";
    oFile << "  push a             \n";
    oFile << "  jz __verifySPFour  \n";
    oFile << "  jmp __invalidStack \n";

    oFile << "__verifySPOne:   \n";
    oFile << "  mov b 0x4001   \n";
    oFile << "  mov b *b       \n";
    oFile << "  lda 4096       \n";
    oFile << "  sub            \n";
    oFile << "  jz __overflow  \n";
    oFile << "  lda 1          \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  mov a 0x4001   \n";
    oFile << "  mov a *a       \n";
    oFile << "  add            \n";
    oFile << "  push a         \n";
    oFile << "  jz __underflow \n";
    oFile << "  ret            \n";

    oFile << "__verifySPTwo:   \n";
    oFile << "  mov b 0x4002   \n";
    oFile << "  mov b *b       \n";
    oFile << "  lda 8192       \n";
    oFile << "  sub            \n";
    oFile << "  jz __overflow  \n";
    oFile << "  lda 4095       \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  mov a 0x4002   \n";
    oFile << "  mov a *a       \n";
    oFile << "  add            \n";
    oFile << "  push a         \n";
    oFile << "  jz __underflow \n";
    oFile << "  ret            \n";

    oFile << "__verifySPThree: \n";
    oFile << "  mov b 0x4002   \n";
    oFile << "  mov b *b       \n";
    oFile << "  lda 12288      \n";
    oFile << "  sub            \n";
    oFile << "  jz __overflow  \n";
    oFile << "  lda 8191       \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  mov a 0x4002   \n";
    oFile << "  mov a *a       \n";
    oFile << "  add            \n";
    oFile << "  push a         \n";
    oFile << "  jz __underflow \n";
    oFile << "  ret            \n";

    oFile << "__verifySPFour:  \n";
    oFile << "  mov b 0x4002   \n";
    oFile << "  mov b *b       \n";
    oFile << "  lda 16384      \n";
    oFile << "  sub            \n";
    oFile << "  jz __overflow  \n";
    oFile << "  lda 12287      \n";
    oFile << "  push a         \n";
    oFile << "  pop b          \n";
    oFile << "  mov a 0x4002   \n";
    oFile << "  mov a *a       \n";
    oFile << "  add            \n";
    oFile << "  push a         \n";
    oFile << "  jz __underflow \n";
    oFile << "  ret            \n";

    //TODO individual pop functions
    //m? move this all to standard library and call it a dependancy

    oFile << "__not:         \n";
    oFile << "  push a       \n";
    oFile << "  jz __notZero \n";
    oFile << "  lda 0        \n";
    oFile << "  ret          \n";
    oFile << " __notZero:    \n";
    oFile << "  lda 1        \n";
    oFile << "  ret          \n";

    // begin initialisation of things that need initialising

    oFile << "__init:       \n";
    oFile << "  lda 0       \n"; // initialise stack pointers
    oFile << "  push a      \n";
    oFile << "  pop b       \n";
    oFile << "  lda 16385   \n";
    oFile << "  mov *a b    \n";

    oFile << "  lda 4096    \n";
    oFile << "  push a      \n";
    oFile << "  pop b       \n";
    oFile << "  lda 16386   \n";
    oFile << "  mov *a b    \n";

    oFile << "  lda 8192    \n";
    oFile << "  push a      \n";
    oFile << "  pop b       \n";
    oFile << "  lda 16387   \n";
    oFile << "  mov *a b    \n";

    oFile << "  lda 12288   \n";
    oFile << "  push a      \n";
    oFile << "  pop b       \n";
    oFile << "  lda 16388   \n";
    oFile << "  mov *a b    \n";

    oFile << "  lda 0       \n"; // set active stack to 0
    oFile << "  push a      \n";
    oFile << "  pop b       \n";
    oFile << "  lda 16384   \n";
    oFile << "  mov *a b    \n";

    oFile << "  jmp __start \n";

    // beginning of user code
    oFile << "__start:\n";

    uint64_t lineNumber = 0;

    for(Token tk : writeToFile){
        switch(tk.type){
            case TOK_separator:
                break;

            case TOK_add:
                oFile << "  jsr __pop  \n";
                oFile << "  push a     \n";
                oFile << "  pop b      \n";
                oFile << "  jsr __pop  \n";
                oFile << "  add        \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_sub:
                oFile << "  jsr __pop  \n";
                oFile << "  push a     \n";
                oFile << "  pop b      \n";
                oFile << "  jsr __pop  \n";
                oFile << "  sub        \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_equals:
                oFile << "  jsr __pop  \n";
                oFile << "  push a     \n";
                oFile << "  pop b      \n";
                oFile << "  jsr __pop  \n";
                oFile << "  sub        \n";
                oFile << "  jsr __not  \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_not :
                oFile << "  jsr __pop  \n";
                oFile << "  jsr __not  \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_stack_dup:
                oFile << "  jsr __pop  \n";
                oFile << "  jsr __push \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_stack_drop:
                oFile << "  jsr __pop \n";
                break;

            case TOK_stack_swap:
                oFile << "  jsr __pop  \n";
                oFile << "  push a     \n";
                oFile << "  pop b      \n";
                oFile << "  jsr __pop  \n";
                oFile << "  push a     \n";
                oFile << "  push b     \n";
                oFile << "  pop a      \n";
                oFile << "  jsr __push \n";
                oFile << "  pop a      \n";
                oFile << "  jsr __push \n";
                break;

            case TOK_switch_stack:
                oFile << "  jsr __pop \n";
                oFile << "  push a    \n";
                oFile << "  pop b     \n";
                oFile << "  lda 16384 \n";
                oFile << "  mov *a b  \n";
                break;

            case TOK_num_digit:
            case TOK_num_combo:
                oFile << "  lda " << tk.content << "\n";
                oFile << "  jsr __push              \n";
                break;

            case TOK_misc:
                if(tk.content.empty()){
                    break;
                }
            case TOK_error:
                std::cout << "error token with content: " + tk.content + "\n";
                // deliberate fall through
            case TOK_func_begin: // we dont want to have function processing tokens, only the calling of one
            case TOK_func_end:   // ""
            case TOK_func_decl:  // ""
            case TOK_misc_combo:
            default:
                std::cout << "unreachable token\n";
                std::cout << "type: " << tk.type << "\n";
                std::cout << "name: " << getTokenName(tk.type) << "\n";
                std::cout << "content: '" << tk.content << "'\n";
                return false;
                break;
        }

        ++lineNumber;
    }

    // end of user code
    oFile << "halt\n";

    oFile.close();

    return true;
}

#endif /* _CODEGEN_HPP */