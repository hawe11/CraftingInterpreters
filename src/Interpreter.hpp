#ifndef Interpreter_HPP_
#define Interpreter_HPP_

#include "Expr.hpp"
#include "Token.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"
#include "Callable.hpp"
#include "Interpreter.hpp"

#include<memory>
#include <string>
#include <vector>
#include <chrono>

using std::shared_ptr;
using std::string;
using std::vector;



class Interpreter: public Visitor, public VisitorStmt{


    private:

        Object evaluate(shared_ptr<Expr> expr);
        bool isTruthy(Object object);
        bool isEqual(Object a, Object b);
        void checkNumberOperand(Token operate, Object operand);
        void checkNumberOperands(Token operate, Object left,Object right);
        string stringify(Object object);
        void execute(shared_ptr<Stmt> statement);

        shared_ptr<Environment> globals {new Environment};
        shared_ptr<Environment> environment = globals;

        

    public:
        Interpreter();

        Object visitLiteral(shared_ptr<Literal> expr) override;
        Object visitLogical(shared_ptr<Logical> expr) override;
        Object visitGroup(shared_ptr<Group> expr) override;
        Object visitUnary(shared_ptr<Unary> expr) override;
        Object visitBinary(shared_ptr<Binary> expr) override;
        Object visitVariable(shared_ptr<Variable> expr) override;
        Object visitAssign(shared_ptr<Assign> expr) override;
        Object visitCall(shared_ptr<Call> expr) override;

        void visitExpression(shared_ptr<Expression> stmt) override;
        void visitFunction(shared_ptr<Function>) override;

        void visitPrint(shared_ptr<Print> stmt) override;
        void visitBlock(shared_ptr<Block> stmt) override;
        void visitVar(shared_ptr<Var> stmt) override;
        void visitReturn(shared_ptr<Return> stmt) override;
        void visitWhile(shared_ptr<While> stmt) override;
        void visitIf(shared_ptr<If> stmt) override;

        void executeBlock(std::vector<shared_ptr<Stmt>> statements, shared_ptr<Environment> enviroment);
        void interpret(std::vector<shared_ptr<Stmt>> statements);
        
};


class ClockFunction: public Callable {

    public:
        int arity() override {
            return 0;
        }

        Object call(Interpreter& interpreter,vector<Object> arguments) override {
            auto ticks = std::chrono::system_clock::now().time_since_epoch();
            return Object::make_double_object( std::chrono::duration<double>{ticks}.count() / 1000.0);  
        }

        string toString() override {
        return "native function";
        }

    private:
        
};



#endif