#ifndef Environment_HPP_
#define Environment_HPP_

#include "Token.hpp"

#include <map>
#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

class Environment: public std::enable_shared_from_this<Environment> {

    public:
        Environment(): enclosing(nullptr) 
        {}

        Environment(shared_ptr<Environment> enclosing) : enclosing(enclosing)
        {}

        void define(string name, Object value);
        void assign(Token name, Object value);
        Object get(Token name);
        shared_ptr<Environment> enclosing;


    private:
        std::map<string, Object> values;

};


#endif