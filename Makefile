CC = gcc
FLAGS = -Wall
SRC := $(wildcard src/*.c)

run: build
	./test

build: move
	@echo $(SRC)
	@echo $(HDR)
	$(CC) $(FLAGS) -I ./ $(SRC) -o test
	
move:
	touch test
	mv test test_old