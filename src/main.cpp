#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        try {
            expr::Lexer lexer(line);
            expr::Parser parser(lexer.tokenize());
            auto ast = parser.parse();

            std::cout << "AST: " << expr::toSExpr(*ast) << "\n";
            std::cout << "RPN: " << expr::toRPN(*ast) << "\n";

            double value = expr::evaluate(*ast, nullptr);
            std::cout << "= " << value << "\n";
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
    return 0;
}
