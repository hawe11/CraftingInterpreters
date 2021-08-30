#ifndef RuntimeError_HPP_
#define RuntimeError_HPP_

#include "Token.hpp"

#include <string>
#include <stdexcept>

using std::string;

class RuntimeError: public std::exception {
    public:
        Token token;
        string message;
        RuntimeError(Token token_, string message_) : token(token_),message(message_) {};
};


#endif