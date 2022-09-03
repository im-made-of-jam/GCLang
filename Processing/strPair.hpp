#pragma once

#ifndef _STRPAIR_HPP
#define _STRPAIR_HPP

struct strPair{
    std::string first;
    std::string second;

    void clear(){
        first = "";
        second = "";
    }
};

#endif /* _STRPAIR_HPP */