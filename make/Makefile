CC = gcc
CXX = g++

CFLAGS += -pipe
CXXFLAGS += -pipe

LIBS = -lGL -lm -lGLEW -lglfw

BIN = mario

main: main.o ui.o
	${CXX} -o ${BIN} main.o ui.o ${CXXFLAGS} ${LIBS}

main.o: main.cpp MapLoader.h ui.h
	${CXX} -c main.cpp ${CXXFLAGS} ${LIBS}

ui.o: ui.cpp ui.h
	${CXX} -c ui.cpp ${CFLAGS} ${LIBS}

clean:
	rm *.o
	rm mario
