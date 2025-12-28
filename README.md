# Interprelator

Interpreter-like implementation of a calculator in C based on Thorsten Ball's book [Writing an Interpreter in Go](https://interpreterbook.com).

## Installation

Run the following commands:
```bash
git clone git@github.com:cfung89/Interprelator.git
cd Interprelator
make
```

The binary is compiled to `bin/main`.

## Features

- Basic features: Addition, subtraction, multiplication, division, exponentiation.
- Follows priority of operations.
- Built-in functions and variables: `sqrt(x)`
  - `rootn(x, n)`
  - `log(x)`
  - `logn(x, n)`
  - `ln(x)`
  - `sin(x)`
  - `cos(x)`
  - `tan(x)`
  - `asin(x)`
  - `acos(x)`
  - `atan(x)`
  - `sum(start, end, expression)` and `i` for the iterator of the sum.
  - `pi`
  - `e` or `e(x)`
  - `ans`

>[!WARNING]
>Certain memory-related issues may arise with invalid inputs (use-after-free bugs and memory leaks).
