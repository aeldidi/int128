#! /usr/bin/env sh
# requires gcc with coverage support, gcov, lcov, and genhtml

gcc -Wall -pedantic-errors -std=c99 -Werror -fsanitize=address,undefined -g test.c -o test --coverage
./test
gcov test.c
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
genhtml coverage.info --output-directory coverage-html --rc genhtml_branch_coverage=1 
