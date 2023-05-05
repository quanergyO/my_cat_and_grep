FLAGS = -Wall -Wextra -Werror

all: s21_cat

s21_cat: makefile s21_cat.h s21_cat.c
	gcc $(FLAGS) s21_cat.c -o s21_cat

check:
	clang-format -style=Google -i *.c
	clang-format -style=Google -n *.c
	clang-format -style=Google -i *.h
	clang-format -style=Google -n *.h

rebuild:
	make clean
	make all

clean:
	rm -f s21_cat