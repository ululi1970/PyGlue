# PyGlue
A C++ library to pass data, run code and retrieve data to/from a Python interpreter from within a C++ code.
See the header files and the file main.cpp for examples of use. 
This class relies on C++17 features.   
At the moment, the following types can be passed as arguments and retrieved as return value from function: ```std::string```, ```int```, ```bool```, ```float```, ```double``` and ```std::size_t```. ```std::array<T,N>``` and ```std::vector<T>``` are either passed by reference (in which case a Numpy can be aliased to it on the Python side) or by copy. Which one occurs depends on the type ```T```. If ```T``` is an ```int```, ```float```, ```double``` or ```std::size_t```, or is a numeric data type for which ```T::isConvertibleToNumpy == true``` and ```T::numpyName()``` returns a ```std:;string```, the container is passed by reference. If not, it appears on the Python side as a tuple whose arguments are created by calls to ```T::pack()```.   
  For all types, both const and non const and rvalues arguments are supported. What this means is that a pointer to the buffer is passed to Python, which thus gives access to the data store. In case the array is flagged as const, the corresponding numpy is flagged as read_ony. Attemp to write to it will cause the program to stop. The symple types (boo, int,..) are always passed by value. If a return value is needed, it can be obtained as a return value.  Finally, a generic class can be passed if it implements a ```PyObject* T::pack()``` function that creates the appropriate data structure (see main.cpp for examples). Likewise, a class ```T``` that has a copy constructor and an assignment constructor can be created from data generated in Python if it implements
a constructor that acepts a ```PyObject*``` as argument.  


A simple example: Suppose we want to perform Z=X^M+A*Y^N, where A is double, and X,Y,Z are vector<double>s and 
M,N are integers. We already have a script, called spam in a module ham.py. 
```python (ham.py)
    import PyGlue as pg # this file contains utilities to convert input data.
    @pg.PYGLUE # this decorator turns the tuples passed by C++ into the corresponding objects in Python
    def spam(X,Y,Z,M,N,A): 
        Z=np.power(X,M)+A*np.power(Y,N)
        # no return. Z aliases the corresponding vector in C++
```
Note that we did not have to do anything to spam to run it. The decorator takes care of converting the inputs behind the scene. 

On the C++ side, 
```c++
    {....
    Py::start(); //To be called before the first Python(Return)Function() call
    vector<double> X,Y,Z; 
    int M,N;
    double A;
    ... // init/create the variables as needed

    Py::PythonFunction("ham","spam",X,Y,Z,M,N,A);

    Py::stop(); //To be called after all calls to Python(Return)Function()
    ... // Do something with Z

    }
```
Note that ```Py::PythonFunction``` requires the first two arguments to be ```std::string``` or string literals. The number and type of the remaining arguments 
is arbitrary, without the need to do anything else (e.g., create wrappers). 
PyGlue will use template metaprogramming to 
generate the appropriate code. The only thing that the user needs to do is to make sure that ham.py contains a function spam which accepts 5 arguments.
For examples on how to transfer more complex data structures and more example of usages 
see main.cpp.   

# Design considerations

The class is purely static and not thread safe. However, this is not a problem since the GIL of the Python interpreter means that only one thread at a time can run scripts. ```Py::start(InitCommands)``` must be 
called before any other calls. The Commands argument is a ```std::vector<std::string>``` that contains instructions to configure the interpreter. If not provided, a set of dafults commands is run (defined in ```PyBase::DefaultInitCommands```). To close the intepreter, call ```Py::stop()```.   
The general philosophy for data handling is that data that lives on stack is passed by copy, whereas data that lives on heap is passed by reference. The exception is ```std::vector<T>```, with ```T``` not a numeric type (POD). In this case, a copy is made of each element of the vector.  In general, it is a good idea to limit data allocation on heap 
to the C++ side and alias to it an appropriate object in Python. However, with care, it is also possible to allocate data on heap on the Python side, and alias a C++ object. The tricky part in this case, discussed below, is to ensure that the Python garbage collector does not release the memory before C++ is done with it.    

# Memory management

Python's memory is managed by an automatic garbage collector. Any object in Python is reference counted: when the reference count hits zero, the garbage collector gets to work. C++ is of course different: For classes that require allocation, the user has to provide an explicit destructor to make sure resources are released when done with an instantiation of the class. When we interact with the Python interpreter, we need to be cognizant of the difference. With few exceptions, all interactions with the interpreter generate pointers to PyObject objects. While totally opaque from the point of view of C++, they can be classified in three categories: "New", "Stolen" and "Borrowed" (this is Python lingo). "New" objects are generated by the interpreter and are passed to C++, e.g. as the return value of a function. When done with a "New" object, it needs to be disposed of properly, by calling ```Py_DECREF()``` on it. If there is a chance that the object may be a null pointer (e.g., when returning from a Python function that does not have an explicit return XXX statement), use ```Py_XDECREF()``` instead. "Stolen" objects are objects that are included in compound structures: e.g., when we pack a bunch of PyObjects into a tuple to be passed to a function as a single argument. Typically this happens with ```XXX_SetItem()``` calls. The "stolen" objects will be released when the structure they have been included into is ```Py_DECREF()```. 
In other words, ```Py_DECREF()``` descends into each object and checks if their ref count is zero. If so, they are garbage collected.  
When processing a request to run a Python script in C++, the arguments are stored as they are processed by the packing function in a vector. Then this vector is processed and all entries are packed into a tuple, which is finally passed to the Python interpreter. Upon return, the pointer of this tuple is Decref'd, which in turns applies the garbage collector to everything that is contained in it.  
The trickiest objects are the "borrowed" ones. These are objects that have been gotten from another object, typically with a ```XXX_GetItem()``` call. If we are planning to hold on the object for a while, during which there is a potential for the other object to get garbage collected, we need to increase the reference count of the borrowed object, to prevent from being garbage collected should the original owner be garbage collected. We do this by ```Py_INCREF()```. Normally, we use GetItem to extract 
data as returned by Python scripts, typically in constructors and there should not be the need to INCREF them. See the definition of a Python constructed class in main.cpp for an example that needs ```Py_INCREF()```  

If all of this sounds confusing, it is because it is. For this reason, the best course of action is not to have PyObjects loose in the C++ code. These issues arise only of you decide to implement packing functions for specific classes. Keep uses of ```XXX_GetItem()``` limited to copying actual data in the constructors. 

# Error management

Presumably, the user is interested in accessing the Python interpreter because Python is (a) simpler and more high-level than C++, and (b) has a huge library that covers tasks ranging from AI calculations to sending SMS and Emails. In other words, it can make life easier. On the other hand, the Python  interpreter adds another layer of possible snafus. It is therefore important to handle exceptions properly. 
In this library, we adopt two strategies. First, when we have reason to suspect that something wrong happened on the Python side, e.g. when we get a null pointer instead of a good one, or ```PyErr_Occurred()==true```, we use ```PyErr_Print()``` to print the errors as reported by the interpreter. We also have a function Py::lintcatcher() that is designed to catch the exceptions, print some context from the C++ side, and exit if needed. The lintcatcher falls back on a separate class to handle how to print messages and to exit (MayDay). This to make it easy to interface with existing libraries for error handling (e.g., by stopping gracefully MPI runs, or output more diagnostic,...). 
As far as PyGlue is concerned, MayDay needs to provide two static functions: ```MayDay::Error(std:string ErrMsg)``` and ```MayDay::Warning(std:string WarningMSG)```. The difference is that ``::Error`` should interrupt the program after printing ErrMSg, whereas ```::Warning``` only prints WarningMSg. 
We provide a simple implementation in ```MayDay.H```. 

Due to the nature of template metaprogramming, it is not 
advisable to continue a program after an exception has raised: you never know what the compiler has prepared for you. Just exit and fix the problem.  

# Types

The library handles the 5 basic data types (bool, int, float, double and std::string). For these data types,  ```std::array<T,N>``` can be used as arguments to scripts and as return value of scripts. In all cases,
data is passed by value. The type ```std::vector<T>``` is treated differently: for numeric types, it is passed in such a way that on the Python side a Numpy is aliased to it. Essentially, this means that the vector is passed by reference. 
Generic types T can be used as arguments to functions if the type provides a function ```PyObject* T::pack()```. Likewise, a generic type can be the return of a script if the type T implements a ```T::T(PyObject* pin)``` constructor. For such types, ```std::array<T,N>``` and ```std::vector<T>``` can be used as arguments to scripts, and ```std::array<T,N>``` as return argument of scripts.   


For ```const``` data which is aliased to a Numpy,  we set ```PyBuf_READ``` when we create the pointer to the buffer. This will cause the program to stop if a function on the Python side will try to write to a container declared const. Therefore, constantness is preserved for data which is passed by reference.   

