

#include "Token.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Environment.hpp"
#include "LoxFunction.hpp"
#include "ReturnCall.hpp"
#include "Callable.hpp"

#include <memory>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

using std::shared_ptr;
using std::vector;

Interpreter::Interpreter()
{
    shared_ptr<Callable> clockfun{new ClockFunction};
    Object obj_fun = Object::make_func_object(clockfun);
    //globals->define("clock",obj_fun);
}

void Interpreter::interpret(vector<shared_ptr<Stmt>> statementes)
{
    try
    {

        for (auto statement : statementes)
        {
            execute(statement);
        }
    }
    catch (RuntimeError error)
    {
        std::cerr << error.message << "\n";
    }
}

Object Interpreter::evaluate(shared_ptr<Expr> expr)
{
    return expr->accept(*this);
}

void Interpreter::execute(shared_ptr<Stmt> statement)
{
    statement->accept(*this);
}

void Interpreter::executeBlock(vector<shared_ptr<Stmt>> statements, shared_ptr<Environment> environment)
{
    shared_ptr<Environment> previous = this->environment;
    try
    {

        this->environment = environment;
        for (auto statement : statements)
        {
            execute(statement);
        }
        this->environment = previous;
    }
    catch (...)
    {
        this->environment = previous;

        throw;
    }
}

Object Interpreter::visitLiteral(shared_ptr<Literal> expr)
{
    return expr->value;
}

Object Interpreter::visitLogical(shared_ptr<Logical> expr)
{
    Object left = evaluate(expr->left);
    if (expr->operate.type == OR)
    {
        if (isTruthy(left))
        {
            return left;
        }
    }
    else
    {
        if (!isTruthy(left))
        {
            return left;
        }
    }

    return evaluate(expr->right);
}

Object Interpreter::visitGroup(shared_ptr<Group> expr)
{
    return evaluate(expr->expression);
}

Object Interpreter::visitUnary(shared_ptr<Unary> expr)
{
    Object right = evaluate(expr->right);

    switch (expr->operate.type)
    {
    case BANG:
        return Object::make_bool_object(!isTruthy(right));

    case MINUS:
        checkNumberOperand(expr->operate, right);
        return Object::make_double_object(-(right.obj_double));
    }

    return Object::make_nil_object();
}

Object Interpreter::visitVariable(shared_ptr<Variable> expr)
{
    return environment->get(expr->name);
}

void Interpreter::visitExpression(shared_ptr<Expression> stmt)
{
    evaluate(stmt->expression);
    return;
}

void Interpreter::visitFunction(shared_ptr<Function> stmt)
{
    shared_ptr<LoxFunction> function{new LoxFunction(stmt, environment)};
    Object func = Object::make_func_object(function);

    environment->define(stmt->name.lexeme, func);
}

void Interpreter::visitIf(shared_ptr<If> stmt)
{
    if (isTruthy(evaluate(stmt->condition)))
    {
        execute(stmt->thenBranch);
    }
    else if (stmt->elseBranch != nullptr)
    {
        execute(stmt->elseBranch);
    }
    return;
}

void Interpreter::visitPrint(shared_ptr<Print> stmt)
{
    Object value = evaluate(stmt->expression);
    std::cout << stringify(value) << "\n";
    return;
}
    
void Interpreter::visitReturn(shared_ptr<Return> stmt)
{   
    Object value = Object::make_nil_object();

    if (stmt->value != nullptr)
    {
        value = evaluate(stmt->value);
    }
    throw ReturnCall(value);
}

void Interpreter::visitVar(shared_ptr<Var> stmt)
{

    Object value;
    if (stmt->initializer != nullptr)
    {
        value = evaluate(stmt->initializer);
    }
    else
    {
        value = Object::make_nil_object();
    }

    environment->define(stmt->name.lexeme, value);
    return;
}

void Interpreter::visitWhile(shared_ptr<While> stmt)
{
    while (isTruthy(evaluate(stmt->condition)))
    {
        execute(stmt->body);
    }
    return;
}

Object Interpreter::visitAssign(shared_ptr<Assign> expr)
{
    Object value = evaluate(expr->value);
    environment->assign(expr->name, value);

    return value;
}

Object Interpreter::visitCall(shared_ptr<Call> expr)
{
    Object callee = evaluate(expr->callee);

    vector<Object> arguments;
    for (auto args : expr->arguments)
    {
        arguments.push_back(evaluate(args));
    }
    bool v = callee.callableObject;
    if (!(v))
    {
        throw new RuntimeError(expr->paren, "Can only call functions and classes");
    }

    shared_ptr<Callable> function;

    if (callee.type == Object::Object_fun)
    {
        function = callee.function;
    }

    if (arguments.size() != function->arity())
    {
        throw new RuntimeError(expr->paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, arguments);
}

void Interpreter::visitBlock(shared_ptr<Block> stmt)
{
    executeBlock(stmt->statements, shared_ptr<Environment>(new Environment(environment)));
    return;
}

void Interpreter::checkNumberOperand(Token operate, Object operand)
{
    if (operand.type == Object::Object_num)
    {
        return;
    }
    throw RuntimeError(operate, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token operate, Object left, Object right)
{
    if (left.type == Object::Object_num && right.type == Object::Object_num)
    {
        return;
    }
    throw RuntimeError(operate, "Operands must be a number.");
}

Object Interpreter::visitBinary(shared_ptr<Binary> expr)
{
    Object left = evaluate(expr->left);
    Object right = evaluate(expr->right);
    double result_num = 0;
    switch (expr->operate.type)
    {
    case GREATER:
        checkNumberOperands(expr->operate, left, right);

        return Object::make_bool_object(left.obj_double > right.obj_double);

    case GREATER_EQUAL:
        checkNumberOperands(expr->operate, left, right);

        return Object::make_bool_object(left.obj_double >= right.obj_double);

    case LESS:
        checkNumberOperands(expr->operate, left, right);

        return Object::make_bool_object(left.obj_double < right.obj_double);

    case LESS_EQUAL:
        checkNumberOperands(expr->operate, left, right);

        return Object::make_bool_object(left.obj_double <= right.obj_double);

    case BANG_EQUAL:

        return Object::make_bool_object(!isEqual(left, right));

    case EQUAL_EQUAL:
        return Object::make_bool_object(isEqual(left, right));

    case MINUS:
        checkNumberOperands(expr->operate, left, right);

        return Object::make_double_object(left.obj_double - right.obj_double);

    case SLASH:
        checkNumberOperands(expr->operate, left, right);
        return Object::make_double_object(left.obj_double / right.obj_double);

    case STAR:
        checkNumberOperands(expr->operate, left, right);
        return Object::make_double_object(left.obj_double * right.obj_double);

    case PLUS:
        if (left.type == Object::Object_num && right.type == Object::Object_num)
        {
            return Object::make_double_object(left.obj_double + right.obj_double);
        }

        if (left.type == Object::Object_str && right.type == Object::Object_str)
        {
            return Object::make_str_object(left.obj_str + right.obj_str);
        }

        throw RuntimeError(expr->operate, "Operands must be either two strings or two numbers.");
    }

    return Object::make_nil_object();
}

bool Interpreter::isTruthy(Object object)
{
    if (object.type == Object::Object_nil)
    {
        return false;
    }

    if (object.type == Object::Object_bool)
    {
        return object.obj_bool;
    }
    return true;
}

bool Interpreter::isEqual(Object a, Object b)
{
    if (a.type == Object::Object_nil && b.type == Object::Object_nil)
    {
        return true;
    }
    if (a.type == Object::Object_nil)
    {
        return false;
    }

    if (a.type == b.type)
    {
        switch (a.type)
        {
        case Object::Object_bool:
            return a.obj_bool == b.obj_bool;
        case Object::Object_num:
            return a.obj_double == b.obj_double;
        case Object::Object_str:
            return a.obj_str == b.obj_str;
        }
    }
    return false;
}

string Interpreter::stringify(Object object)
{
    string text = object.toString();
    if (text[text.length() - 2] == '.' &&
        text[text.length() - 1] == '0')
    {
        text = text.substr(0, text.length() - 2);
    }

    return text;
}
