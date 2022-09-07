#pragma once

#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP

#include <vector>

#include "tokens.hpp"
#include "Tokens/tokenVerifications.hpp"
#include "Tokens/MainTokens.hpp"

bool combineNumberTokens(std::vector<Token>& inputList, std::vector<Token>& tkList){ // combine strings of number tokens into one number_combo token
    Token listTok(TOK_error);
    Token workingTok(TOK_num_combo);

    for(uint64_t i = 0; i < inputList.size(); ++i){
        listTok = inputList.at(i);

        if(listTok.type == TOK_num_digit){
            workingTok.content += listTok.content;

            if((i + 1) <= inputList.size()){
                if(inputList.at(i + 1).type != TOK_num_digit){
                    if(!verifyNumber(workingTok.content)){
                        return false;
                    }

                    tkList.push_back(workingTok);

                    workingTok = Token(TOK_num_combo);
                }
            }
        }
        else{
            tkList.push_back(listTok);
        }
    }

    return true;
}

bool removeDuplicateSeparators(const std::vector<Token>& inputList, std::vector<Token>& tkList){ // not necessarily combine more get rid of strings and replace them by a single token
    bool lastWasSep = false;

    for(uint64_t i = 0; i < inputList.size(); ++i){
        if(inputList.at(i).type == TOK_separator){
            if(lastWasSep){
                continue;
            }
            else{
                tkList.push_back(inputList.at(i));
                lastWasSep = true;
            }
        }
        else{
            tkList.push_back(inputList.at(i));
            lastWasSep = false;
        }
    }

    return true;
}

bool combineMiscTokens(std::vector<Token>& inputList, std::vector<Token>& tkList){ // combine strings of misc tokens into misc_combo tokens
    Token listTok(TOK_error);
    Token workingTok(TOK_misc_combo);

    for(uint64_t i = 0; i < inputList.size(); ++i){
        listTok = inputList.at(i);

        if(listTok.type == TOK_misc){
            workingTok.content += listTok.content;

            if((i + 1) <= inputList.size()){
                if(inputList.at(i + 1).type != TOK_misc){
                    tkList.push_back(workingTok);

                    workingTok = Token(TOK_misc_combo);
                }
            }
        }
        else{
            tkList.push_back(listTok);
        }
    }

    return true;
}

bool convertExternCalls(const std::vector<Token>& inputList, std::vector<Token>& tkList){
    // list size of one means only whitespace, so no need to worry about that case
    // instead we worry about list size 2, where the first token gets ignored in afvour of the second with main behaviour
    // instead we just make the lists equal then return. this works
    if(inputList.size() == 2){
        tkList = {inputList[0], inputList[1]};
        return true;
    }

    Token tkPair[2] = {Token(TOK_misc), Token(TOK_misc)};

    for(uint64_t i = 0; i < inputList.size() - 2; ++i){
        tkPair[0] = inputList.at(i);
        tkPair[1] = inputList.at(i + 1);

        if(tkPair[0].type == TOK_call_extern){
            if(tkPair[1].type == TOK_num_combo){
                tkList.push_back(Token(TOK_call_extern, tkPair[1].content));
                ++i;
            }
            else{
                std::cout << "extern with no identifier immediately following\n";

                return false;
            }
        }
        else{
            tkList.push_back(tkPair[0]);
        }
    }

    tkList.push_back(tkPair[1]);

    return true;
}

#endif /* COMBINATIONS_HPP */
