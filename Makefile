

GCC=g++ -O3 -std=c++11  

PYVER=3.4



LIBDIR= -L/usr/lib/python$(PYVER)-config-$(PYVER)m-x86_64-linux-gnu/ 
INCDIR= -I./ -I/usr/include/python$(PYVER)m 
LIBS = -lpython$(PYVER)m

.SUFFIXES: .cpp .H .o 
default: main

.cpp.o: PyGlue.H
	$(GCC) $(INCDIR) -c $<

main.o: PyGlue.H

PyGlue.o: PyGlue.H

main:  PyGlue.H PyGlue.o main.o
	$(GCC) $(LIBDIR) $(LIBS) PyGlue.o main.o -o main 
clean:
	rm -f *~
	rm -f *.o

