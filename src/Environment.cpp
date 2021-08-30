#include "Environment.hpp"
#include "Token.hpp"
#include "RuntimeError.hpp"

#include <iostream>
#include <string>
#include <map>
#include <iterator>
#include <vector>
#include <utility>

using std::string;




void Environment::define(string name, Object value){
    values[name]=(value);
}

void Environment::assign(Token name,Object value) {
    auto found_element = values.find(name.lexeme);

    if(found_element != values.end())  {

        found_element->second=value;
        return;
    }

    if(enclosing!=nullptr) {
        enclosing->assign(name,value);
        return;
    }

    throw RuntimeError(name,
        "Undefined variable '" + name.lexeme + "'.");
}

Object Environment::get(Token name) {

    auto found_element = values.find(name.lexeme);
    if(found_element != values.end())  {

        return found_element->second;
    }

    if(enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable '"+ name.lexeme + "'.");
}


