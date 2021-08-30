#ifndef Expr_HPP_
#define Expr_HPP_

#include "Token.hpp"

#include <memory>
#include <vector>
//#include <any>

using std::shared_ptr;
//using std::any;

class Binary;
class Call;
class Group;
class Literal;
class Logical;
class Unary;
class Variable;
class Assign;

class Visitor {
    public:
        virtual Object visitBinary(shared_ptr<Binary> expr)=0;
        virtual Object visitCall(shared_ptr<Call> epxr) =0;
        virtual Object visitGroup(shared_ptr<Group> expr)=0;
        virtual Object visitLiteral(shared_ptr<Literal> expr)=0;
        virtual Object visitLogical(shared_ptr<Logical> expr)=0;
        virtual Object visitUnary(shared_ptr<Unary> expr)=0;
        virtual Object visitVariable(shared_ptr<Variable> expr)=0;
        virtual Object visitAssign(shared_ptr<Assign> expr)=0;


        virtual ~Visitor() = default;
};


class Expr {
    public:
        virtual Object accept(Visitor& visitor)=0;


};

class Assign : public Expr, public std::enable_shared_from_this<Assign> {
    public:
        Assign(Token name, shared_ptr<Expr> value) : name(name), value(value) 
        {}
        Object accept(Visitor& visitor) override{
            return visitor.visitAssign(shared_from_this());
        }

        Token name;
        shared_ptr<Expr> value;
};

class Binary : public Expr,  public std::enable_shared_from_this<Binary>
 {
    public:
        Binary(shared_ptr<Expr> left, Token operate, shared_ptr<Expr> right) : left{(left)}, operate{(operate)}, right{(right)}
  {}

        Object accept(Visitor& visitor) override{
            return visitor.visitBinary(shared_from_this());
        }

       const Token operate;
       const shared_ptr<Expr> left;
       const shared_ptr<Expr> right;

};

class Call : public Expr, public std::enable_shared_from_this<Call> {
    public:
        Call(shared_ptr<Expr> callee, Token paren, std::vector<shared_ptr<Expr>> arguments) : callee(callee), paren(paren), arguments(arguments) 
        {}

        Object accept(Visitor& visitor) override{
            return visitor.visitCall(shared_from_this());
        }

    shared_ptr<Expr> callee;
    Token paren;
    std::vector<shared_ptr<Expr>> arguments;
};

class Group : public Expr, public std::enable_shared_from_this<Group>
{
    public:
        Group(shared_ptr<Expr> expression_) : expression(expression_) {}
        Object accept(Visitor& visitor) override {
            return visitor.visitGroup(shared_from_this());
        }
    shared_ptr<Expr> expression;
};

class Literal : public Expr, public std::enable_shared_from_this<Literal>
{
    public:
        Literal(Object value_) : value(value_) {}
        Object accept(Visitor& visitor) override {
            return visitor.visitLiteral(shared_from_this());
        }
    const Object value;
};


class Logical : public Expr, public std::enable_shared_from_this<Logical>
{
    public:
        Logical(shared_ptr<Expr> left, Token operate, shared_ptr<Expr> right)
        : left{(left)}, operate{(operate)}, right{(right)}
        {}
        Object accept(Visitor& visitor) override {
            return visitor.visitLogical(shared_from_this());
        }
    shared_ptr<Expr> left;
    Token operate;
    shared_ptr<Expr> right;

};


class Unary : public Expr, public std::enable_shared_from_this<Unary>
{
    public:
        Unary(Token operate,shared_ptr<Expr> right_) : operate(operate), right(right_)
        {}
        Object accept(Visitor& visitor) override {
            return visitor.visitUnary(shared_from_this());
        }
    const Token operate;
    const shared_ptr<Expr> right;
};


class Variable : public Expr, public std::enable_shared_from_this<Variable>
{
    public:
        Variable(Token name) : name(name) {}
        Object accept(Visitor& visitor) override {
            return visitor.visitVariable(shared_from_this());
        }
    Token name;
};

#endif