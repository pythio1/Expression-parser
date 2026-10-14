#pragma once
#include <memory>
#include <string>
#include <utility>

namespace expr {

struct Node {
    virtual ~Node() = default;
};

using NodePtr = std::unique_ptr<Node>;

struct NumberNode : Node {
    double value;
    explicit NumberNode(double v) : value(v) {}
};

struct VariableNode : Node {
    std::string name;
    explicit VariableNode(std::string n) : name(std::move(n)) {}
};

struct BinaryNode : Node {
    char op;
    NodePtr lhs;
    NodePtr rhs;
    BinaryNode(char o, NodePtr l, NodePtr r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct UnaryNode : Node {
    char op;
    NodePtr operand;
    UnaryNode(char o, NodePtr n) : op(o), operand(std::move(n)) {}
};

} // namespace expr
