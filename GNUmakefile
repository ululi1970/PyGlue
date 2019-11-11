PYTHON_INCDIR=/usr/include/python3.6m 
PYTHON_LIBDIR=./
testPyGlue: main.o PyGlue.o MayDay.o
	g++ -g -o testPyGlue main.o PyGlue.o MayDay.o -L$(PYTHON_LIBDIR) -lpython3.6m
	
main.o: PyGlue.H MayDay.H main.cpp
	g++ -g -I$(PYTHON_INCDIR) -c main.cpp

PyGlue.o: PyGlue.H PyGlue.cpp
	g++ -g -I$(PYTHON_INCDIR) -c PyGlue.cpp

MayDay.o: MayDay.H MayDay.cpp
	g++ -g -I$(PYTHON_INCDIR) -c MayDay.cpp

clean:
	rm *.o testPyGlue

