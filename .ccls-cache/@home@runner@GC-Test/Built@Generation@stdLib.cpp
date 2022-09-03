// getch is platform dependant
#ifdef WIN32
    #include <conio.h>

#else // probably unix, dont really care otherwise
    #include <termios.h>

    // not perfect but this will do for now
    #define getch() getchar()
#endif

#include <iostream>
#include <fstream>
#include <ios>
#include <chrono>
#include <thread>

bool outputToStdout = true;

std::ofstream gcoutFile; // output stream for "GCOut.gco"



// __BEGIN_EXTERN_INTERNALS

// open gcout if needed
void _extern_io_check_init(){
    if(!gcoutFile.is_open()){
        gcoutFile.open("GCOut.gco", std::ios::binary);
    }
}

// flush the current output stream
void _extern_cout_flush(){
    if(outputToStdout){
        std::cout << std::flush;
    }
    else{
        gcoutFile << std::flush;
    }
}

// print an integer to the current output stream
void _extern_cout(int content){
    if(outputToStdout){
        std::cout << content;
    }
    else{
        gcoutFile << content;
    }
}

// and also for chars to allow @2 to work properly
void _extern_cout_char(char content){
    if(outputToStdout){
        std::cout << content;
    }
    else{
        gcoutFile << content;
    }
}


// __BEGIN_EXTERNS

// exit with value from top of stack
void _extern_0(){
    exitFlag = true;
    exit(stacks[activeStack].back());
    stacks[activeStack].pop_back();
}

// print number from top of stack
void _extern_1(){
    _extern_io_check_init();
    _extern_cout(stacks[activeStack].back());
    stacks[activeStack].pop_back();
}

// print char value (e.g. 65 -> 'A') of number on top of stack
void _extern_2(){
    _extern_io_check_init();
    _extern_cout_char(static_cast<char>(stacks[activeStack].back()));
    stacks[activeStack].pop_back();
}

// pause for N milliseconds where N is the number on the top of the stack
void _extern_3(){
    std::this_thread::sleep_for(std::chrono::milliseconds(stacks[activeStack].back()));
    stacks[activeStack].pop_back();
}

// push char ascii code on stack from input
void _extern_4(){
    R_A += getch();

    #ifdef WIN32
        // since only first char from input is recieved, and windows uses CRLF
        // (ASCII codes 13, then 10), but UNIX systems use only LF (ASCII code 10)
        // we translate to maintain compatibility
        if(R_A == 13){
            R_A = 10;
        }
    #endif

    stacks[activeStack].push_back(R_A);
}

// push size of active stack (before call) onto active stack
void _extern_5(){
    stacks[activeStack].push_back(stacks[activeStack].size());
}

// switch output stream to stdout
void _extern_6(){
    _extern_io_check_init();
    outputToStdout = true;
}

// switch output stream to a file called "GCOut.gco"
void _extern_7(){
    _extern_io_check_init();
    outputToStdout = false;
}

// flush the current output buffer
void _extern_8(){
    _extern_io_check_init();

    _extern_cout_flush();
}

// print string
void _extern_9(){
    _extern_io_check_init();

    // dereference the pointer and remove it from the stack
    R_A = stacks[activeStack].back();
    stacks[activeStack].pop_back();
    GC_P = reinterpret_cast<GCPointer*>(R_A);

    // then for every char in the dereferenced pointer from the end to the start, we print it
    for(uint64_t i = 0; i < GC_P->data[0]; ++i){
        std::cout << static_cast<char>((GC_P->data[GC_P->data[0] - i]));
    }
    
    delete GC_P; // then free the memory
}