CC=gcc

mario: main.c
	${CC} -o mario main.c

clean:
	rm mario
