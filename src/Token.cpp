#include <string>
#include <iostream>

#include "Token.hpp"
#include "Callable.hpp"


using std::string;
using std::to_string;
using std::cout;

Token::Token(TokenType type,string lexeme,Object literal,int line)
 : type(type), lexeme(lexeme), literal(literal), line(line) {

 }

string Token::toString() {
     return to_string(type);
}

Object Object::make_str_object(string obj_str) {
    Object str_object;
    str_object.type = Object_str;
    str_object.obj_str=obj_str;
    return str_object;
}

Object Object::make_double_object(double obj_double) {
    Object double_object;
    double_object.type = Object_num;
    double_object.obj_double=obj_double;
    return double_object;
}

Object Object::make_nil_object() {
    Object nil_object;
    nil_object.type = Object_nil;
    nil_object.obj_null=true;
    return nil_object;
}

Object Object::make_bool_object(bool obj_bool) {
    Object bool_object;
    bool_object.type = Object_bool;
    bool_object.obj_bool=obj_bool;
    return bool_object;
}

Object Object::make_func_object(shared_ptr<Callable> func) {
    Object func_object;
    func_object.type = Object_fun;
    func_object.function = func;
    func_object.callableObject = true;
    return func_object;
}

string Object::toString() {
    switch (type)
    {
    case Object_nil:
        return "nil";
    case Object_bool:
        if(obj_bool) {
            return "1";
        } else {
            return "0";
        }
    case Object_str:
        return obj_str;    

    case Object_num:
        return to_string(obj_double);
    case Object_fun:
        return function->toString();
    default:
        return "default";
        break;
    }
}