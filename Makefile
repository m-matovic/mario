CXX = g++

CXXFLAGS += -pipe

LIBS = -lGL -lm -lGLU -lGLEW -lglfw

BIN = mario

debug: CXXFLAGS += -g
debug: main

main: main.o ui.o MapEntityCommon.o
	${CXX} -o ${BIN} main.o ui.o EntityHandler.o MapLoader.o ${CXXFLAGS} ${LIBS}

main.o: main.cpp MapEntityCommon.hpp ui.h Movement.h
	${CXX} -c main.cpp ${CXXFLAGS} ${LIBS}

ui.o: ui.cpp ui.h
	${CXX} -c ui.cpp ${CXXFLAGS} ${LIBS}

MapEntityCommon.o: MapLoader.cpp EntityHandler.cpp MapEntityCommon.hpp
	${CXX} -c EntityHandler.cpp MapLoader.cpp ${CXXFLAGS} ${LIBS}

clean:
	rm *.o
	rm mario
