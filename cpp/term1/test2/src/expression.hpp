#pragma once

#include <cstddef>
#include <iostream>
#include <map>
#include <optional>

class Context {
  private:
    std::map<std::string, int64_t> vars;

  public:
    Context() = default;

    int64_t getVar(std::string const &name) const;
    void setVar(std::string const &name, int64_t value);
    bool varIsSet(std::string const &name) const;
};

class Expression {
  public:
    virtual ~Expression() = default;

    virtual void print(std::ostream &out) const = 0;
    virtual int64_t eval(std::optional<Context> ctx = std::nullopt) const = 0;
    virtual Expression *simplified(Context ctx) const = 0;

    void *operator new(size_t size);
    void operator delete(void *ptr) noexcept;
};

class Val : public Expression {
  private:
    int64_t value;

  public:
    Val(int64_t value);

    void print(std::ostream &out) const override;
    int64_t eval(std::optional<Context> ctx = std::nullopt) const override;
    Expression *simplified(Context ctx) const override;

    void *operator new(size_t size);
    void operator delete(void *ptr) noexcept;
};

class Add : public Expression {
  private:
    Expression *lhs;
    Expression *rhs;

  public:
    Add(Expression *lhs, Expression *rhs);
    ~Add();

    Add(const Add &) = delete;
    Add &operator=(const Add &) = delete;

    void print(std::ostream &out) const override;
    int64_t eval(std::optional<Context> ctx = std::nullopt) const override;
    Expression *simplified(Context ctx) const override;

    void *operator new(size_t size);
    void operator delete(void *ptr) noexcept;
};

class Mul : public Expression {
  private:
    Expression *lhs;
    Expression *rhs;

  public:
    Mul(Expression *lhs, Expression *rhs);
    ~Mul();

    Mul(const Mul &) = delete;
    Mul &operator=(const Mul &) = delete;

    void print(std::ostream &out) const override;
    int64_t eval(std::optional<Context> ctx = std::nullopt) const override;
    Expression *simplified(Context ctx) const override;

    void *operator new(size_t size);
    void operator delete(void *ptr) noexcept;
};

class Var : public Expression {
  private:
    std::string name;

  public:
    Var(std::string name);

    void print(std::ostream &out) const override;
    int64_t eval(std::optional<Context> ctx = std::nullopt) const override;
    Expression *simplified(Context ctx) const override;

    void *operator new(size_t size);
    void operator delete(void *ptr) noexcept;
};
