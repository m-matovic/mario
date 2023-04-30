CC = gcc
CXX = g++

CFLAGS = -O2
CXXFLAGS = -O2

LIBS = -lGL -lm -lGLU -lGLEW -lglfw

BIN = mario

main: main.o ui.o
	${CXX} -o ${BIN} main.o ui.o ${CXXFLAGS} ${LIBS}

main.o: main.cpp MapLoader.h ui.h
	${CXX} -c main.cpp ${CXXFLAGS} ${LIBS}

ui.o: ui.c ui.h
	${CXX} -c ui.c ${CFLAGS} ${LIBS}

clean:
	rm *.o
	rm mario
