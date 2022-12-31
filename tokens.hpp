#pragma once

#ifndef _TOKENS_HPP
#define _TOKENS_HPP

#include <vector>
#include <string>

#include "tokens.hpp"

enum Tokens{
    TOK_misc          ,  // anything that isnt an error but also isnt another token
    TOK_misc_combo    ,  // any combination of any other misc tokens
    TOK_error         ,  // anything that is an error and therefore is not another token
    TOK_separator     ,  // ' '

    TOK_add           ,  // '+'   "1 5 +"  -> 6
    TOK_sub           ,  // '-'   "6 2 -"  -> 4
    TOK_mul           ,  // '*'   "6 3 *"  -> 18
    TOK_div           ,  // '/'   "18 6 /" -> 3
    TOK_mod           ,  // '%'   "18 5 %" -> 3
    TOK_equals        ,  // '='   "5 5 ="  -> 1   "5 4 =" -> 0
    TOK_not           ,  // '!'    0       -> 1    4      -> 0

    TOK_if_begin      ,  // '['
    TOK_if_else       ,  // '|'
    TOK_if_end        ,  // ']'

    TOK_loop_start    ,  // '('
    TOK_loop_end      ,  // ')'
    TOK_loop_break    ,  // '^'

    TOK_func_begin    ,  // '{'
    TOK_func_end      ,  // '}'
    TOK_func_decl     ,  // any misc_combo followed immediately by '{'
    TOK_func_call     ,  // any misc_combo which has previously been used in declaring a function

    TOK_num_digit     ,  // '0', '1', '2', etc.
    TOK_num_combo     ,  // e.g. "477563"

    TOK_string_begin  , // '"'
    TOK_string_char   , // any character between a string_begin token and a string_end token
    TOK_string_crun   , // a run of characters between an end and a start token
    TOK_string_end    , // '"' also, but the end of a string literal rather than the start

    TOK_stack_dup     ,  // "dup"
    TOK_stack_drop    ,  // "drop"
    TOK_stack_swap    ,  // '$'
    TOK_switch_stack  ,  // '&'
    TOK_data_move     ,  // '~'

    TOK_call_extern   ,  // '@'

    // pops the top element, then the next N elements, where N is the number that was on the top of the stack
    // this is then all put into a GCPointer struct
    // see: Processing/GCPointer.hpp
    TOK_make_pointer  ,  // '>'
    TOK_dereference   ,  // '<'

    TOK_token_argument,  // for things such as function decorators, or low level modifications to code generation
};

class Token{
  public:
    uint32_t type;
    std::string content;
    std::vector<Token> subTokens;

    Token(uint32_t type){
        this->type = type;
        this->content = "";
    }

    Token(uint32_t type, std::string content){
        this->type = type;
        this->content = content;
    }
};

#endif /* _TOKENS_HPP */
