/*******************************************************************************
 *  SOMAR - Stratified Ocean Model with Adaptive Refinement
 *  Developed by Ed Santilli & Alberto Scotti
 *  Copyright (C) 2018
 *    Jefferson (Philadelphia University + Thomas Jefferson University) and
 *    University of North Carolina at Chapel Hill
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
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <valarray>
#include <type_traits>
//#include "BCDescriptor.H"
//#include "MayDay.H"

//#include "AMRNSLevel.H"
#ifdef CHOMBO
#include "FArrayBox.H"
#endif 
// these macros are to be used on arithmetic and string types
// note that the chicanery with the pointer is ok
// since at the end of the day the whole expression 
// is evaluated only if X and T are the same type. 
#define IFTYPEU(X, Y)                      \
  {                                       \
    if (std::is_same<T, X>::value)        \
    { X s=this->Y(a_pin); \
     void* p=static_cast<void*>(&s);\
      T* pr=reinterpret_cast<T*>(p);         \
      t=*pr;   \
      return;                             \
    }                                     \
  }

#define IFTYPEP(X, Y)              \
  {                                \
    if (std::is_same<T, X>::value) \
    {                              \
                     \
      void * pr =static_cast<void *>(const_cast<T*>(&t)); \
      X* p=reinterpret_cast<X*>(pr);\
      return this->Y(*p);           \
    }                              \
  }
  #define TYPEtoSTR(X)               \
  {                                \
    if (std::is_same<T, X>::value) \
    {                              \
      return #X;                   \
    }                              \
  }


#define SAME(X) std::is_same<T, X>::value
// The purpose of this class is to be able to run functions on Python
// from within a C++ code. The use is
/* {Py Python;
    ....
    // this calls shows how to execute moduleName.funcName(arg1,arg2,arg3,...)
    // on the Python interpreter. The return value 
    // (Python functions have always a return value, even if not explicitely declared)
    // is discarded 
    Python.PythonFunction("moduleName", "funcName", arg1, arg2, arg3,...)

    // whereas this call stores the return value in the first argument
    Python.PythonFunction(return_value, "moduleName", "funcName", arg1, arg2, arg3,...)

    // in the latter case, it is responsibility of the caller to properly dispose of it 
    // via a call to Py_DECREF() or Py_XDECREF().  This function is currently set to always
    // return a nullptr. 
    ....
    
 */
// Attention shoppers: this class is intended to instantiate a global variable!
// If the constructor detects that a Python interpreter is already active,
// it will throw an exception

#include <array>

//dark magic used to check if a class as a .size() function. 
template<typename... Ts> struct make_void{typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;
// note the above is not needed if using c++14,  
template<typename T,  typename =  void> 
struct has_size_func: std::false_type{}; // default to false if third argument cannot work

template<typename T > 
struct has_size_func<T, void_t<decltype(std::declval<T&>().size())> >: std::true_type{}; 
// If the instantation of the Expression works, we set it to true, else we default to the false type

// So now we have a mechanism to detect Expression 

// now we work out the specific expression we need


  


#include "MayDay.H"

class Py
{
private:

//forward declarations
#ifdef CHOMBO
PyObject *packFAB(FArrayBox &a_q);
  PyObject *packFAB(const FArrayBox &a_q);
#endif   
  PyObject *packDouble(double a_x);
  PyObject *packFloat(float a_x);
  PyObject *packInt(int a_i);
  PyObject *packBool(bool a_b);
  PyObject *packString(std::string s);
  int unpackInt(PyObject *a_pin);
  float unpackFloat(PyObject *a_pin);
  double unpackDouble(PyObject *a_pin);
  std::string unpackString(PyObject *a_pin);
  bool unpackBool(PyObject *a_pin);

  void runVoidFunction(std::string Module, std::string function, std::vector<PyObject *> arg);
  PyObject *runFunction(std::string Module, std::string function, std::vector<PyObject *> arg);
  
  enum
  {
    MULTIPLE_INSTANCES_DETECTED = 0,
    MODULE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    WRONG_NUMBER_OF_ARGUMENTS,
    VALARRAY_TYPE_NOT_SUPPORTED,
    CANNOT_PACK_TYPE,
    CANNOT_UNPACK_TYPE,
    CANNOT_START_INTERPRETER,
    INTERPRETER_CANNOT_BE_INITIALIZED
  }; //stores labels for error and warning messages

  // handlers for exceptions
  void lintcatcher(int a_i);
  void lintcatcher(int a_i, std::string toOutput);

  // stores the pointers of already loaded modules
  std::map<std::string, PyObject *> m_Module_map;
  // used to stash the pointers of the arguments to be passed to the Python function
  std::vector<PyObject *> m_args;

  // overloaded function to create the Python pointer for their arguments

  //used to set a str of the correspoding type, to be used to specify what type
  // of numpy should be attached to the valarray
  template <class T>
  inline std::string TypeOf()
  {
    const bool check_type_is_defined = SAME(int) || SAME(float) || SAME(double);
    static_assert(check_type_is_defined, "Valarrays are only supported for int, float and double. Feel free to add to the list and post back.");
    TYPEtoSTR(int)
        TYPEtoSTR(double)
            TYPEtoSTR(float)

                this->lintcatcher(VALARRAY_TYPE_NOT_SUPPORTED);
    return "";
  }

  // The packing and unpacking functions that do the actual work!
  template <class U, class T>
  PyObject *makeView(U &a_v)
  {
    
    TypeOf<T>(); // will stop compilation if T is not in the list
    
    unsigned long long size = a_v.size() * sizeof(T);
    char *p = reinterpret_cast<char *>(&(a_v[0]));
    return PyMemoryView_FromMemory(p, size, PyBUF_WRITE);
  }
template <class U, class T>
  PyObject *makeView(const U &a_v)
  {
    
    TypeOf<T>(); // will stop compilation if T is not in the list
    
    unsigned long long size = a_v.size() * sizeof(T);
    char *p = reinterpret_cast<char *>(const_cast<T*>(&(a_v[0])));
   
      return PyMemoryView_FromMemory(p, size, PyBUF_READ);
  }
  
  
  
  // basic packing and unpacking template sfor objects that are are passed by reference
  template <class T>
  void unpack(T &t, PyObject *a_pin)
  {
    IFTYPEU(bool, unpackBool)

    IFTYPEU(int, unpackInt)

    IFTYPEU(double, unpackDouble)

    IFTYPEU(float, unpackFloat)

    IFTYPEU(std::string, unpackString)
    this->lintcatcher(CANNOT_UNPACK_TYPE, " unpack");
  }

  template <class T>
  PyObject *pack(T &t)
  {
    const bool check_type_is_defined = SAME(bool) || SAME(int) || SAME(float) || SAME(std::string) || SAME(double);
    static_assert(check_type_is_defined, " Only bool, int, float, double and std::string are supported, but feel free to add to the list and post back");
    T x = t;
    IFTYPEP(bool,packBool)
    IFTYPEP(int, packInt)
    IFTYPEP(double, packDouble)
    IFTYPEP(float, packFloat)
    IFTYPEP(std::string, packString)

    this->lintcatcher(CANNOT_PACK_TYPE, " pack"); //TODO: catch exception at compile time.
    return static_cast<PyObject *>(nullptr);
  };

  template <class T>
  PyObject *pack(const T &t)
  {
    const bool check_type_is_defined = SAME(bool) || SAME(int) || SAME(float)  || SAME(std::string) || SAME(double);
    static_assert(check_type_is_defined, " Only bool, int, float, double and std::string are supported, but feel free to add to the list and post back");
    
    IFTYPEP(bool,packBool)
    IFTYPEP(int, packInt)
    IFTYPEP(double, packDouble)
    IFTYPEP(float, packFloat)
    IFTYPEP(std::string,packString)

    this->lintcatcher(CANNOT_PACK_TYPE, " pack"); //TODO: catch exception at compile time.
    return static_cast<PyObject *>(nullptr);
  };


  template <class U, class T>
  PyObject* packCont( U & a_v){
    PyObject *pView = makeView<U,T>(a_v);
    PyObject *pSize = PyLong_FromLong(a_v.size());
    PyObject *pTypeOfT = PyUnicode_FromString(TypeOf<T>().c_str());
    PyObject *pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, pSize);
    PyTuple_SetItem(pArgs, 1, pTypeOfT);
    PyTuple_SetItem(pArgs, 2, pView);
    return pArgs;

  }

  template <class U, class T>
  PyObject* packCont( const U & a_v){
    PyObject *pView = makeView<U,T>(a_v);
    PyObject *pSize = PyLong_FromLong(a_v.size());
    PyObject *pTypeOfT = PyUnicode_FromString(TypeOf<T>().c_str());
    PyObject *pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, pSize);
    PyTuple_SetItem(pArgs, 1, pTypeOfT);
    PyTuple_SetItem(pArgs, 2, pView);
    return pArgs;

  }
  template <class T>
  inline PyObject *pack(std::valarray<T> &a_v) { return this->packCont<std::valarray<T>,T>(a_v); };
  template <class T>
  inline PyObject *pack(const std::valarray<T> &a_v) { return this->packCont<std::valarray<T>,T>(a_v); };
  template <class T>
  inline PyObject *pack(std::vector<T> &a_v) { return this->packCont<std::vector<T>,T>(a_v); };
  template <class T>
  inline PyObject *pack(const std::vector<T> &a_v) { return this->packCont<std::vector<T>,T>(a_v); };
  
  
  // bottom of recursion
  template <class T>
  void BuildArgsVector(T &t)
  {
    PyObject *r = pack(t);
    m_args.push_back(r);
  };
  // recursion definition
  template <class T, class... Args>
  void BuildArgsVector(T &t, Args &... args)
  {
    BuildArgsVector(t);
    BuildArgsVector(args...);
  };

  //sets or returns Module, whichever is needed
  PyObject *ImportAndGetModule(std::string ModuleName);

public:
  Py();  //Constructor
  ~Py(); //Destructor

  // accessor to inquire about existence of function
  bool isFuncDefined(std::string Module, std::string function);

  // the functions that actually do the work

  // basic templates for functions that do not require arguments
  void PythonFunction(std::string Module, std::string function)
  {
    runVoidFunction(Module, function, m_args);
  };

  template <class T>
  T PythonReturnFunction(std::string Module, std::string function)
  {
    PyObject *return_value = runFunction(Module, function, m_args);
    T t;
    unpack<T>(t, return_value);
    Py_DECREF(return_value);
    return t;
  };

  // templates definitions for functions that require arguments
  template <class... Ts>
  void PythonFunction(std::string Module, std::string function, Ts &... ts)
  {
    this->BuildArgsVector(ts...);
    //runVoidFunction(Module, function, m_args);
    runVoidFunction(Module, function, m_args);
    m_args.clear();
  };
  template <class T, class... Ts>
  T PythonReturnFunction(std::string Module, std::string function, Ts &... ts)
  {
    this->BuildArgsVector(ts...);
    PyObject *return_value = runFunction(Module, function, m_args);
    m_args.clear();
    T t;
    unpack<T>(t, return_value);
    Py_DECREF(return_value);
    return t;
  }

private:
};

//specializations
#ifdef CHOMBO
template <>
inline PyObject *Py::pack(FArrayBox &a_q) { return this->packFAB(a_q); }; // return an object that Python uses to alias a_q to a numpy
template <>
inline PyObject *Py::pack(const FArrayBox &a_q) { return this->packFAB(a_q); }; // return an object that Python uses to alias a_q to a numpy
#endif




  // In the above specializations, we make local copies to mimic the behavior of a function
  // that is called by value. Apparently c++11 does not like mixing and matching of
  // templates by reference and by value.

#endif //!___PyGlue_H__INCLUDED___
