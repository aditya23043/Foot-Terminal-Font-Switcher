build: src/main.c
	gcc src/main.c -o bin/main -g -Wall

run: bin/main
	./bin/main
