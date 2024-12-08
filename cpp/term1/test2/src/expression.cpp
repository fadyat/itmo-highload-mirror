#include "expression.hpp"

#include <optional>

int64_t Context::getVar(std::string const &name) const { return vars.at(name); }

void Context::setVar(std::string const &name, int64_t value) { vars[name] = value; }

bool Context::varIsSet(std::string const &name) const { return vars.find(name) != vars.end(); }

Val::Val(int64_t value) : value(value) {}

void Val::print(std::ostream &out) const { out << value; }

int64_t Val::eval(std::optional<Context> ctx) const { return value; }

Expression *Val::simplified(Context ctx) const { return new Val(value); }

Add::Add(Expression *lhs, Expression *rhs) : lhs(lhs), rhs(rhs) {}

Add::~Add() {
    delete lhs;
    delete rhs;
}

void Add::print(std::ostream &out) const {
    out << "(";
    lhs->print(out);
    out << " + ";
    rhs->print(out);
    out << ")";
}

int64_t Add::eval(std::optional<Context> ctx) const { return lhs->eval(ctx) + rhs->eval(ctx); }

Expression *Add::simplified(Context ctx) const {
    auto lhs_simplified = lhs->simplified(ctx);
    auto rhs_simplified = rhs->simplified(ctx);

    if (dynamic_cast<Val *>(lhs_simplified) && dynamic_cast<Val *>(rhs_simplified)) {
        auto result = lhs_simplified->eval() + rhs_simplified->eval();
        delete lhs_simplified;
        delete rhs_simplified;
        return new Val(result);
    }

    return new Add(lhs_simplified, rhs_simplified);
}

Mul::Mul(Expression *lhs, Expression *rhs) : lhs(lhs), rhs(rhs) {}

Mul::~Mul() {
    delete lhs;
    delete rhs;
}

void Mul::print(std::ostream &out) const {
    lhs->print(out);
    out << " * ";
    rhs->print(out);
}

int64_t Mul::eval(std::optional<Context> ctx) const { return lhs->eval(ctx) * rhs->eval(ctx); }

Expression *Mul::simplified(Context ctx) const {
    auto lhs_simplified = lhs->simplified(ctx);
    auto rhs_simplified = rhs->simplified(ctx);

    if (dynamic_cast<Val *>(lhs_simplified) && dynamic_cast<Val *>(rhs_simplified)) {
        auto result = lhs_simplified->eval() * rhs_simplified->eval();
        delete lhs_simplified;
        delete rhs_simplified;
        return new Val(result);
    }

    return new Mul(lhs_simplified, rhs_simplified);
}

Var::Var(std::string name) : name(std::move(name)) {}

void Var::print(std::ostream &out) const { out << name; }

int64_t Var::eval(std::optional<Context> ctx) const {
    if (ctx.has_value() && ctx->varIsSet(name)) {
        return ctx->getVar(name);
    }

    throw std::runtime_error("Variable " + name + " is not set");
}

Expression *Var::simplified(Context ctx) const {
    if (ctx.varIsSet(name)) {
        return new Val(ctx.getVar(name));
    }

    return new Var(name);
}
