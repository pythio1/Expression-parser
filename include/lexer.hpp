#pragma once
#include <stdexcept>
#include <string>
#include <vector>

namespace expr {

enum class TokenType {
    Number,
    Identifier,
    Plus,
    Minus,
    Star,
    Slash,
    LParen,
    RParen,
    End
};

struct Token {
    TokenType type;
    std::string text;
    double value = 0.0;
    std::size_t pos = 0;
};

class LexerError : public std::runtime_error {
public:
    explicit LexerError(const std::string& msg) : std::runtime_error(msg) {}
};

class Lexer {
public:
    explicit Lexer(std::string input);
    std::vector<Token> tokenize() const;

private:
    std::string input_;
};

} // namespace expr
