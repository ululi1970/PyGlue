PYTHON_INCDIR=/usr/include/python3.10/
PYTHON_LIBDIR=./
SRC=../src/
ERR=../ErrorHandling/
testPyGlue: main.o PyBase.o MayDay.o
	g++ -g  -std=c++17 -o testPyGlue main.o PyBase.o MayDay.o -L$(PYTHON_LIBDIR) -lpython3.10
	
main.o: $(SRC)PyGlue.H $(SRC)PyGlue.hpp $(ERR)MayDay.H $(SRC)PyBase.H main.cpp
	g++ -g -std=c++17 -I$(PYTHON_INCDIR) -I$(ERR) -I$(SRC) -c main.cpp

PyBase.o: $(SRC)PyBase.H $(SRC)PyBase.cpp
	g++ -g  -std=c++17 -I$(PYTHON_INCDIR) -I$(ERR) -I$(SRC) -c $(SRC)PyBase.cpp

MayDay.o: $(ERR)MayDay.H $(ERR)MayDay.cpp
	g++ -g  -std=c++17 -I$(PYTHON_INCDIR) -I$(ERR) -c $(ERR)MayDay.cpp

clean:
	rm *.o testPyGlue

