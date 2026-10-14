#include "interpreter.hpp"
#include <sstream>

namespace expr {

double evaluate(const Node& node, const VariableResolver& resolver) {
    if (auto n = dynamic_cast<const NumberNode*>(&node)) {
        return n->value;
    }
    if (auto v = dynamic_cast<const VariableNode*>(&node)) {
        if (!resolver) {
            throw EvalError("no variable resolver provided for '" + v->name + "'");
        }
        return resolver(v->name);
    }
    if (auto b = dynamic_cast<const BinaryNode*>(&node)) {
        double lhs = evaluate(*b->lhs, resolver);
        double rhs = evaluate(*b->rhs, resolver);
        switch (b->op) {
            case '+': return lhs + rhs;
            case '-': return lhs - rhs;
            case '*': return lhs * rhs;
            case '/':
                if (rhs == 0.0) throw EvalError("division by zero");
                return lhs / rhs;
        }
        throw EvalError(std::string("unknown binary operator '") + b->op + "'");
    }
    if (auto u = dynamic_cast<const UnaryNode*>(&node)) {
        double v = evaluate(*u->operand, resolver);
        switch (u->op) {
            case '+': return v;
            case '-': return -v;
        }
        throw EvalError(std::string("unknown unary operator '") + u->op + "'");
    }
    throw EvalError("unknown AST node");
}

std::string toSExpr(const Node& node) {
    if (auto n = dynamic_cast<const NumberNode*>(&node)) {
        std::ostringstream oss;
        oss << n->value;
        return oss.str();
    }
    if (auto v = dynamic_cast<const VariableNode*>(&node)) {
        return v->name;
    }
    if (auto b = dynamic_cast<const BinaryNode*>(&node)) {
        return std::string("(") + b->op + " " + toSExpr(*b->lhs) + " " + toSExpr(*b->rhs) + ")";
    }
    if (auto u = dynamic_cast<const UnaryNode*>(&node)) {
        return std::string("(") + u->op + "u " + toSExpr(*u->operand) + ")";
    }
    return "?";
}

std::string toRPN(const Node& node) {
    if (auto n = dynamic_cast<const NumberNode*>(&node)) {
        std::ostringstream oss;
        oss << n->value;
        return oss.str();
    }
    if (auto v = dynamic_cast<const VariableNode*>(&node)) {
        return v->name;
    }
    if (auto b = dynamic_cast<const BinaryNode*>(&node)) {
        return toRPN(*b->lhs) + " " + toRPN(*b->rhs) + " " + b->op;
    }
    if (auto u = dynamic_cast<const UnaryNode*>(&node)) {
        return toRPN(*u->operand) + " " + u->op + "u";
    }
    return "?";
}

} // namespace expr
