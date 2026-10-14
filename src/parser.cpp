#include "parser.hpp"
#include <sstream>

namespace expr {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

const Token& Parser::peek() const { return tokens_[pos_]; }

const Token& Parser::advance() { return tokens_[pos_++]; }

bool Parser::match(TokenType t) {
    if (peek().type == t) {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string& msg) const {
    std::ostringstream oss;
    oss << "Parse error at position " << peek().pos << ": " << msg;
    throw ParseError(oss.str());
}

NodePtr Parser::parse() {
    NodePtr root = parseExpression();
    if (peek().type != TokenType::End) {
        error("unexpected token '" + peek().text + "'");
    }
    return root;
}

NodePtr Parser::parseExpression() {
    NodePtr node = parseTerm();
    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        char op = (peek().type == TokenType::Plus) ? '+' : '-';
        advance();
        NodePtr rhs = parseTerm();
        node = std::make_unique<BinaryNode>(op, std::move(node), std::move(rhs));
    }
    return node;
}

NodePtr Parser::parseTerm() {
    NodePtr node = parseUnary();
    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {
        char op = (peek().type == TokenType::Star) ? '*' : '/';
        advance();
        NodePtr rhs = parseUnary();
        node = std::make_unique<BinaryNode>(op, std::move(node), std::move(rhs));
    }
    return node;
}

NodePtr Parser::parseUnary() {
    if (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        char op = (peek().type == TokenType::Plus) ? '+' : '-';
        advance();
        NodePtr operand = parseUnary();
        return std::make_unique<UnaryNode>(op, std::move(operand));
    }
    return parsePrimary();
}

NodePtr Parser::parsePrimary() {
    const Token& t = peek();

    if (t.type == TokenType::Number) {
        advance();
        return std::make_unique<NumberNode>(t.value);
    }
    if (t.type == TokenType::Identifier) {
        advance();
        return std::make_unique<VariableNode>(t.text);
    }
    if (t.type == TokenType::LParen) {
        advance();
        NodePtr node = parseExpression();
        if (!match(TokenType::RParen)) {
            error("expected ')'");
        }
        return node;
    }

    if (t.type == TokenType::End) {
        error("unexpected end of input");
    }
    error("unexpected token '" + t.text + "'");
}

} // namespace expr
