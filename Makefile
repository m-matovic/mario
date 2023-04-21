CXX=g++

mario: main.cpp
	${CXX} -o mario main.cpp

clean:
	rm mario