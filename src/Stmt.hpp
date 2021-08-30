#ifndef Stmt_HPP_
#define Stmt_HPP_


#include "Expr.hpp"

#include <memory>
#include <vector>

class Block;
class Expression;
class Function;
class If;
class Print;
class Return;
class Var;
class While;

using std::vector;
using std::shared_ptr;


class VisitorStmt {
    public:
        virtual void visitBlock(shared_ptr<Block> stmt)=0;
        virtual void visitExpression(shared_ptr<Expression> stmt)=0;
        virtual void visitIf(shared_ptr<If> stmt) =0;
        virtual void visitPrint(shared_ptr<Print> stmt)=0;
        virtual void visitFunction(shared_ptr<Function> stmt) = 0;
        virtual void visitReturn(shared_ptr<Return> stmt) = 0;
        virtual void visitVar(shared_ptr<Var> stmt)= 0;
        virtual void visitWhile(shared_ptr<While> stmt)= 0;

        virtual ~VisitorStmt() = default;
};

    class Stmt {
        public:
            virtual void accept(VisitorStmt& visitor)=0;
    };

    class Block: public Stmt, public  std::enable_shared_from_this<Block>{
        public:
        Block(vector<shared_ptr<Stmt>> statements) : statements{(statements)}
  {}

        void accept(VisitorStmt& visitor) override{
            return visitor.visitBlock(shared_from_this());
        }

        vector<shared_ptr<Stmt>> statements;
};


    class Expression: public Stmt, public  std::enable_shared_from_this<Expression>{
        public:
        Expression(shared_ptr<Expr> expression) : expression{(expression)}
  {}

        void accept(VisitorStmt& visitor) override{
            return visitor.visitExpression(shared_from_this());
        }

        shared_ptr<Expr> expression;

    };

   class Function: public Stmt, public  std::enable_shared_from_this<Function>{
        public:
        Function(Token name, vector<Token> params, vector<shared_ptr<Stmt>> body)
         : name{(name)}, params{params}, body{body} 
         {}
        void accept(VisitorStmt& visitor) override{
            return visitor.visitFunction(shared_from_this());
        }

        Token name;
        vector<Token> params;
        vector<shared_ptr<Stmt>> body;
};




   class If: public Stmt, public  std::enable_shared_from_this<If>{
        public:
        If(shared_ptr<Expr> condition, shared_ptr<Stmt> thenBranch, shared_ptr<Stmt> elseBranch)
         : condition{(condition)}, thenBranch{thenBranch}, elseBranch{elseBranch} 
         {}
        void accept(VisitorStmt& visitor) override{
            return visitor.visitIf(shared_from_this());
        }

        shared_ptr<Expr> condition;
        shared_ptr<Stmt> thenBranch;
        shared_ptr<Stmt> elseBranch;
};


    class Print: public Stmt, public  std::enable_shared_from_this<Print>{
        public:
        Print(shared_ptr<Expr> expression) : expression{(expression)}
  {}

        void accept(VisitorStmt& visitor) override{
            return visitor.visitPrint(shared_from_this());
        }

        shared_ptr<Expr> expression;
};


    class Return: public Stmt, public std::enable_shared_from_this<Return> {
        public:
            Return(Token keyword, shared_ptr<Expr> value) : keyword(keyword), value(value) 
            {}

            void accept(VisitorStmt& visitor) override{
            return visitor.visitReturn(shared_from_this());
        } 

        Token keyword;
        shared_ptr<Expr> value;   
        
    };

    class Var: public Stmt, public  std::enable_shared_from_this<Var>{
        public:
        Var(Token name, shared_ptr<Expr> initializer) : name{(name)}, initializer{(initializer)}
  {}

        void accept(VisitorStmt& visitor) override{
            return visitor.visitVar(shared_from_this());
        }

        Token name;
        shared_ptr<Expr> initializer;
};

class While: public Stmt, public std::enable_shared_from_this<While> {
    public:
        While(shared_ptr<Expr> condition,shared_ptr<Stmt> body) : condition(condition), body(body) 
        {}

        void accept(VisitorStmt& visitor) override {
            return visitor.visitWhile(shared_from_this());
        }

    shared_ptr<Expr> condition;
    shared_ptr<Stmt> body;
};

#endif