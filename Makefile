CXX = g++

CXXFLAGS += -pipe

LIBS = -lGL -lm -lGLU -lGLEW -lglfw

BIN = mario

debug: CXXFLAGS += -g
debug: main

main: main.o ui.o
	${CXX} -o ${BIN} main.o ui.o ${CXXFLAGS} ${LIBS}

main.o: main.cpp MapEntityCommon.hpp MapLoader.hpp EntityHandler.hpp ui.h Movement.h
	${CXX} -c main.cpp ${CXXFLAGS} ${LIBS}

ui.o: ui.cpp ui.h
	${CXX} -c ui.cpp ${CXXFLAGS} ${LIBS}

clean:
	rm *.o
	rm mario
