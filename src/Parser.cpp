#include "Parser.hpp"
#include "Token.hpp"
#include "ReadFile.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Scanner.hpp"


#include <list>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>


using std::list;
using std::shared_ptr;
using std::string;
using std::runtime_error;
using std::vector;


vector<shared_ptr<Stmt>> Parser::parse() {
        vector<shared_ptr<Stmt>> statements;
        while(!isAtEnd()) {
            statements.push_back(declaration());

        }

        return statements; 
}

shared_ptr<Expr> Parser::expression() {
    return assignment();
}

shared_ptr<Stmt> Parser::declaration() {
    try
    {
        if(match({FUN})) {
            return function("function");
        }
        if(match({VAR})) {
            return varDeclaration();
        }
        return statement();
    }
    catch(ParseError error)
    {
        synchronize();
        return nullptr;
    }
    
}

shared_ptr<Stmt> Parser::statement() {

    if(match({PRINT})) {
        return printStatement();
    }

    if(match({RETURN})) {
        return returnStatement();
    }

    if(match({WHILE})) {
        return whileStatement();
    }

    if(match({LEFT_BRACE})) {
        return shared_ptr<Stmt>(new Block(block()));
    }

    if(match({FOR})) {
        return forStatement();
    }

    if(match({IF})) {
        return ifStatement();
    }
    return expressionStatement();
}

shared_ptr<Stmt> Parser::forStatement() {
    consume(LEFT_PAREN, "Expected '(' after for statement.");
    shared_ptr<Stmt> initilizer;
    if(match({SEMICOLON})) {
        initilizer = nullptr;
    } else if (match({VAR})){
        initilizer = varDeclaration();
    } else {
        initilizer = expressionStatement();
    }

    shared_ptr<Expr> condition = nullptr;

    if(!check(SEMICOLON)) {
        condition = expression();
    }

    consume(SEMICOLON, "Expect ';' after loop condition");

    shared_ptr<Expr> increment = nullptr;
    if(!check(RIGHT_PAREN)) {
        increment = expression();
    }

    consume(RIGHT_PAREN,"Expect ')' after for clauses.");

    shared_ptr<Stmt> body = statement();

    if(increment != nullptr) {
        std::vector<shared_ptr<Stmt>> vec;
        vec.push_back(body);

        vec.push_back(shared_ptr<Stmt>(new Expression(increment)));
        body = shared_ptr<Stmt>(new Block(vec));
    }

    if(condition == nullptr) {
        condition = shared_ptr<Expr>(new Literal(Object::make_bool_object(true)));
    }

    body = shared_ptr<Stmt>(new While(condition,body));

    if(initilizer != nullptr) {
        body = shared_ptr<Stmt>(new Block({initilizer,body}));
    }

    return body;

}

shared_ptr<Stmt> Parser::ifStatement() {
    consume(LEFT_PAREN,"Expect ')' after if.");
    shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expected ')' after if conditions");

    shared_ptr<Stmt> thenBranch = statement();
    shared_ptr<Stmt> elseBranch = nullptr;

    if(match({ELSE})) {
        elseBranch = statement();
    }

    return shared_ptr<Stmt>(new If(condition,thenBranch,elseBranch));


}


shared_ptr<Stmt> Parser::printStatement() {
    shared_ptr<Expr> value = expression();
    consume(SEMICOLON,"Expect ';' after value.");
    return shared_ptr<Stmt>(new Print(value));
}

shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    shared_ptr<Expr> value = nullptr;

    if(!check(SEMICOLON)) {
        value = expression();
    }
    consume(SEMICOLON,"Expected ';' after return value");

    return shared_ptr<Stmt>(new Return(keyword,value));
}

shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(IDENTIFIER, "Expected variable name.");
    shared_ptr<Expr> intilizer= nullptr;
    if(match({EQUAL})) {
        intilizer=expression();
    }
    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return shared_ptr<Stmt> (new Var(name,intilizer));
}

shared_ptr<Stmt> Parser::whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'. ");

    shared_ptr<Expr> condition = expression();

    consume(RIGHT_PAREN, "Expect ')' after '('.");

    shared_ptr<Stmt> body = statement();

    return shared_ptr<Stmt>(new While(condition,body));
}

shared_ptr<Stmt> Parser::expressionStatement() {
    shared_ptr<Expr> expr = expression();
    consume(SEMICOLON,"Expect ';' after expression.");
    return shared_ptr<Stmt>(new Expression(expr));
}

shared_ptr<Stmt> Parser::function(string kind) {
    Token name = consume(IDENTIFIER, "Expected "+ kind + "name");
    consume(LEFT_PAREN, "Expected '(' after "+ kind+".");
    vector<Token> parameters;

    if(!check(RIGHT_PAREN)) {
        do{
            if(parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }

            parameters.push_back(consume(IDENTIFIER,"Expected paramter name."));
        } while(match({COMMA}));
    }
    consume(RIGHT_PAREN,"Expected ')' after parameters");

    consume(LEFT_BRACE, "Expected '{' before "+ kind+" body.");
    vector<shared_ptr<Stmt>> body=block();
    return shared_ptr<Stmt>(new Function(name,parameters,body));
}

vector<shared_ptr<Stmt>> Parser::block() {
    vector<shared_ptr<Stmt>> statements;

    while(!check({RIGHT_BRACE}) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(RIGHT_BRACE,"Expected '}' after block");
    return statements;
}

shared_ptr<Expr> Parser::assignment() {
    shared_ptr<Expr> expr=orExpr();

    if(match({EQUAL})) {
        Token equals = previous();
        shared_ptr<Expr> value = assignment();

        if(Variable* v=dynamic_cast<Variable*>(expr.get())) {
            Token name = v->name;
            return shared_ptr<Expr>(new Assign(name,value));
        }
        error(equals, "Invalid assignment target.");
    }

    return expr;
}

shared_ptr<Expr> Parser::orExpr(){
    shared_ptr<Expr> expr = andExpr();

    while(match({OR})){
        Token operate = previous();
        shared_ptr<Expr> right=andExpr();
        expr = shared_ptr<Logical>(new Logical(expr, operate,right));
    }
    return expr;
}

shared_ptr<Expr> Parser::andExpr() {
    shared_ptr<Expr> expr = equality();

    while(match({AND})) {
        Token operate = previous();
        shared_ptr<Expr> right = equality();
        expr = shared_ptr<Logical>(new Logical(expr, operate, right));
    }
    return expr;
}

shared_ptr<Expr> Parser::equality() {
    shared_ptr<Expr> expr = comparison();
    vector<TokenType> type ={BANG_EQUAL,EQUAL_EQUAL};
    while(match(type)) {
        Token operate=previous();
        shared_ptr<Expr> right = comparison();
        expr =  shared_ptr<Binary>(new Binary(expr,operate,right)); 
    }
    return expr;
}

bool Parser::match(vector<TokenType> types ) {
    for (TokenType type : types) {
        if(check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if(isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

Token Parser::advance() {
    if(!isAtEnd()) {
        ++current;
    }
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type==EOF_TOKEN;
}

Token Parser::peek() {
    return tokens.at(current);
}

Token Parser::previous() {
    return tokens.at(current - 1);
}

shared_ptr<Expr> Parser::comparison(){
    shared_ptr<Expr> expr = term();
    vector<TokenType> type ={GREATER, GREATER_EQUAL, LESS, LESS_EQUAL};
    while(match(type)) {
        Token operate = previous();
        shared_ptr<Expr> right= term();
        expr =  shared_ptr<Binary>(new Binary(expr,operate,right)); 
    }
    return expr;
}

shared_ptr<Expr> Parser::term() {
    shared_ptr<Expr> expr = factor();
    vector<TokenType> type={MINUS,PLUS};
    while(match(type)) {
        Token operate = previous();
        shared_ptr<Expr> right= factor();
        expr =  shared_ptr<Binary>(new Binary(expr,operate,right)); 
    }
    return expr;
}

shared_ptr<Expr> Parser::factor() { 
    shared_ptr<Expr> expr = unary();
    vector<TokenType> type ={SLASH,STAR};
    while(match(type)) {
        Token operate = previous();
        shared_ptr<Expr> right= unary();
        expr =  shared_ptr<Binary>(new Binary(expr,operate,right)); 
    }
    return expr;
}


shared_ptr<Expr> Parser::unary() {
    vector<TokenType> type = {BANG,MINUS}; 
    if(match(type)) {
        Token operate = previous();
        shared_ptr<Expr> right = unary();
        shared_ptr<Expr> expr = shared_ptr<Unary>(new Unary(operate,right));
        return expr;
    }
    return call();
}

shared_ptr<Expr> Parser::finishCall(shared_ptr<Expr> callee) {
    vector<shared_ptr<Expr>> arguments;

    if(!check(RIGHT_PAREN)) {
        do {
            if(arguments.size() >= 255) {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());

        } while (match({COMMA}));
    }
    Token paren = consume(RIGHT_PAREN,"Expect ')' after argument.");

    return shared_ptr<Expr>(new Call(callee,paren,arguments));
}

shared_ptr<Expr> Parser::call() {
    shared_ptr<Expr> expr = primary();

    while(true) {
        if(match({LEFT_PAREN})) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }

    return expr;
}

shared_ptr<Expr> Parser::primary() {
    if (match({ FALSE })) {
        return shared_ptr<Expr>(
          new Literal(Object::make_bool_object(false)));
    }
    if (match({ TRUE })) {
        return shared_ptr<Expr>(
          new Literal(Object::make_bool_object(true)));
    }
    if (match({ NIL })) {
        return shared_ptr<Expr>(
          new Literal(Object::make_nil_object()));
    }

    if (match({ NUMBER })) {
      return shared_ptr<Expr>(
        new Literal(Object::make_double_object(previous().literal.obj_double)));
    }
    if (match({ STRING })) {

      return shared_ptr<Expr>(
        new Literal(Object::make_str_object(previous().literal.obj_str)));
    }

    if (match({ LEFT_PAREN })) {
      shared_ptr<Expr> expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return shared_ptr<Expr>(new Group(expr));
    }

    if(match({IDENTIFIER})) {
        return shared_ptr<Expr>(new Variable(previous()));
    }

    throw error(peek(),"Expected expression.");
}

Token Parser::consume(TokenType type, string msg) {
    if(check(type)) {
        return advance();
    }
    throw error(peek(),msg);
}

Parser::ParseError Parser::error(Token token, string msg) {
    ReadFile::error(token,msg);
    return ParseError();
}

  void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
      if (previous().type == SEMICOLON) {
            return;
      }
      switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
      }

      advance();
    }
  }