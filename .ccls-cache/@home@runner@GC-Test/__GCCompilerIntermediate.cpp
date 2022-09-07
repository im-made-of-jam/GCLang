#include <vector>
#include <cstdint>
#include <iostream>

bool exitFlag = false;
uint64_t R_A = 0;
uint64_t R_B = 0;

std::vector<uint64_t> stacks[4] = {std::vector<uint64_t>(),
                                   std::vector<uint64_t>(),
                                   std::vector<uint64_t>(),
                                   std::vector<uint64_t>()};
uint64_t activeStack = 0;

struct GCPointer{uint64_t* data;GCPointer(uint64_t count, std::vector<uint64_t>* stack){if((stack->size() == 0) || (count == 0)){data = new uint64_t[1]{0};return;}data = new uint64_t[count + 1];data[0] = count;for(uint64_t i = 0; i < count; ++i){if(!stack->size()){std::cout << "stack underflow upon creating pointer struct\n";}data[i + 1] = stack->back();stack->pop_back();}}~GCPointer(){delete[] data;}uint64_t getCount(){return data[0];}};
GCPointer* GC_P;
// __BEGIN_STDLIB
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

// push char ascii code onto top of stack from input
void _extern_4(){
    R_A = getch();

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

// return the 0-index number of the currently active stack
void _extern_9(){
    stacks[activeStack].push_back(activeStack);
}

// like system() in C(++)
//   (just look at the last line of this function)
void _extern_10(){
    std::string call;

    GCPointer* GCP = reinterpret_cast<GCPointer*>(stacks[activeStack].back());

    stacks[activeStack].pop_back();

    for(uint64_t i = 0; i < (GCP->data[0]) - 1; ++i){
        call += static_cast<char>(stacks[activeStack].back() & 0xFF);
        stacks[activeStack].pop_back();
    }

    delete GCP;

    [[maybe_unused]]
    uint64_t unused = system(call.c_str());
}

// __BEGIN_MAIN
int main(int argc, char* argv[]){
stacks[0].reserve(262144);
stacks[0].reserve(65536);
stacks[0].reserve(65536);
stacks[0].reserve(65536);
// __TOK_func_decl
  auto puts = [&](){
// __TOK_dereference
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  GC_P = reinterpret_cast<GCPointer*>(R_A);
  for(uint64_t i = 0; i < GC_P->data[0]; ++i){
    stacks[activeStack].push_back(GC_P->data[GC_P->data[0] - (i)]);
  }
  stacks[activeStack].push_back(GC_P->data[0]);
  delete GC_P;
// __TOK_loop_start
  while(1){

// __TOK_stack_dup
  stacks[activeStack].push_back(stacks[activeStack].back());

  stacks[activeStack].push_back(0);

// __TOK_equals
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  if(R_A == R_B){
    stacks[activeStack].push_back(1);
  }
  else{
    stacks[activeStack].push_back(0);
  }

// __TOK_i_begin
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  if(R_A){

// __TOK_loop_break
  break;
// __TOK_if_end
  }
  stacks[activeStack].push_back(1);

// __TOK_sub
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  stacks[activeStack].push_back(R_B - R_A);

// __TOK_stack_swap
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  stacks[activeStack].push_back(R_A)
;  stacks[activeStack].push_back(R_B)
;// __TOK_call_extern
  _extern_2();

// __TOK_loop_end
  }

// __TOK_stack_drop
  stacks[activeStack].pop_back();

// __TOK_func_end
  };
// __TOK_call_extern
  _extern_6();

  stacks[activeStack].push_back('s');
  stacks[activeStack].push_back('e');
  stacks[activeStack].push_back('l');
  stacks[activeStack].push_back('p');
  stacks[activeStack].push_back('p');
  stacks[activeStack].push_back('a');
  stacks[activeStack].push_back(6);

// __TOK_make_pointer
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = reinterpret_cast<uint64_t>(new GCPointer(R_A, &stacks[activeStack]));
  stacks[activeStack].push_back(reinterpret_cast<uint64_t>(R_B));

// __TOK_func_call
  puts();
  stacks[activeStack].push_back(10);

// __TOK_call_extern
  _extern_2();

  stacks[activeStack].push_back('p');
  stacks[activeStack].push_back('p');
  stacks[activeStack].push_back('a');
  stacks[activeStack].push_back(3);

// __TOK_make_pointer
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = reinterpret_cast<uint64_t>(new GCPointer(R_A, &stacks[activeStack]));
  stacks[activeStack].push_back(reinterpret_cast<uint64_t>(R_B));

// __TOK_dereference
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  GC_P = reinterpret_cast<GCPointer*>(R_A);
  for(uint64_t i = 0; i < GC_P->data[0]; ++i){
    stacks[activeStack].push_back(GC_P->data[GC_P->data[0] - (i)]);
  }
  stacks[activeStack].push_back(GC_P->data[0]);
  delete GC_P;
  stacks[activeStack].push_back('r');
  stacks[activeStack].push_back('a');
  stacks[activeStack].push_back('e');
  stacks[activeStack].push_back('l');
  stacks[activeStack].push_back('c');
  stacks[activeStack].push_back(5);

// __TOK_make_pointer
  R_A = stacks[activeStack].back();
  stacks[activeStack].pop_back();
  R_B = reinterpret_cast<uint64_t>(new GCPointer(R_A, &stacks[activeStack]));
  stacks[activeStack].push_back(reinterpret_cast<uint64_t>(R_B));

// __TOK_call_extern
  _extern_10();

  stacks[activeStack].push_back(0);

// __TOK_call_extern
  _extern_0();

  stacks[activeStack].push_back(0);


// __BEGIN_EXTERN_WIND_DOWN
if(gcoutFile.is_open()){
    gcoutFile.flush();
    gcoutFile.close();
}
}