#pragma once

#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP

#include <vector>

#include "tokens.hpp"
#include "Tokens/tokenVerifications.hpp"
#include "Tokens/MainTokens.hpp"

// combine strings of number tokens into one number_combo token
bool combineNumberTokens(std::vector<Token>& inputList, std::vector<Token>& tkList){
    // token from the list
    Token listTok(TOK_error);
    // the token we use for the current loop iteration
    Token workingTok(TOK_num_combo);

    for(uint64_t i = 0; i < inputList.size(); ++i){
        listTok = inputList.at(i);

        if(listTok.type == TOK_num_digit){
            // workingTok.content(std::string) += listTok.content(std::string)
            //   effectively add one digit to the current number
            workingTok.content += listTok.content;

            // if next character is still within range
            if((i + 1) <= inputList.size()){
                // if next character is not another number then were done with this token and can add it to the list and start over
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

// not necessarily combine, more get rid of strings and replace them by a single token
bool removeDuplicateSeparators(const std::vector<Token>& inputList, std::vector<Token>& tkList){
    bool lastWasSep = false;

    for(uint64_t i = 0; i < inputList.size(); ++i){
        if(inputList.at(i).type == TOK_separator){
            // make sure no more than one in a row
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

// combine strings of misc tokens into misc_combo tokens. similar to combineNumberTokens but with TOK_misc instead of TOK_num_digit
bool combineMiscTokens(std::vector<Token>& inputList, std::vector<Token>& tkList){
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
    // instead we worry about list size 2, where the first token gets ignored in favour of the second with main behaviour
    // instead we just make the lists equal then return. this works
    if(inputList.size() == 2){
        tkList = {inputList[0], inputList[1]};
        return true;
    }

    // this is needed because if the very last token is associated with an extern call then it prevents it from being output
    bool lastWasExtern = false;

    // extern calls always come in a pair of two tokens, a TOK_call_extern followed by a TOK_num_combo, hence, we have a pair to ensure that
    Token tkPair[2] = {Token(TOK_misc), Token(TOK_misc)};

    for(uint64_t i = 0; i < inputList.size() - 2; ++i){
        tkPair[0] = inputList.at(i);
        tkPair[1] = inputList.at(i + 1);

        if(tkPair[0].type == TOK_call_extern){
            if(tkPair[1].type == TOK_num_combo){
                tkList.push_back(Token(TOK_call_extern, tkPair[1].content));
                i += 1;

                lastWasExtern = true;
            }
            else if(tkPair[1].type == TOK_misc_combo){
                tkList.push_back(Token(TOK_call_extern, tkPair[1].content));
                i += 1;

                lastWasExtern = true;
            }
            else{
                std::cout << "extern with no identifier immediately following\n";

                return false;
            }
        }
        else{
            tkList.push_back(tkPair[0]);
            lastWasExtern = false;
        }
    }

    if(!lastWasExtern){
        tkList.push_back(tkPair[1]);
    }

    return true;
}

#endif /* COMBINATIONS_HPP */
