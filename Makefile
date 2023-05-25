CXX = g++

CXXFLAGS += -pipe

LIBS = -lGL -lm -lGLU -lGLEW -lglfw

BIN = mario

debug: CXXFLAGS += -g
debug: main

main: main.o ui.o MapLoader.o EntityHandler.o
	${CXX} -o ${BIN} main.o ui.o EntityHandler.o MapLoader.o ${CXXFLAGS} ${LIBS}

main.o: main.cpp MapEntityCommon.hpp ui.h Movement.h
	${CXX} -c main.cpp ${CXXFLAGS} ${LIBS}

ui.o: ui.cpp ui.h MapEntityCommon.hpp
	${CXX} -c ui.cpp ${CXXFLAGS} ${LIBS}

MapLoader.o: MapLoader.cpp MapEntityCommon.hpp
	${CXX} -c MapLoader.cpp ${CXXFLAGS} ${LIBS}

EntityHandler.o: EntityHandler.cpp MapEntityCommon.hpp
	${CXX} -c EntityHandler.cpp ${CXXFLAGS} ${LIBS}

clean:
	rm *.o
	rm mario
