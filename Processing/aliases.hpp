#pragma once

#ifndef REPLACEMENTS_HPP
#define REPLACEMENTS_HPP

#include <vector>
#include <string>

#include "tokens.hpp"
#include "Misc/loadFile.hpp"
#include "Processing/strPair.hpp"

bool getAliases(const std::string& filename, std::vector<strPair>& aliasList){
    std::vector<std::string> lines;

    if(!loadFile(lines, filename)){
        return false;
    }

    std::string firstBuffer;
    std::string secondBuffer;

    strPair buffer;

    for(std::string line : lines){
        if(line.empty()){
            continue;
        }

        buffer.clear();

        firstBuffer = "";
        secondBuffer = "";

        for(uint32_t i = 0; i < line.size(); ++i){
            if(line.at(i) == ','){
                buffer.first = firstBuffer;

                buffer.second = line.substr(i + 1, line.size());

                break;
            }

            firstBuffer += line.at(i);

            if(i == (line.size() - 1)){
                std::cout << "no comma in stdLib.alias definition\n";

                return false;
            }

        }

        aliasList.push_back(buffer);
    }

    return true;
}

bool convertAliasesToCalls(std::vector<Token>& inputList, std::vector<Token>& tkList, std::vector<strPair>& aliasList){
    // for every token
    for(Token tk : inputList){
        // if it is a token with a word that is not defined
        if(tk.type == TOK_misc || tk.type == TOK_misc_combo){
            // check it against every string pair
            for(strPair pair : aliasList){
                // if the token is equal to the alias
                if(tk.content == pair.first){
                    // replace it with what we want
                    tk.type = TOK_num_combo;
                    tk.content = pair.second;
                    tkList.push_back(Token(TOK_call_extern, "@"));

                    break;
                }
            }
        }

        tkList.push_back(tk);
    }

    return true;
}

#endif /* REPLACEMENTS_HPP */
