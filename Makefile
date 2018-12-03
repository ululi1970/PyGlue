

GCC=g++ 
#-std=c++11  

PYVER=3.6



LIBDIR= -lpython$(PYVER)m
INCDIR= -I./ -I/usr/include/python$(PYVER)m 


.SUFFIXES: .cpp .H .o 
default: main

.cpp.o: PyGlue.H
	$(GCC) $(INCDIR) -c $<

main.o: PyGlue.H

PyGlue.o: PyGlue.H

main:  PyGlue.H PyGlue.o main.o
	$(GCC) $(LIBDIR)  PyGlue.o main.o -o main
clean:
	rm -f *~
	rm -f *.o

