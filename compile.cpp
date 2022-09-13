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

    // tokenising expects one honking massive string rather than a vector of strings so now we sort that out
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
        std::cout << initialTokenList.size() << " tokens at first pass\n";
    }

    if(args.deDeDebug){
        std::cout << "last token:\n";
        std::cout << "  type: " << getTokenName(  initialTokenList.at(  initialTokenList.size() - 1).type);
        std::cout << "  content: '" << initialTokenList.at(  initialTokenList.size() - 1).content << "'\n";;
    }

    // all strings of separator tokens replaced by a single separator token
    std::vector<Token> separatorsSmallList;

    if(!removeDuplicateSeparators(initialTokenList, separatorsSmallList)){
        std::cout << "failed at whitespace aggregation\n";
        return false;
    }

    if(args.deDebug){
        std::cout << separatorsSmallList.size() << " with separators minimised\n";
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
        std::cout << miscCombined.size() << " tokens with miscellaneous combined\n";
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
        std::cout << numbersProcessedList.size() << " tokens with numbers combined\n";
    }

    miscCombined.clear();
    miscCombined.shrink_to_fit();

    // aliases of external functions are changed from words into @ symbols
    std::vector<Token> aliasesProcessedList;

    std::vector<strPair> aliasNames;
    if(!getAliases(args.CPPStdLibAliasPath, aliasNames)){
        std::cout << "could not generate aliases\n";
    }

    // this step also adds a call of the "exit" stdlib function at the end of the program
    if(!convertAliasesToCalls(numbersProcessedList, aliasesProcessedList, aliasNames)){
        std::cout << "failed at extern alias handling\n";

        return false;
    }

    if(args.deDebug){
        std::cout << aliasesProcessedList.size() << " tokens with aliases processed\n";
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
        std::cout << keywordsProcessed.size() << " tokens after keywords are processed\n";
    }

    aliasesProcessedList.clear();
    aliasesProcessedList.shrink_to_fit();

    std::vector<Token> functionsMade;
    
    if(!makeFunctions(keywordsProcessed, functionsMade, aliasNames)){
        std::cout << "failed at function generation\n";

        return false;
    }

    if(args.deDebug){
        std::cout << functionsMade.size() << " tokens after function handling\n";
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    if(!convertExternCalls(functionsMade, output)){
        std::cout << "failed at extern handling\n";

        return false;
    }

    if(args.deDebug){
        std::cout << output.size() << " tokens after extern tokenisation\n";
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    return true;
}