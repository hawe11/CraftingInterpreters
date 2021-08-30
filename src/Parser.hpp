#ifndef Parser_HPP_
#define Parser_HPP_

#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

#include <list>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

using std::shared_ptr;
using std::string;
using std::vector;

class Parser {
    class ParseError: public std::exception {
        
        };
    const vector <Token> &tokens;
    int current =0;

    public:
        Parser(const vector <Token> &tokens_) : tokens(tokens_) {};
        vector<shared_ptr<Stmt>> parse();
    private:
  
        shared_ptr<Expr> expression();
        shared_ptr<Expr> equality();
        shared_ptr<Expr> comparison();
        shared_ptr<Expr> call();
        shared_ptr<Expr> term(); 
        shared_ptr<Expr> factor();
        shared_ptr<Expr> finishCall(shared_ptr<Expr> callee);
        shared_ptr<Expr> unary();
        shared_ptr<Expr> primary();
        shared_ptr<Expr> assignment();
        shared_ptr<Expr> orExpr();
        shared_ptr<Expr> andExpr();
    

        bool match(vector<TokenType> type);
        bool check(TokenType type);
        Token advance();
        bool isAtEnd();
        Token peek();
        Token previous();
        Token consume(TokenType type,string msg); 
        ParseError error(Token token, string msg);
        void synchronize();

        //Functions regarding statements
        shared_ptr<Stmt> statement();
        shared_ptr<Stmt> printStatement();
        shared_ptr<Stmt> expressionStatement();
        shared_ptr<Stmt> declaration();
        shared_ptr<Stmt> varDeclaration();
        shared_ptr<Stmt> ifStatement();
        shared_ptr<Stmt> function(string kind);
        shared_ptr<Stmt> whileStatement();
        shared_ptr<Stmt> forStatement();
        shared_ptr<Stmt> returnStatement();

        vector<shared_ptr<Stmt>> block();

};



#endif