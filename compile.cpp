#pragma once

#include "Misc/loadFile.hpp"
#include "Misc/removeComments.hpp"
#include "Processing/aggregation.hpp"
#include "Processing/aliases.hpp"
#include "Processing/functions.hpp"
#include "Tokens/MainTokens.hpp"
#include "Generation/codeGen.hpp"
#include "Misc/mainArgs.hpp"

bool tokeniseAndProcess(MainArgs args, std::vector<Token>& output){
    std::vector<std::string> lines;

    if(!loadFile(lines, args.inputFilename)){
        std::cout << "could not open input file";
        return -1;
    }

    if(lines.size() == 0){
        std::cout << "input file is empty\n";

        return false;
    }

    if(args.deDebug){
        std::cout << lines.size() << " lines\n";
    }

    std::vector<std::string> commentsRemoved;

    if(!removeComments(lines, commentsRemoved, '#')){
        std::cout << "could not remove comments\n";

        return false;
    }

    if(args.deDebug){
        std::cout << commentsRemoved.size() << " lines with comments removed\n";
    }

    // clear vector of lines to free up some memory
    lines.clear();
    lines.shrink_to_fit();

    // tokenising expects one massive string rather than a vector of strings so now we sort that out
    std::string longLine;

    for(std::string str : commentsRemoved){
        longLine += (str + ' ');
    }

    commentsRemoved.clear();
    commentsRemoved.shrink_to_fit();

    // list of every token without any additional passes
    std::vector<Token> initialTokenList;

    // initial tokenisation
    if(!tokenise(initialTokenList, longLine)){
        std::cout << "failed at tokenisation\n";

        std::cout << initialTokenList.at(initialTokenList.size() - 1).content;

        return false;
    }

    if(args.deDebug){
        std::cout << initialTokenList.size() << " tokens at initial tokenisation - - - - - - - - - -\n";
    }

    if(args.deDeDebug){
        for(Token tk : initialTokenList){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    // all strings of separator tokens replaced by a single separator token
    std::vector<Token> separatorsSmallList;

    if(!removeDuplicateSeparators(initialTokenList, separatorsSmallList)){
        std::cout << "failed at whitespace aggregation\n";
        return false;
    }

    if(args.deDebug){
        std::cout << separatorsSmallList.size() << " with separators minimised - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : separatorsSmallList){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    // memory freeing operations
    initialTokenList.clear();
    initialTokenList.shrink_to_fit();

    // strings of misc are combined into one token with longer content instead of many tokens with small content
    std::vector<Token> miscCombined;

    if(!combineMiscTokens(separatorsSmallList, miscCombined)){
        std::cout << "failed at misc aggregation\n";
        return false;
    }

    if(args.deDebug){
        std::cout << miscCombined.size() << " tokens with miscellaneous combined - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : miscCombined){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    separatorsSmallList.clear();
    separatorsSmallList.shrink_to_fit();

    std::vector<Token> stringRunsMade;

    if(!stringAggregation(miscCombined, stringRunsMade)){
        std::cout << "failed at string aggregation\n";

        return false;
    }

    miscCombined.clear();
    miscCombined.shrink_to_fit();

    // strings of number tokens are combined into single tokens similar to misc processing
    std::vector<Token> numbersProcessedList;

    if(!combineNumberTokens(stringRunsMade, numbersProcessedList)){
        std::cout << "failed at number combining\n";

        return false;
    }

    if(args.deDebug){
        std::cout << numbersProcessedList.size() << " tokens with numbers combined - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : numbersProcessedList){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    miscCombined.clear();
    miscCombined.shrink_to_fit();

    // aliases of external functions are changed from words into @ symbols
    std::vector<Token> aliasesProcessedList;

    std::vector<strPair> aliasNames;
    if(!getAliases(args.CPPStdLibAliasPath, aliasNames)){
        std::cout << "could not generate aliases\n";
    }

    if(!convertAliasesToCalls(numbersProcessedList, aliasesProcessedList, aliasNames)){
        std::cout << "failed at extern alias handling\n";

        return false;
    }

    if(args.deDebug){
        std::cout << aliasesProcessedList.size() << " tokens with aliases processed - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : aliasesProcessedList){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    numbersProcessedList.clear();
    numbersProcessedList.shrink_to_fit();

    // keywords and aliases are turned from misc tokens into their respective proper tokens
    std::vector<Token> keywordsProcessed;

    if(!keyWordsToTokens(aliasesProcessedList, keywordsProcessed)){
        std::cout << "failed at keyword processing\n";

        return false;
    }

    if(args.deDebug){
        std::cout << keywordsProcessed.size() << " tokens after keywords are processed - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : keywordsProcessed){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    aliasesProcessedList.clear();
    aliasesProcessedList.shrink_to_fit();

    std::vector<Token> functionsMade;

    if(!makeFunctions(keywordsProcessed, functionsMade, aliasNames)){
        std::cout << "failed at function generation\n";

        return false;
    }

    if(args.deDebug){
        std::cout << functionsMade.size() << " tokens after function handling - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : functionsMade){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    if(!convertExternCalls(functionsMade, output)){
        std::cout << "failed at extern handling\n";

        return false;
    }

    if(args.deDebug){
        std::cout << output.size() << " tokens after extern tokenisation - - - - - - - - - -\n";
    }
    if(args.deDeDebug){
        for(Token tk : output){
            std::cout << "token of type:  " << getTokenName(tk.type) << '\n';
            std::cout << "  with content: " << tk.content << '\n';
        }
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    return true;
}