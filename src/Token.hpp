#ifndef Token_HPP_
#define Token_HPP_


#include <string>
#include <memory>


using std::string;
using std::shared_ptr;

class Callable;

typedef enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER, NIL,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  EOF_TOKEN
} TokenType;

class Object {
    public:
      typedef enum Object_type{
        Object_str,
        Object_num,
        Object_bool,
        Object_nil,
        Object_fun,
        //Object_instance,
        //Object_class,
    } Object_type;
    Object_type type;
    string obj_str;
    double obj_double;
    bool obj_bool;
    bool obj_null=false;
    bool callableObject = false;
    shared_ptr<Callable> function;
    static Object make_str_object(string obj_str);
    static Object make_double_object(double obj_double);
    static Object make_bool_object(bool obj_bool);
    static Object make_nil_object();
    static Object make_func_object(shared_ptr<Callable> func);
    string toString();
};

class Token {
    public:
        Token(TokenType type,string lexeme,Object literal,int line);
        string toString();
        TokenType type;
        string lexeme;
        Object literal;
        int line;

};

#endif