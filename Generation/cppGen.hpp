#pragma once

#ifndef CPPGEN_HPP
#define CPPGEN_HPP

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

#include "tokens.hpp"

bool generateCpp(const std::string filename, std::vector<Token>& writeToFile, bool includeStdlib, const std::string stdLibPath, const std::string windDownPath){
    std::filesystem::remove(filename);

    std::ofstream oFile;

    oFile.open(filename);

    // basic setup lines for registers, the four stacks, the GCPointer struct etc.

    oFile << "#include <vector>\n";                // needed for stack
    oFile << "#include <cstdint>\n";               // needed for 'register' types
    oFile << "#include <iostream>\n";              // as much as it is unwanted, it is needed by GCPointer
    oFile << "\n";
    oFile << "bool exitFlag = false;\n";           // goes true when time to exit
    oFile << "uint64_t R_A = 0;\n";                // 'register' A
    oFile << "uint64_t R_B = 0;\n";                // 'register' B
    oFile << "\n";
    oFile << "std::vector<uint64_t> stacks[4] = {std::vector<uint64_t>(),\n";
    oFile << "                                   std::vector<uint64_t>(),\n";
    oFile << "                                   std::vector<uint64_t>(),\n";
    oFile << "                                   std::vector<uint64_t>()};\n"; // the main stacks
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

    // allocate some memory for stacks at the start so that they do not need to realloc at a later stage
    oFile << "stacks[0].reserve(262144);\n";
    oFile << "stacks[0].reserve(65536);\n";
    oFile << "stacks[0].reserve(65536);\n";
    oFile << "stacks[0].reserve(65536);\n";

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
                oFile << "// __TOK_add\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A + R_B);\n";
                oFile << "\n";
                break;
            }
            case TOK_sub:{
                oFile << "// __TOK_sub\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B - R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_mul:{
                oFile << "// __TOK_mul\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A * R_B);\n";
                oFile << "\n";
                break;
            }
            case TOK_div:{
                oFile << "// __TOK_div\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B / R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_mod:{
                oFile << "// __TOK_mod\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_B % R_A);\n";
                oFile << "\n";
                break;
            }
            case TOK_not:{
                oFile << "// __TOK_not\n";
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
                oFile << "// __TOK_equals\n";
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
                oFile << "// __TOK_call_extern\n";
                oFile << "  _extern_" + tk.content + "();\n";
                oFile << "\n";
                break;
            }

            case TOK_stack_dup:{
                oFile << "// __TOK_stack_dup\n";
                oFile << "  stacks[activeStack].push_back(stacks[activeStack].back());\n";
                oFile << "\n";
                break;
            }
            case TOK_stack_drop:{
                oFile << "// __TOK_stack_drop\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "\n";
                break;
            }
            case TOK_stack_swap:{
                oFile << "// __TOK_stack_swap\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  stacks[activeStack].push_back(R_A)\n;";
                oFile << "  stacks[activeStack].push_back(R_B)\n;";
                break;
            }
            case TOK_switch_stack:{
                oFile << "// __TOK_switch_stack\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  activeStack = R_A;\n";
                oFile << "\n";
                break;
            }
            case TOK_data_move:{
                oFile << "// __TOK_data_move\n";
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
                oFile << "// __TOK_loop_start\n";
                oFile << "  while(1){\n";
                oFile << "\n";
                break;
            }
            case TOK_loop_end:{
                oFile << "// __TOK_loop_end\n";
                oFile << "  }\n";
                oFile << "\n";
                break;
            }
            case TOK_loop_break:{
                oFile << "// __TOK_loop_break\n";
                oFile << "  break;";
                oFile << "\n";
                break;
             }

            case TOK_if_begin:{
                oFile << "// __TOK_i_begin\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  if(R_A){\n";
                oFile << "\n";
                break;
            }
            case TOK_if_else:{
                oFile << "// __TOK_if_else\n";
                oFile << "  }\n";
                oFile << "  else{\n";
                break;
            }
            case TOK_if_end:{
                oFile << "// __TOK_if_end\n";
                oFile << "  }\n";
            }

            case TOK_separator:{
                break; // do nothing
            }

            case TOK_make_pointer:{
                oFile << "// __TOK_make_pointer\n";
                oFile << "  R_A = stacks[activeStack].back();\n";
                oFile << "  stacks[activeStack].pop_back();\n";
                oFile << "  R_B = reinterpret_cast<uint64_t>(new GCPointer(R_A, &stacks[activeStack]));\n";
                oFile << "  stacks[activeStack].push_back(reinterpret_cast<uint64_t>(R_B));\n";
                oFile << "\n";
                break;
            }
            case TOK_dereference:{
                oFile << "// __TOK_dereference\n";
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
            case TOK_string_crun:{
                // the first character of a string should be the top element of the stack
                for(uint64_t i = 0; i < tk.content.size(); ++i){
                    oFile << "  stacks[activeStack].push_back('" << tk.content[tk.content.size() - i - 1] << "');\n";
                }
                break;
            }
            case TOK_string_end:{
                break; // nothing as of yet
            }

            case TOK_func_decl:{
                oFile << "// __TOK_func_decl\n";
                oFile << "  auto " << tk.content << " = [&](){\n";

                break;
            }
            case TOK_func_end:{
                oFile << "// __TOK_func_end\n";
                oFile << "  };\n";

                break;
            }
            case TOK_func_call:{
                oFile << "// __TOK_func_call\n";
                oFile << "  " << tk.content << "();\n";

                break;
            }

            case TOK_error:{
                std::cout << "error token with content: " + tk.content + "\n";
                [[fallthrough]];
            }
            case TOK_string_char:
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


#endif /* CPPGEN_HPP */