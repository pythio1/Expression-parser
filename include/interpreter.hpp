#pragma once
#include "ast.hpp"
#include <functional>
#include <stdexcept>
#include <string>

namespace expr {

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg) : std::runtime_error(msg) {}
};

using VariableResolver = std::function<double(const std::string&)>;

double evaluate(const Node& node, const VariableResolver& resolver);

// Красивая печать AST в виде S-выражения — удобно для тестов
std::string toSExpr(const Node& node);

// Печать в RPN (постфиксная нотация)
std::string toRPN(const Node& node);

} // namespace expr
