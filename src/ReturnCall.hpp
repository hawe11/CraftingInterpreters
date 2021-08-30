#ifndef Return_HPP_
#define Return_HPP_

#include "Token.hpp"

#include <stdexcept>


class ReturnCall:  public std::runtime_error {

    public:
        Object value;
        ReturnCall(Object value_) : runtime_error(""), value(value_)
        {}

};



#endif