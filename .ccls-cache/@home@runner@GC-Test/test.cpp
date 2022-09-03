#include <cstdint>
#include <iostream>
#include <vector>

#include "Processing/GCPointer.hpp"

int main() {
    uint64_t a = 0;
    std::vector<uint64_t> b = {5, 6, 7, 8, 9};

    a = reinterpret_cast<uint64_t>(new GCPointer(2, &b));

    std::cout << a;
 
    std::cout << "size = " << reinterpret_cast<GCPointer *>(a)->data[0];

    for(uint64_t i = 0; i < reinterpret_cast<GCPointer *>(a)->data[0]; ++i){
        std::cout << "\n" << reinterpret_cast<GCPointer *>(a)->data[i + 1];
    }
}