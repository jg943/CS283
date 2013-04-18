.PHONY: build run view test clean

build: connect4.c
	gcc connect4.c -o connect4.out

run: build
	./connect4.out ${GAMES} ${DIMENSION}

test: build
	./connect4.out 4 8

view:
	less connect4.c

clean:
	-\rm *.out
