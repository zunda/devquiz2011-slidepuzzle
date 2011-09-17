CFLAGS=-I. -O3
#CFLAGS=-I. -O3 -pg
#CFLAGS=-I. -g -Wall -DDEBUG -DCOUNT_MALLOC

default: solver

#solver: game.o board.o boards.o
solver: game.o board.o boards.o count-malloc.o

answers: solve

.PHONY: solve
solve: solve.rb solver
	touch solve.log
	date >> solve.log
	ruby solve.rb problems.txt 2>&1 | tee -a solve.log

.PHONY: benchmark
benchmark: solve.rb solver
	rm -rf answers
	@echo Please wait for 10 minutes
	-timeout 600 ruby solve.rb problems.txt > /dev/null 2>&1
	@echo Number of answers:
	ls answers | wc -l

answer.txt: answer.rb
	ruby answer.rb problems.txt >| $@

clean:
	rm -f solver *.o core.* gmon.out

.PHONY: zip
zip:
	cd ..; zip SlidePuzzle-`date +%y%m%d`.zip devquiz2011-slidepuzzle
