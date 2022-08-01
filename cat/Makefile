.PHONY: all clean test check rebuild s21_cat
CC=gcc
CFLAGS=-Wall -Werror -Wextra

all: clean s21_cat

s21_cat:
	${CC} ${CFLAGS} s21_cat.c -o s21_cat

clean:
	rm -rf s21_cat.d* s21_cat log.txt

check: s21_cat
	cppcheck --enable=all --suppress=missingIncludeSystem s21_cat.c s21_cat.h
	CK_FORK=no leaks -atExit -- ./s21_cat -est s21_cat.c

test: t_cat.sh bytes.txt s21_cat
	sh t_cat.sh

rebuild: clean all