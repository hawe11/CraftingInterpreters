#ifndef Callable_HPP_
#define Callable_HPP_


#include "Token.hpp"

#include <vector>
#include <string>
#include <memory>

class Interpreter;

class Callable {
    public:
        virtual Object call(Interpreter& interpreter, std::vector<Object> arguments) = 0;
        virtual int arity() = 0;
        virtual std::string toString() = 0;

        virtual ~Callable() = default;
};



#endif
