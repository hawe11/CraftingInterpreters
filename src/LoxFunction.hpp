#ifndef Function_HPP_
#define Function_HPP_

#include "Callable.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include "Interpreter.hpp"
#include "Environment.hpp"


#include <memory>
#include <vector>
#include <string>
using std::shared_ptr;

class LoxFunction: public Callable{
    public:
        LoxFunction(shared_ptr<Function> declaration_, shared_ptr<Environment> closure_) : declaration(declaration_), closure(closure_)
        {}

        Object call(Interpreter& interpreter, std::vector<Object> arguments) override;
        int arity() override;
        std::string toString() override;

        shared_ptr<Function> declaration;
        shared_ptr<Environment> closure;
    private:

        void function(shared_ptr<Function> declaration, shared_ptr<Environment> closure);



};


#endif