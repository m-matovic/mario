CC = gcc
CXX = g++

CFLAGS = -O2
CXXFLAGS = -O2

LIBS = -lGL -lm -lGLU -lGLEW -lglfw

mario: main.cpp MapLoader.h ui.h
	${CXX} -o mario main.cpp ${CXXFLAGS} ${LIBS}

clean: mario
	rm mario
