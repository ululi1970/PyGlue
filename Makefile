

GCC=g++ -g -std=c++11  

PYVER=3.6



LIBDIR= -L/usr/lib/python$(PYVER)-config-$(PYVER)m-x86_64-linux-gnu/ 
INCDIR= -I./ -I/usr/include/python$(PYVER)m 
LIBS = -lpython$(PYVER)m

.SUFFIXES: .cpp .H .o 
default: main

.cpp.o: PyGlue.H MayDay.H
	$(GCC) $(INCDIR) -c $<

main.o: PyGlue.H MayDay.H

PyGlue.o: PyGlue.H MayDay.H

MayDay.o: MayDay.H 

main:  PyGlue.H MayDay.H PyGlue.o main.o MayDay.o
	$(GCC) $(LIBDIR)  MayDay.o PyGlue.o main.o $(LIBS) -o main 
clean:
	rm -f *~
	rm -f *.o

