PYTHON_INCDIR=/usr/include/python3.6m 
PYTHON_LIBDIR=./
testPyGlue: main.o PyBase.o MayDay.o
	g++ -g  -std=c++17 -o testPyGlue main.o PyBase.o MayDay.o -L$(PYTHON_LIBDIR) -lpython3.6m
	
main.o: PyGlue.H PyGlue.hpp MayDay.H PyBase.H main.cpp
	g++ -g -std=c++17 -I$(PYTHON_INCDIR) -c main.cpp

PyBase.o: PyBase.H PyBase.cpp
	g++ -g  -std=c++17 -I$(PYTHON_INCDIR) -c PyBase.cpp

MayDay.o: MayDay.H MayDay.cpp
	g++ -g  -std=c++17 -I$(PYTHON_INCDIR) -c MayDay.cpp

clean:
	rm *.o testPyGlue

