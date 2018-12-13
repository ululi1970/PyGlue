# PyGlue
A simple C++ library to pass variables, run code and retrieve data on a Python interpreter from within a C++ code.
See the header file and the file main.cpp for examples of use. 
This class relies on C++11 features. On GCC 4.8, 
the flag -std=c++11 must be set.  
At the moment, the following types can be passed as arguments and retrieved as return value from function: std::string, int, bool, float and double. The library also supports array<T> and valarray<T> as function arguments, wiht T being int, float and double. Since normally these types are used to store large data, we only implemented them as function arguments to avoid unnecessary deep copies. For all types, both const and non const arguments are supported. However, note that regardless, the symple types (boo, int,..) are always passed by value. If a return value is needed, it can be obtained as a return value. The compound data is alwasy passed by reference. What this means is that a pointer to the buffer is passed to Python, which thus gives access to the data store. In case the variable is flagged as const, the corresponding numpy is flagged as read_ony. Attemp to write to it will cause the program to stop. 


A simple example: Suppose we want to perform Z=X^M+A*Y^N, where A is double, and X,Y,Z are valarray<double>s and 
M,N are integers. In Python this would be accomplished writing a py file (call it ham.py)

import PyGlue as pg # this file contains utilities to convert input data.

def spam(X,Y,Z,M,N,A):
    x,y,z=pg.ValarrayToNumpy(X),pg.ValarrayToNumpy(Y),pg.ValarrayToNumpy(Z) # the tuples passed 
    z=np.power(x,M[0])+A[0]*np.power(y,N)
    # since x,y and z access the same memory buffer of X,Y and Z, there is no need to call a
    # NumpyToValarray function at the end. 


On the C++ side, 

{....
Py Python; 
valarray<float> X,Y,Z; 
int M,N;
float A;
... // init/create the variables as needed

Python.PythonFunction("ham","spam",X,Y,Z,M,N,A);

... // Do something with Z

}

Note that Py::PythonFunction requires the first two std::string arguments. The number and type of the remaining arguments 
is arbitrary, without the need to do anything else (e.g., create wrappers). PyGlue will use template metaprogramming to 
generate the appropriate code. The only thing that the user needs to do is to make sure that ham.py contains a function spam which accepts 5 arguments, and use the appropriate functions to convert them to numpy if necessary. 
See also the main.cpp file for more example of usage.  

# Design considerations

The library is designed to minimize deep copies. To this effect, only basic types (bool, std::string, int, float and double) are passed by value, and can be returned  as function values. Data rich structures (e.g., valarrays) are instead aliased into numpys, without deep copies. 

# Memory management

Python's memory is managed by an automatic garbage collector. Any object in Python is reference counted: when the reference count hits zero, the garbage collector gets to work. C++ is of course different: For classes that require allocation, the user has to provide an explicit destructor to make sure resources are released when done with an instantiation of the class. When we interact with the Python interpreter, we need to be cognizant of the difference. With few exceptions, all interactions with the interpreter generate pointers to PyObject objects. While totally opaque from the point of view of C++, they can be classified in three categories: "New", "Stolen" and "Borrowed" (these is Python lingo). "New" objects are generated by the interpreter and are passed to C++, e.g. as the return value of a function. When done with a "New" object, it needs to be disposed of properly, by calling Py_DECREF() on it. If there is a chance that the object may be a null pointer (e.g., when returning from a Python function that does not have an explicit return XXX statement), use Py_XDECREF() instead. "Stolen" objects are objects that are included in compound structures: e.g., when we pack a bunch of PyObjects into a tuple to be passed to a function as a single argument. Typically this happens with XXX_SetItem() calls. The "stolen" objects will be released when the structure they have been included into is Py_DECREF().
In other words, Py_DECREF descends into each object and checks if their ref count is zero. If so, they are zapped.  
The trickiest objects are the "borrowed" ones. These are objects that have been gotten from another object, typically with a XXX_GetItem() call. If we are planning to hold on the object for a while, during which there is a potential for the other object to get zapped, we need to increase the reference count of the borrowed object, to prevent from being zapped should the original owner be zapped. We do this by Py_INCREF(). 

If all of this sounds confusing, it is because it is. For this reason, the best course of action is not to have PyObjects loose in the C++ code. Thus, by design PyGlue hides all references to PyObjects. The public functions only deal with non PyObjects. That way, hopefully, we can keep tab on the PyObjects and avoid momery leaks and dangling pointers. However, note that Python is not completely safe from memory issue, so be careful nonetheless. 

# Error management

Presumably, the user is interested in accessing the Python interpreter because Python is (a) simpler and more high-level than C++, and (b) has a huge library that covers tasks raning from AI calculations to sending SMS and Emails. In other words, it can make life easier. On the other hand, the Python  interpreter adds another layer of possible snafus. It is therefore important to handle exceptions properly. 
In this library, we adopt two strategy. First, when we have reason to suspect that something wrong happened on the Python side, e.g. when we get a null pointer instead of a good one, or PyErr_Occurred()==true, we use PyErr_Print() to print the errors as reported by the interpreter. We also have a function Py::lintcatcher() that is designed to catch the exceptions, print some context from the C++ side, and exit if needed. The lintcatcher falls back on a separate class to handle how to print messages and to exit (MayDay). This to make it easy to interface with existing libraries for error handling (e.g., by stopping gracefully MPI runs, or output more diagnostic,...). 
As far as PyGlue is concerned, MayDay needs to provide two static functions: MayDay::Error(std:string ErrMsg) and MayDay::Warning(std:string WarningMSG). The difference is that ::Error should interrupt the program after printing ErrMSg, whereas ::Warning only prints WarningMSg. 
We provide a simple implementation in MayDay.H. 

Due to the nature of template metaprogramming, it is not 
advisable to continue a program after an exception has raised: you never know what the compiler has prepared for you. Just exit and fix the problem.  

# Types

The library handles the 5 basic data types (bool, int, float, double and std::string) and valarray<T> and array<T> with T (int, double, float). Functions that allow return only do for the basic data types. The idea is that if a function needs to fill a data container, it should be passed as argument rather than doing a deep copy. Basic data types are always passed by value, so the const and noncost versions are the same. For containers, in the const case, we set PyBuf_READ when we create the pointer to the buffer. This will cause the program to stop if a function on the Python side will try to write to a container declared const. Unfortunately, I do not think there is a way to enforce it at the compile level. 
