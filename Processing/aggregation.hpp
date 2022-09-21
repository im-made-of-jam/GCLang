#pragma once

#ifndef AGGREGATION_HPP
#define AGGREGATION_HPP

#include "tokens.hpp"

// turns strings of string_char tokens into string_crun tokens
bool stringAggregation(const std::vector<Token>& inputList, std::vector<Token>& tkList){
    bool inString = false;

    Token working(TOK_string_crun);
    
    for(Token tk : inputList){
        if(tk.type == TOK_string_char){
            inString = true;

            working.content += tk.content;

            continue;
        }

        if(inString){
            inString = false;

            tkList.push_back(working);

            working = Token(TOK_string_crun);

            continue;
        }

        tkList.push_back(tk);
    }

    return true;
}

// turns strings that are keywords e.g. "dup" into their respective tokens
bool keyWordsToTokens(const std::vector<Token>& inputList, std::vector<Token>& tkList){
    for(Token tk : inputList){
        if(tk.type == TOK_misc || tk.type == TOK_misc_combo){
            if(tk.content == "dup"){
                tkList.push_back(Token(TOK_stack_dup, "dup"));
            }
            else if(tk.content == "break"){
                tkList.push_back(Token(TOK_loop_break, "<"));
            }
            else if(tk.content == "if"){
                tkList.push_back(Token(TOK_if_begin, "["));
            }
            else if(tk.content == "else"){
                tkList.push_back(Token(TOK_if_else, "|"));
            }
            else if(tk.content == "endif"){
                tkList.push_back(Token(TOK_if_end, "]"));
            }
            else if(tk.content == "loop"){
                tkList.push_back(Token(TOK_loop_start, "("));
            }
            else if(tk.content == "endloop"){
                tkList.push_back(Token(TOK_loop_end, ")"));
            }
            else if(tk.content == "exit"){
                tkList.push_back(Token(TOK_misc, "@0"));
            }
            else if(tk.content == "drop"){
                tkList.push_back(Token(TOK_stack_drop, "drop"));
            }
            else if(tk.content == "swap"){
                tkList.push_back(Token(TOK_stack_swap, "$"));
            }
            else{
                tkList.push_back(tk);
            }
        }
        else{
            tkList.push_back(tk);
        }
    }

    return true;
}

#endif /* AGGREGATION_HPP */
