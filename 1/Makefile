GCC_FLAGS = -Wextra -Werror -Wall -Wno-gnu-folding-constant

all: libcoro.c solution.c
	gcc $(GCC_FLAGS) libcoro.c solution.c

clean:
	rm a.out

debug:
	gcc -g solution.c libcoro.c  && gdb --args ./a.out test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt
run:
	gcc solution.c libcoro.c && ./a.out test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt
test: 
	gcc ../test.c -o test && valgrind ./test
val:
	gcc -ggdb3 solution.c libcoro.c && valgrind --log-file=v.out --verbose --track-origins=yes --leak-check=full ./a.out test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt