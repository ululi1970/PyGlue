
GCC=g++ -g -fPIC -std=c++11  

PYVER=3.4

SOMAR=/mnt/c/Users/Alberto/research/LES-SOMAR/src

LIBDIR= -L/usr/lib/python$(PYVER)-config-$(PYVER)m-x86_64-linux-gnu/ -L./
INCDIR= -I./ -I/usr/include/python$(PYVER)m 
LIBS = -lpython$(PYVER)m  

.SUFFIXES: .cpp .H .o 
default: main

.cpp.o: PyGlue.H
	$(GCC) $(INCDIR) -c $<

main.o: PyGlue.H MayDay.o 

PyGlue.o: PyGlue.H MayDay.o

MayDay.o: MayDay.H

main:  PyGlue.H  PyGlue.o MayDay.o main.o 
	$(GCC) $(LIBDIR)  PyGlue.o MayDay.o main.o  $(LIBS) -o main 
clean:
	rm -f *~
	rm -f *.o

