APP_NAME=tutorial

CC = g++
CFLAGS = -fopenmp -O3 -Wall

all: hello loop sqrt

$(APP_NAME): hello loop sqrt

hello: hello.c
	$(CC) $(CFLAGS) $^ -o $@

loop: loop.c
	$(CC) $(CFLAGS) $^ -o $@

sqrt: sqrt.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f hello loop sqrt

# For a given rule:
# $< = first prerequisite
# $@ = target
# $^ = all prerequisite