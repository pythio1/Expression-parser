#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {

int g_failures = 0;
int g_total = 0;

void expectEq(const std::string& what, const std::string& got, const std::string& want) {
    ++g_total;
    if (got != want) {
        ++g_failures;
        std::cerr << "[FAIL] " << what << "\n  got:  " << got << "\n  want: " << want << "\n";
    }
}

void expectEq(const std::string& what, double got, double want, double eps = 1e-9) {
    ++g_total;
    if (std::fabs(got - want) > eps) {
        ++g_failures;
        std::cerr << "[FAIL] " << what << "\n  got:  " << got << "\n  want: " << want << "\n";
    }
}

void expectThrows(const std::string& what, const std::string& input) {
    ++g_total;
    try {
        expr::Lexer lx(input);
        expr::Parser p(lx.tokenize());
        auto ast = p.parse();
        (void)ast;
        ++g_failures;
        std::cerr << "[FAIL] " << what << ": expected throw, got success\n";
    } catch (const std::exception&) {
        // ok
    }
}
void expectThrowsEval(const std::string& what, const std::string& input) {
    ++g_total;
    try {
        expr::Lexer lx(input);
        expr::Parser p(lx.tokenize());
        auto ast = p.parse();
        (void)expr::evaluate(*ast, nullptr);
        ++g_failures;
        std::cerr << "[FAIL] " << what << ": expected throw on eval, got success\n";
    } catch (const std::exception&) {
        // ok
    }
}

std::string parseSExpr(const std::string& input) {
    expr::Lexer lx(input);
    expr::Parser p(lx.tokenize());
    auto ast = p.parse();
    return expr::toSExpr(*ast);
}

std::string parseRPN(const std::string& input) {
    expr::Lexer lx(input);
    expr::Parser p(lx.tokenize());
    auto ast = p.parse();
    return expr::toRPN(*ast);
}

double parseEval(const std::string& input) {
    expr::Lexer lx(input);
    expr::Parser p(lx.tokenize());
    auto ast = p.parse();
    return expr::evaluate(*ast, nullptr);
}

} // namespace

int main() {
    // Приоритеты и ассоциативность
    expectEq("1+2*3",         parseSExpr("1+2*3"),        "(+ 1 (* 2 3))");
    expectEq("(1+2)*3",       parseSExpr("(1+2)*3"),      "(* (+ 1 2) 3)");
    expectEq("1-2-3 left",    parseSExpr("1-2-3"),        "(- (- 1 2) 3)");
    expectEq("1+2+3 left",    parseSExpr("1+2+3"),        "(+ (+ 1 2) 3)");
    expectEq("8/4/2 left",    parseSExpr("8/4/2"),        "(/ (/ 8 4) 2)");

    // Унарные операторы
    expectEq("-5",            parseSExpr("-5"),           "(-u 5)");
    expectEq("- -5",          parseSExpr("--5"),          "(-u (-u 5))");
    expectEq("-2*3",          parseSExpr("-2*3"),         "(* (-u 2) 3)");
    expectEq("2*-3",          parseSExpr("2*-3"),         "(* 2 (-u 3))");
    expectEq("-(2+3)",        parseSExpr("-(2+3)"),       "(-u (+ 2 3))");

    // Пробелы
    expectEq("spaces",        parseSExpr("  1  +  2 * 3 "), "(+ 1 (* 2 3))");
    expectEq("tabs",          parseSExpr("\t1\t+\t2"),    "(+ 1 2)");

    // Переменные
    expectEq("var",           parseSExpr("x"),            "x");
    expectEq("var mix",       parseSExpr("foo_bar + x1"), "(+ foo_bar x1)");

    // Числа с плавающей точкой
    expectEq("float",         parseSExpr("3.14"),         "3.14");

    // Вычисление
    expectEq("eval 1+2*3",    parseEval("1+2*3"),         7.0);
    expectEq("eval (1+2)*3",  parseEval("(1+2)*3"),       9.0);
    expectEq("eval 10/4",     parseEval("10/4"),          2.5);
    expectEq("eval -3+5",     parseEval("-3+5"),          2.0);
    expectEq("eval 2*-3",     parseEval("2*-3"),         -6.0);

    // RPN
    expectEq("rpn 1+2*3",     parseRPN("1+2*3"),         "1 2 3 * +");
    expectEq("rpn (1+2)*3",   parseRPN("(1+2)*3"),       "1 2 + 3 *");
    expectEq("rpn -5+1",      parseRPN("-5+1"),          "5 -u 1 +");

    // Ошибки
    expectThrows("empty",         "");
    expectThrows("unmatched (",   "(1+2");
    expectThrows("unmatched )",   "1+2)");
    expectThrows("bad char",      "1 $ 2");
    expectThrows("trailing jumk", "1 2");
    expectThrows("decimal alone", "1.");
    expectThrows("only plus",     "+");
    // 1++2 и 1--2 — валидные выражения (унарный +/-, как в C/C++)
    expectEq("1++2 = 1+(+2)",     parseEval("1++2"),  3.0);
    expectEq("1--2 = 1-(-2)",     parseEval("1--2"),  3.0);
    expectEq("1+-2",              parseEval("1+-2"), -1.0);
    expectEq("1-+2",              parseEval("1-+2"), -1.0);

    // деление на ноль — ошибка этапа вычисления, не парсинга
    expectThrowsEval("div by zero", "1/0");
    if (g_failures == 0) {
        std::cout << "All " << g_total << " tests passed.\n";
        return 0;
    }
    std::cerr << g_failures << " / " << g_total << " tests failed.\n";
    return 1;
}
