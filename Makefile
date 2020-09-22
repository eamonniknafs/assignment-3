.PHONY: all clean test gitlog

all: assignment-3

assignment-3: assignment-3.c
	${CC} -std=c99 -g assignment-3.c -o assignment-3

test: assignment-3 gitlog
	./test.sh

test1: assignment-3
	./test.sh 1

test2: assignment-3
	./test.sh 2

test3: assignment-3
	./test.sh 3

test4: assignment-3
	./test.sh 4

test5: assignment-3
	./test.sh 5

test6: assignment-3
	./test.sh 6

test7: assignment-3
	./test.sh 7

test8: assignment-3
	./test.sh 8

test9: assignment-3
	./test.sh 9

test10: assignment-3
	./test.sh 10

test11: assignment-3
	./test.sh 11

test12: assignment-3
	./test.sh 12

test13: assignment-3
	./test.sh 13

test14: assignment-3
	./test.sh 14

test15: assignment-3
	./test.sh 15

test16: assignment-3
	./test.sh 16

bonus1: assignment-3
	./test.sh 1 bonus

bonus2: assignment-3
	./test.sh 2 bonus

gitlog:
	git log -p > gitlog.txt

clean:
	-${RM} assignment-3
