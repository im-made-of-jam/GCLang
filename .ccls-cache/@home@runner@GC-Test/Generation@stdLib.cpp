#include <iostream>
#include <unordered_map>

// getch is platform dependant
#ifdef WIN32
  #include <conio.h>

#else // must be unix
  #include <termios.h>

  #define getch() getchar()
#endif

#include <chrono>
#include <thread>

void _extern_0(){ // exit with value
    exitFlag = true;
    std::cout << stack.back() << std::endl;
    exit(stack.back());
    stack.pop_back();
}

void _extern_1(){ // print number
    std::cout << stack.back();
    stack.pop_back();
}

void _extern_2(){ // print char value (e.g. 65 -> 'A')
    std::cout << static_cast<char>(stack.back());
    stack.pop_back();
}

void _extern_3(){ // pause for specified amount of ms
    std::this_thread::sleep_for(std::chrono::milliseconds(stack.back()));
    stack.pop_back();
}

void _extern_4(){ // push char ascii code on stack from input
    stack.push_back(getch());
}