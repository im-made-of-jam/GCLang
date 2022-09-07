#pragma once

#ifndef STRPAIR_HPP
#define STRPAIR_HPP

struct strPair{
    std::string first;
    std::string second;

    void clear(){
        first = "";
        second = "";
    }
};

#endif /* STRPAIR_HPP */
