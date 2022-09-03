#pragma once

#ifndef PROCESSING_GCPOINTER_HPP
#define PROCESSING_GCPOINTER_HPP

struct GCPointer{
    // the first element is the size of the array. this is always present, and will be 0 if it is the only element
    uint64_t* data; // this is an array, btu still has to be defined like a pointer

    GCPointer(uint64_t count, std::vector<uint64_t>* stack){
        // ensure stack is not empty, and the count is not 0
        if((stack->size() == 0) || (count == 0)){
            // but if it is then we simply store one data point, the count, which is 0
            data = new uint64_t[1]{0};
            return;
        }
        
        std::cout << "first size: " << stack->size() << "\n\n";

        // things seem fine so we make the array to the correct size
        // "count + 1" is used due to the first elemenbt of the array being used to store the count of the array
        data = new uint64_t[count + 1];

        std::cout << "count + 1 = " << count + 1 << "\n";
        
        // speaking of which...
        data[0] = count;

        for(uint64_t i = 0; i < count; ++i){
            // dont actually do anything to handlean underflow, we still want it to be an error, but we also want a somewhat reasonable message
            if(!stack->size()){
                std::cout << "stack underflow upon creating pointer struct\n";
            }

            // now we pop off of the top of the stack and remove the last object from the stack
            data[i + 1] = stack->back();
            stack->pop_back();
        }
    }

    ~GCPointer(){
        // this is very easy compared to construction
        delete[] data;
    }

    uint64_t getCount(){
        return data[0];
    }
};

#endif /* PROCESSING_GCPOINTER_HPP */