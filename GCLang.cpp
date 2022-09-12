// g++ ./GCLang.cpp -o ./Built/GCLang.exe -O2 -std=gnu++20 -I .
// g++ ./GCLang.cpp -o ./Built/GCLang -O2 -std=gnu++20 -I .


// most things should still work perfectly fine if this is built on a 32 bit machine
//  however there is no guarantee that GC pointers will work on anything other than 64 bit


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "Misc/loadFile.hpp"
#include "Misc/removeComments.hpp"
#include "Processing/aggregation.hpp"
#include "Processing/aliases.hpp"
#include "Processing/functions.hpp"
#include "Tokens/MainTokens.hpp"
#include "Generation/codeGen.hpp"

void printHelp(){
    std::cout << "GC compiler help\n";
    std::cout << "usage:\n";
    std::cout << "    GCLang inputfile outputfile\n";
    std::cout << "  or\n";
    std::cout << "    GCLang inputfile -o output file";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "if exactly two arguments are given then they are assumed to be in the format of input file first, then output file\n";
    std::cout << "\n";
    std::cout << "if any more than two arguments are to be added, then it is assumed that the first argument is the name fo the input file, and the output file must be specified\n";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "\n";
    std::cout << "arguments:\n";
    std::cout << "  -o\n";
    std::cout << "    specify the name of the otuput file.\n";
    std::cout << "    on windows, the string \".exe\" will automatically be added to the end of this\n";
    std::cout << "\n";
    std::cout << "  -S\n";
    std::cout << "  -standalone\n";
    std::cout << "    do not call a c++ compiler after completion of generation\n";
    std::cout << "\n";
    std::cout << "  -I\n";
    std::cout << "  -keep-intermediate\n";
    std::cout << "    keep the intermediate c++ file generated\n";
    std::cout << "\n";
    std::cout << "  -K\n";
    std::cout << "  -dont-overwrite\n";
    std::cout << "    do not proceed with compilation if the output file given with -o already exists\n";
    std::cout << "\n";
    std::cout << "  -C\n";
    std::cout << "  -cpp-compiler\n";
    std::cout << "    the c++ compiler that gets called\n";
    std::cout << "    GCC will be called by default\n";
    std::cout << "    0 - GCC\n";
    std::cout << "    1 - clang/LLVM\n";
    std::cout << "\n";
    std::cout << "  -T\n";
    std::cout << "  -dump-tokens\n";
    std::cout << "    print to stdout the number type of every token, and also the content\n";
    std::cout << "    contained within the token, but only once every processing step on the token\n";
    std::cout << "    has been completed\n";
    std::cout << "\n";
    std::cout << "  -h\n";
    std::cout << "    this message\n";
}

struct MainArgs{
    bool debug = false;
    bool deDebug = false;
    bool deDeDebug = false;

    std::string inputFilename = "";  // name of input file
    std::string intermediateName = "__GCCompilerIntermediate.cpp"; // name of intermediate file
    std::string outputFilename = ""; // name of output file

    bool deleteIntermediate = true; // delete any intermidiate files after we are done with them

    bool dumpTokens = false; // output every token's type and content once the last processing step has been completed

    bool doNotOverwrite = false; // do not overwrite the output file if it already exists

    bool generateCPP = true;
    bool generateBytecode = false;

    bool callCPP = true; // call a c++ compiler after everything else is done
    std::string CPPCompiler = "g++";

    bool includeCPPStdlib = true; // include the stdLib file at the top of the c++ file
    std::string CPPStdLibPath = "Built/Generation/stdLib.cpp"; // the path to the c++ standard library
    std::string CPPStdLibAliasPath = "Built/Generation/CPPStdLib.alias"; // the path to the c++ alias file
    std::string CPPWindDownPath = "Built/Generation/stdLibWindDown.cpp"; // the path to the c++ standard library wind down file

    // same as above, but switch "c++" for "pyasm"
    bool includeBCStdLib = true;
    std::string BCStdLibPath = "Build/Generation/stdLib.pyasm";
    std::string BCStdLibAliasPath = "Built/Generation/ASMStdLib.alias";
    std::string BCWindDownPath = "Build/Generation/stdLibWindDown.pyasm";
};

int main(int argc, char* argv[]){
    MainArgs args;

    if(argc < 3){
        if(std::string{"-h"}.compare(argv[1]) == 0){
            printHelp();

            return 0;
        }

        std::cout << "no arguments or not enough arguments \n";
        std::cout << "add argument -h for help\n";

        return -1;
    }

    if(argc == 3){ // assume format is: "GCLang.exe inputFilename outputFilename" or "./GCLang inputFilename outputFilename"
        args.inputFilename = std::string{argv[1]};
        args.outputFilename = std::string{argv[2]};
    }
    else{ // assume the first argument is the input filename and all else should be processed properly
        args.inputFilename = argv[1];

        for(uint32_t i = 2; i < argc; ++i){
            // specify an output file
            if(std::string{"-o"}.compare(argv[i]) == 0){
                if(i >= (argc - 1)){
                    std::cout << "no name specified for output file after -o";

                    return -1;
                }
                else{
                    args.outputFilename = argv[i + 1];
                    i += 1;
                }
            }


            // -S for standalone, i.e. act alone and dont call a c++ compiler afterwards
            else if(std::string{"-S"}.compare(argv[i]) == 0){
                args.callCPP = false;
            }
            // same as above but explicit
            else if(std::string{"-standalone"}.compare(argv[i]) == 0){
                args.callCPP = false;
            }


            // -I for keeping intermediate file
            else if(std::string{"-I"}.compare(argv[i]) == 0){
                args.deleteIntermediate = false;
            }
            // same as above but more explicit
            else if(std::string{"-keep-intermediate"}.compare(argv[i]) == 0){
                args.deleteIntermediate = false;
            }


            // -K for keeping an output file that already exists and not overwriting it
            else if(std::string{"-K"}.compare(argv[i]) == 0){
                args.doNotOverwrite = true;
            }
            // same as above but more explicit
            else if(std::string{"-dont-overwrite"}.compare(argv[i]) == 0){
                args.doNotOverwrite = true;
            }


            // -C for which compiler to use
            else if(std::string{"-C"}.compare(argv[i]) == 0){
                if(i >= (argc - 1)){
                    std::cout << "no c++ compiler specified after -C\n";

                    return -1;
                }

                // "0" for GCC
                if(std::string{"0"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }
                else if(std::string{"gcc"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }
                else if(std::string{"g++"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }

                // "1" for clang
                else if(std::string{"1"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"clang"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"clang++"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"llvm"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else{
                    std::cout << "invalid c++ compiler number specified:\n";
                    std::cout << "  GCC:\n";
                    std::cout << "    0\n";
                    std::cout << "    gcc\n";
                    std::cout << "    g++\n";
                    std::cout << "\n";
                    std::cout << "  LLVM clang:\n";
                    std::cout << "    1\n";
                    std::cout << "    clang\n";
                    std::cout << "    clang++\n";
                    std::cout << "    llvm\n";

                    return -1;
                }
            }
            // same as above but more explicit
            else if(std::string{"-cpp-compiler"}.compare(argv[i]) == 0){
                if(i >= (argc - 1)){
                    std::cout << "no c++ compiler specified after -C\n";

                    return -1;
                }

                // "0" for GCC
                if(std::string{"0"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }
                else if(std::string{"gcc"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }
                else if(std::string{"g++"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"g++"};
                }

                // "1" for clang
                else if(std::string{"1"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"clang"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"clang++"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else if(std::string{"llvm"}.compare(argv[i + 1]) == 0){
                    args.CPPCompiler = std::string{"clang++"};
                }
                else{
                    std::cout << "invalid c++ compiler number specified:\n";
                    std::cout << "  GCC:\n";
                    std::cout << "    0\n";
                    std::cout << "    gcc\n";
                    std::cout << "    g++\n";
                    std::cout << "\n";
                    std::cout << "  LLVM clang:\n";
                    std::cout << "    1\n";
                    std::cout << "    clang\n";
                    std::cout << "    clang++\n";
                    std::cout << "    llvm\n";

                    return -1;
                }
            }

            // -A for generating assembly instead of c++
            else if(std::string{"-A"}.compare(argv[i]) == 0){
                args.callCPP = false;
                args.generateCPP = false;
                args.generateBytecode = true;
            }

            // -T for dumping tokens when done
            else if(std::string{"-T"}.compare(argv[i]) == 0){
                args.dumpTokens = true;
            }
            // same as above but more explicit
            else if(std::string{"-dump-tokens"}.compare(argv[i]) == 0){
                args.dumpTokens = true;
            }

            // -h for help
            else if(std::string{"-h"}.compare(argv[i]) == 0){
                printHelp();

                return 0;
            }

            else if(std::string{"-D"}.compare(argv[i]) == 0){
                args.debug = true;
            }

            else if(std::string{"-DD"}.compare(argv[i]) == 0){
                args.deDebug = true;
            }
            else if(std::string{"-DDD"}.compare(argv[i]) == 0){
                args.deDeDebug = true;
            }
        }
    }

    // check to see if output file is specified
    if(std::string{""}.compare(args.outputFilename) == 0){
        std::cout << "no output file specified\n";

        return -1;
    }

    // check for output file already existng if "-K" argument is present
    if(args.doNotOverwrite){
        std::string testForThis = args.outputFilename;

        #ifdef WIN32 // windows will only excecute files ending in ".exe" or ".com", other platforms are more lenient
        testForThis += ".exe";
        #endif

        if(std::filesystem::exists(testForThis)){
            std::cout << "output file already exists with -K argument selected\n";

            return -1;
        }
    }

    // raw lines from input file
    std::vector<std::string> lines;

    if(!loadFile(lines, args.inputFilename)){
        std::cout << "could not open input file";
        return -1;
    }

    if(lines.size() == 0){
        std::cout << "input file is empty\n";

        return 0;
    }

    if(args.deDebug){
        std::cout << lines.size() << " lines\n";
    }

    std::vector<std::string> commentsRemoved;

    if(!removeComments(lines, commentsRemoved, '#')){
        std::cout << "could not remove comments\n";

        return -1;
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

        return -1;
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
        return -1;
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
        return -1;
    }

    if(args.deDebug){
        std::cout << miscCombined.size() << " tokens with miscellaneous combined\n";
    }

    separatorsSmallList.clear();
    separatorsSmallList.shrink_to_fit();

    std::vector<Token> stringRunsMade;

    if(!stringAggregation(miscCombined, stringRunsMade)){
        std::cout << "failed at string aggregation\n";

        return -1;
    }

    miscCombined.clear();
    miscCombined.shrink_to_fit();

    // strings of number tokens are combined into single tokens similar to misc processing
    std::vector<Token> numbersProcessedList;

    if(!combineNumberTokens(stringRunsMade, numbersProcessedList)){
        std::cout << "failed at number combining\n";

        return -1;
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

        return -1;
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

        return -1;
    }

    if(args.deDebug){
        std::cout << keywordsProcessed.size() << " tokens after keywords are processed\n";
    }

    aliasesProcessedList.clear();
    aliasesProcessedList.shrink_to_fit();

    std::vector<Token> functionsMade;
    
    if(!makeFunctions(keywordsProcessed, functionsMade, aliasNames)){
        std::cout << "failed at function generation\n";

        return -1;
    }

    if(args.deDebug){
        std::cout << functionsMade.size() << " tokens after function handling\n";
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    // external functions are changed from being misc tokens into call_extern tokens
    std::vector<Token> externsProcessed;

    if(!convertExternCalls(functionsMade, externsProcessed)){
        std::cout << "failed at extern handling\n";

        return -1;
    }

    if(args.deDebug){
        std::cout << externsProcessed.size() << " tokens after extern tokenisation\n";
    }

    keywordsProcessed.clear();
    keywordsProcessed.shrink_to_fit();

    if(args.dumpTokens){
        for(Token k : externsProcessed){
            std::cout << "type: " << getTokenName(k.type) << '\n';
            std::cout << "content: \"" << k.content << "\"\n\n";
        }
    }


    if(args.generateCPP){
        if(!generateCpp(args.intermediateName, externsProcessed, args.includeCPPStdlib, args.CPPStdLibPath, args.CPPWindDownPath)){
            std::cout << "code gen failure\n";

            return -1;
        }
    }

    if(args.generateBytecode){
        std::string outputName = std::string{args.outputFilename} + ".gcb";

        if(!generateBytecode(outputName, externsProcessed, args.includeBCStdLib, args.BCStdLibPath, args.BCWindDownPath)){
            std::cout << "code gen failure\n";

            return -1;
        }
    }

    std::cout << longLine << '\n';

    if(args.callCPP){
        std::string sysString = args.CPPCompiler + ' ';

        sysString += std::string{args.intermediateName} + " -o " + args.outputFilename;

        #ifdef WIN32 // windows is funny about executables, and needs the correct file extension. linux doesnt care
        sysString += ".exe";
        #endif

        sysString += " -Wall";

        std::cout << "calling: " << sysString + '\n';

        std::cout << std::flush;

        // dont care about the return value of this system() call (yet) so supress the warning (for now)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-result"
        system(sysString.c_str());
        #pragma GCC diagnostic pop
    }

    if(args.deleteIntermediate){
        std::filesystem::remove(args.intermediateName);
    }

    return 0;
}