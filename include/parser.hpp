#pragma once
#include "ast.hpp"
#include "lexer.hpp"
#include <stdexcept>
#include <vector>

namespace expr {

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    NodePtr parse();

private:
    std::vector<Token> tokens_;
    std::size_t pos_ = 0;

    const Token& peek() const;
    const Token& advance();
    bool match(TokenType t);

    NodePtr parseExpression();  // +, -
    NodePtr parseTerm();        // *, /
    NodePtr parseUnary();       // унарный +/-
    NodePtr parsePrimary();     // число, идентификатор, (expr)

    [[noreturn]] void error(const std::string& msg) const;
};

} // namespace expr
