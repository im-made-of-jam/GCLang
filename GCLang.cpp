// g++ ./GCLang.cpp -o ./Built/GCLang.exe -O2 -std=gnu++20 -I .
// g++ ./GCLang.cpp -o ./Built/GCLang -O2 -std=gnu++20 -I .

// most things should still work perfectly fine if this is built on a 32 bit machine
//  however there is no guarantee that GC pointers will work on anything other than 64 bit

#define UNICODE

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "Misc/loadFile.hpp"
#include "Misc/removeComments.hpp"
#include "Misc/mainArgs.hpp"
#include "Processing/aggregation.hpp"
#include "Processing/aliases.hpp"
#include "Processing/functions.hpp"
#include "Tokens/MainTokens.hpp"
#include "Generation/codeGen.hpp"
#include "compile.cpp"

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
    std::cout << "  -h\n";
    std::cout << "    print this message\n";
    std::cout << "\n";
    std::cout << "  -o\n";
    std::cout << "    specify the name of the output file.\n";
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
    std::cout << "  -A\n";
    std::cout << "    generate assembly instead of c++\n";
    std::cout << "\n";
    std::cout << "  -B\n";
    std::cout << "    generate GC bytecode instead of c++\n";
    std::cout << "\n";
    std::cout << "  -D\n";
    std::cout << "  -DD\n";
    std::cout << "  -DDD\n";
    std::cout << "    various levels of debugging information\n";
    std::cout << "\n";
}

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
                args.generateASM = true;
            }

            // -B for generating bytecode instead of c++
            else if(std::string{"-B"}.compare(argv[i]) == 0){
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

    std::vector<Token> readyForGen;

    if(!tokeniseAndProcess(args, readyForGen)){
        std::cout << "tokeniseAndProcess() failure\n";

        return -1;
    }

    if(args.dumpTokens){
        for(Token k : readyForGen){
            std::cout << "type: " << getTokenName(k.type) << '\n';
            std::cout << "content: \"" << k.content << "\"\n\n";
        }
    }


    if(args.generateCPP){
        if(!generateCpp(args.intermediateName, readyForGen, args.includeCPPStdlib, args.CPPStdLibPath, args.CPPWindDownPath)){
            std::cout << "code gen failure\n";

            return -1;
        }
    }

    if(args.generateBytecode){
        std::string outputName = std::string{args.outputFilename} + ".gcb";

        if(!generateBytecode(outputName, readyForGen, args.includeBCStdLib, args.BCStdLibPath, args.BCWindDownPath)){
            std::cout << "code gen failure\n";

            return -1;
        }
    }

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

    if(args.generateASM){
        std::string outputName = std::string(args.outputFilename) + ".S";

        if(!generateASM(outputName, readyForGen, args.includeASMStdLib, args.ASMStdLibPath, args.ASMWindDownPath)){
            std::cout << "code gen failure\n";

            return -1;
        }
    }

    if(args.deleteIntermediate){
        std::filesystem::remove(args.intermediateName);
    }

    return 0;
}