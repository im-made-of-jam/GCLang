#include <iostream>
#include <thread>
#include <cstdint>

int main(){
    uint64_t a = std::thread::hardware_concurrency();

    std::cout << a;
}