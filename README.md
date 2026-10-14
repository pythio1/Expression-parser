# expr-parser

Рекурсивный нисходящий парсер арифметических выражений на C++17.
Строит AST, умеет печатать его в виде S-выражения и в RPN, и вычислять значение.

## Грамматика

```
expression → term (("+" | "-") term)*
term       → unary (("*" | "/") unary)*
unary      → ("+" | "-") unary | primary
primary    → NUMBER | IDENT | "(" expression ")"
```

- `NUMBER` — целое или дробное (`3`, `3.14`).
- `IDENT`  — `[A-Za-z_][A-Za-z0-9_]*` (для подстановки переменных).
- Пробелы игнорируются.
- `-` и `+` поддерживаются как бинарные и унарные; унарный имеет более высокий приоритет, чем `*`/`/`.

## Сборка

Через CMake:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Через Makefile:

```bash
make
make test
```

## Использование

```bash
$ echo "(1 + 2) * 3.5 - x" | ./build/expr_cli
AST: (- (* (+ 1 2) 3.5) x)
RPN: 1 2 + 3.5 * x -
error: no variable resolver provided for 'x'
```

(последняя строка — потому что CLI не умеет переменные; для них используйте API.)

## API

```cpp
#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"

expr::Lexer lx("2*(x+1)");
expr::Parser p(lx.tokenize());
auto ast = p.parse();

double v = expr::evaluate(*ast, [](const std::string& name) {
    if (name == "x") return 10.0;
    throw expr::EvalError("unknown variable: " + name);
}); // 22
```

Все ошибки — через исключения `expr::LexerError`, `expr::ParseError`, `expr::EvalError`.

## Возможные расширения

- Возведение в степень `^` (правоассоциативно).
- Оператор `%`, сравнения и логические операции.
- Компиляция AST в байт-код / JIT.
- Поддержка переменных с присваиванием (`x = 3 + 4`).
