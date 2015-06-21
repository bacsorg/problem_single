# Problem

## "Single" problem
Problem type implemented here is "single".
Solutions for such problems run independently,
i.e. raw result of solution does not depend on other solutions.
Solution is executed multiple times using tests.

## Driver

For this problem type different formats are supported.
Format is `$type#$driver`.

Problem is read by [bacs::problem::single::driver](../include/bacs/problem/single/driver.hpp)
instance which "understand" particular format. Drivers are implemented via plugins.

## Generator

Problem is converter into internal representation via
[bacs::problem::single::generator](../include/bacs/problem/single/generator.hpp) instance
which uses problem's driver to read problem data and create appropriate representation.
Generator's implementations are placed in
[bacs::problem::generators](../src/generators) namespace.

**TODO** how is generator related to problem type
