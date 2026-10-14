#include "lexer.hpp"
#include <cctype>
#include <sstream>

namespace expr {

Lexer::Lexer(std::string input) : input_(std::move(input)) {}

std::vector<Token> Lexer::tokenize() const {
    std::vector<Token> tokens;
    std::size_t i = 0;
    const std::size_t n = input_.size();

    auto makeError = [&](std::size_t pos, const std::string& what) {
        std::ostringstream oss;
        oss << "Lex error at position " << pos << ": " << what;
        return LexerError(oss.str());
    };

    while (i < n) {
        unsigned char c = static_cast<unsigned char>(input_[i]);

        if (std::isspace(c)) {
            ++i;
            continue;
        }

        // Число: [0-9]+ ('.' [0-9]+)?
        if (std::isdigit(c)) {
            std::size_t start = i;
            while (i < n && std::isdigit(static_cast<unsigned char>(input_[i]))) ++i;
            if (i < n && input_[i] == '.') {
                ++i;
                if (i >= n || !std::isdigit(static_cast<unsigned char>(input_[i]))) {
                    throw makeError(i, "expected digits after decimal point");
                }
                while (i < n && std::isdigit(static_cast<unsigned char>(input_[i]))) ++i;
            }
            std::string text = input_.substr(start, i - start);
            Token t;
            t.type = TokenType::Number;
            t.text = text;
            t.value = std::stod(text);
            t.pos = start;
            tokens.push_back(std::move(t));
            continue;
        }

        // Идентификатор: [A-Za-z_][A-Za-z0-9_]*
        if (std::isalpha(c) || c == '_') {
            std::size_t start = i;
            while (i < n &&
                   (std::isalnum(static_cast<unsigned char>(input_[i])) || input_[i] == '_')) {
                ++i;
            }
            Token t;
            t.type = TokenType::Identifier;
            t.text = input_.substr(start, i - start);
            t.pos = start;
            tokens.push_back(std::move(t));
            continue;
        }

        Token t;
        t.pos = i;
        switch (input_[i]) {
            case '+': t.type = TokenType::Plus;   break;
            case '-': t.type = TokenType::Minus;  break;
            case '*': t.type = TokenType::Star;   break;
            case '/': t.type = TokenType::Slash;  break;
            case '(': t.type = TokenType::LParen; break;
            case ')': t.type = TokenType::RParen; break;
            default:
                throw makeError(i, std::string("unexpected character '") + input_[i] + "'");
        }
        t.text = std::string(1, input_[i]);
        tokens.push_back(std::move(t));
        ++i;
    }

    Token end;
    end.type = TokenType::End;
    end.pos = n;
    tokens.push_back(std::move(end));
    return tokens;
}

} // namespace expr
