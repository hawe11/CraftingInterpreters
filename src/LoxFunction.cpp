#include "LoxFunction.hpp"
#include "Environment.hpp"
#include "ReturnCall.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

using std::shared_ptr;

void LoxFunction::function(shared_ptr<Function> declaration_, shared_ptr<Environment> closure_) {
    declaration=declaration_;
    closure=closure_;
}

Object LoxFunction::call(Interpreter& interpreter, std::vector<Object> arguments) {
    shared_ptr<Environment> environment(new Environment{closure});

    for (int i=0;i<declaration->params.size();++i) {
        environment->define(declaration->params.at(i).lexeme, arguments.at(i));
    }
    try{
        interpreter.executeBlock(declaration->body, environment);
    } catch (ReturnCall returnValue) {

        return returnValue.value;
    }
    return Object::make_nil_object();
}

int LoxFunction::arity() {
    return declaration->params.size();
}

std::string LoxFunction::toString() {
    return "<fn "+ declaration->name.lexeme +" >";
}

