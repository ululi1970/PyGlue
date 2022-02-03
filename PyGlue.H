/*******************************************************************************
 *  PyGlue
 *  Developed Alberto Scotti
 *  Copyright (C) 2022
 *    
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 *
 *  For up-to-date contact information, please visit the repository homepage,
 *  https://github.com/somarhub.
 ******************************************************************************/
#ifndef ___PyGlue_H__INCLUDED___
#define ___PyGlue_H__INCLUDED___

#include <Python.h>
#include <iostream>
#include <map>
#include <sstream>
#include <type_traits>
#include <vector>
#include <array>
#include "MayDay.H"

// a couple of helper macros
#define TYPEtoSTR(X)                 \
  {                                  \
    if (std::is_same<T, X>::value) { \
      return #X;                     \
    }                                \
  }


#define SAME(X) std::is_same<T, X>::value


// The purpose of this class is to be able to run functions on Python
// from within a C++ code. The use is
/* {Py Python;
    ....
    // this calls shows how to execute moduleName.funcName(arg1,arg2,arg3,...)
    // on the Python interpreter. The return value
    // (Python functions have always a return value, even if not explicitely
   declared)
    // is discarded
    Python.PythonFunction("moduleName", "funcName", arg1, arg2, arg3,...);

    // whereas this calls a function with a return value of tipe T
    T r=Python.PythonReturnFunction<T>("moduleName", "funcName", arg1, arg2,
   arg3,...);

    //
    ....

 */
// This class is intended to be initialized only ONCE! 
// So either use it as a global variable or turn it into a singleton class. 
// If the constructor detects that a Python interpreter is already active,
// it will throw an exception.

/*
How it works: when the compiler encounter a call to PythonFunction (or
PythonReturnFunction), the arguments after the first two are recursively sent to
a packing function which turns them into the corresponding  PyObject. Unless the type of the 
argument is  int, float, double, str (std::string or char[N]), bool or vector<T>, the argument of type T (class or struct) must provide 
a function PyObject* T::pack() that returns the data packaged for Python.  
Once the right packing function is called, the output is pushed into a vector. At the end of
the recursion, the vector contains the PyObjects corresponding to the argument
lists. These PyObjects are packed into an object that is sent to the Python function specified 
by the first two arguments. Objects of type int, float, double, str and bool appear to the 
receiving function as tuples (x,label) where x is the value and label is "int", "float",...
A vector<T> appears as a tuple (n, type, address, "vector"), where n is the the size, label is the type T, and 
the address is the address of the starting element. The file PyGlue.py defines a decorator PYGLUE that 
automatically turns the objects into the corresponding objects in Python (vectors appearing as numpys). 
For generic classes T, it is expected that the user provides T::pack() and defines a Python class with 
a constructor that accepts the incoming tuple which populates the data members. 
For classes that have an assignment operator, it is possible to reverse the process. In this case, the class must 
have a constructor T(PyObject *) that takes the PyObject containing the data from Python and returns an instance 
of the class. This requires that T provides  T& operator=(const T&). 
See the attached main.cpp for some examples of use. 
*/



/* little helper class that always return false at compile time.
Useful when there is the need to trigger a static_assert in the definition 
of a template function which needs to be triggered only if the template is 
actually instantiated. The Intel compiler does not seem to act on a static_assert
unless the template is initiated. GNU and CLang  parse it. 
Using static_assert<alwaysFalse<T>::value, "Error") prevents spurious stoppages 
since only when T is known at instantiation the argument can be evaluated by the compiler.
*/
template <typename T>
struct alwaysFalse:std::false_type {};


class Py {
 // data members 
 private:
 // stores the pointers of already loaded modules
  std::map<std::string, PyObject *> m_Module_map;
  // used to stash the pointers of the arguments to be passed to the Python
  // function
  std::vector<PyObject *> m_args;
  //public interface
 public:
  Py();   // Constructor
  ~Py();  // Destructor

  Py(const Py& ) = delete;
  Py & operator=(const Py) = delete;


  // accessor to inquire about existence of function
  bool isFuncDefined(std::string Module, std::string function);

  // the functions that actually do the work

  // basic templates for functions that do not require arguments
  inline void PythonFunction(std::string Module, std::string function) {
    runVoidFunction(Module, function, m_args);
  }

  template <class T>
  T PythonReturnFunction(std::string Module, std::string function);

  // templates definitions for functions that require arguments
  template <class... Ts>
  void PythonFunction(std::string Module, std::string function, Ts &&... ts);

  template <class T, class... Ts>
  T PythonReturnFunction(std::string Module, std::string function, Ts &&... ts); 

 
// these functions are used to pack and unpack simple types. They are declared 
// static so that they can be used to pack more elaborate classes. 
// if tagIt is true, the appropriate tagi is added at the end to be used 
// by the PyGlue decorator to infer type. 
static  PyObject *packDouble(double a_x,bool tagIt=true);
static  PyObject *packFloat(float a_x, bool tagIt=true);
static  PyObject *packInt(int a_i, bool tagIt=true);
static  PyObject *packBool(bool a_b, bool tagIt=true);
static  PyObject *packString(std::string s, bool tagIt=true);

static  int unpackInt(PyObject *a_pin);
static  float unpackFloat(PyObject *a_pin);
static  double unpackDouble(PyObject *a_pin);
static  std::string unpackString(PyObject *a_pin);
static  bool unpackBool(PyObject *a_pin);

// these are used for classes that have a vector-like structure (.size() and iterators)
template <class T>
static  PyObject *packCont(T &a_v); 
template <class T>
static  PyObject *packCont(const T &a_v); 

private:
  void runVoidFunction(std::string Module, std::string function,
                       std::vector<PyObject *> arg);
  PyObject *runFunction(std::string Module, std::string function,
                        std::vector<PyObject *> arg);

  enum {
    MULTIPLE_INSTANCES_DETECTED = 0,
    MODULE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    WRONG_NUMBER_OF_ARGUMENTS,
    VALARRAY_TYPE_NOT_SUPPORTED,
    CANNOT_PACK_TYPE,
    CANNOT_UNPACK_TYPE,
    CANNOT_START_INTERPRETER,
    INTERPRETER_CANNOT_BE_INITIALIZED
  };  // stores labels for error and warning messages

  // handlers for exceptions
  static void lintcatcher(int a_i);
  static void lintcatcher(int a_i, std::string toOutput);

  

  // overloaded function to create the Python pointer for their arguments

  // used to set a str of the correspoding type, to be used to specify what type
  // of numpy should be attached to the valarray
  template <class T>
  static std::string TypeOf(); 

  // The packing and unpacking functions that do the actual work for arrays and valarrays
  template <class T>
  static PyObject *makeView(T &a_v);

  template <class T>
  static PyObject *makeView(const T &a_v); 

  // basic packing and unpacking template for objects that are are passed by
  // reference
  template <class T>
  inline void unpack(T &t, PyObject *a_pin) {
    t = T(a_pin);  
  }

  template <class T>
  inline PyObject *pack(const T*t ){
   static_assert(alwaysFalse<T>::value, "You have reached the bottom of the recursion with a pointer which is not \
    a cont char *. If you know what you are doing, comment out the static_assert and recompile. \
    But be advised that this can be dangerous.");
    return t->pack();
    
  }
template <class T>
  inline PyObject *pack(T *t ){
    static_assert(alwaysFalse<T>::value, "You have reached the bottom of the recursion with a pointer which is not \
    a char *. If you know what you are doing, comment out the static_assert and recompile. \
    But be advised that this can be dangerous.");
    return t->pack();
    
  }
  
  template <class T>
  inline PyObject* pack(T&& t){
    return t.pack();
  }

// partial specializations for vector-like objects. 

  template <class T>
  inline PyObject *pack(std::vector<T> &a_v) {
    return this->packCont(a_v);
  }
  template <class T>
  inline PyObject *pack(const std::vector<T> &a_v) {
    return this->packCont(a_v);
  }
 template <class T>
 inline PyObject *pack(std::vector<T> &&a_v) {
    return this->packCont(std::move(a_v));
 }
  // bottom of recursion
  template <class T>
  void BuildArgsVector(T&& t); 
  // recursion definition
  template <class T, class... Args>
  void BuildArgsVector(T &&t, Args&&... ts);
  // sets or returns Module, whichever is needed
  PyObject *ImportAndGetModule(std::string ModuleName);


};


#include "PyGlue.hpp"

#endif  //!___PyGlue_H__INCLUDED___
