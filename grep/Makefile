.PHONY: all clean test check rebuild s21_grep
CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: clean s21_grep

s21_grep:
	${CC} ${CFLAGS} s21_grep.c -o s21_grep

clean:
	rm -rf s21_grep.d* s21_grep log.txt

check: s21_grep
	cppcheck --enable=all --suppress=missingIncludeSystem s21_grep.c s21_grep.h
	CK_FORK=no leaks -atExit -- ./s21_grep -e while -e void s21_grep.c Makefile -o -h -n -f pattern.txt

test: t_grep.sh pattern.txt s21_grep
	sh t_grep.sh

rebuild: clean all