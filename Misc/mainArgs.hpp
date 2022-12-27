#pragma once

#include <string>

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

    bool callCPP = true; // call a c++ compiler after everything else is done, the default
    std::string CPPCompiler = "g++";

    bool includeCPPStdlib = true; // include the stdLib file at the top of the c++ file
    std::string CPPStdLibPath = "Built/Generation/stdLib.cpp"; // the path to the c++ standard library
    std::string CPPStdLibAliasPath = "Built/Generation/CPPStdLib.alias"; // the path to the c++ alias file
    std::string CPPWindDownPath = "Built/Generation/stdLibWindDown.cpp"; // the path to the c++ standard library wind down file

    // same as above, but switch "c++" for "bytecode"
    bool includeBCStdLib = true;
    std::string BCStdLibPath = "Build/Generation/stdLib.gcbc";
    std::string BCStdLibAliasPath = "Built/Generation/BytecodeStdLib.alias";
    std::string BCWindDownPath = "Build/Generation/stdLibWindDown.gcbc";

    // same as aboce but switch "bytecode" for "asm"
    bool includeASMStdLib = true;
    std::string ASMStdLibPath = "Build/Generation/stdLib.s";
    std::string ASMStdLibAliasPath = "Built/Generation/ASMStdLib.alias";
    std::string ASMWindDownPath = "Build/Generation/stdLibWindDown.s";
};