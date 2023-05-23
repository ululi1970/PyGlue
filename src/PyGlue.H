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

#include "PyBase.H"
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>
#include <array>

// a couple of helper macros
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

// helper classes for type introspection
template <typename T>
struct alwaysFalse : std::false_type
{
};

template <typename T>
struct isNumeric
{
  static constexpr bool value = (std::is_same<T, typename std::remove_cv<int>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<float>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<double>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<std::size_t>::type>::value );
};



template<typename T, typename Q=void>
struct is_arrayLike : std::false_type{};

template<typename T>
struct is_vectorAndNumeric : std::false_type{};


template<typename T>
struct is_arrayLike<T, typename std::enable_if<std::is_base_of<std::array<typename T::value_type,  std::tuple_size<T>::value >, T>::value>::type> : std::true_type{};

template<typename T>
struct is_arrayLike<T, typename std::enable_if<std::is_base_of<std::vector<typename T::value_type>,                             T>::value>::type> : std::true_type{};

template<typename T>
struct is_vectorAndNumeric<std::vector<T>> : isNumeric<T>{};

class Py : public PyBase
{

  // public interface
public:
  Py()=delete;    // This is a static class
  virtual ~Py(){}; // ditto for destructor.

  Py(const Py &) = delete;
  Py &operator=(const Py) = delete;

  // these are the functions that the user calls to execute Module.function with
  // the arguments provided
  template <class... Ts>
  static void PythonFunction(std::string Module, std::string function, Ts &&...ts);

  template <class T, class... Ts>
  static T PythonReturnFunction(std::string Module, std::string function, Ts &&...ts);


  // used to set a str of the correspoding type, to be used to specify what type
  // of numpy should be attached to the memView
  template <class T>
  static std::string TypeOf();

  // Provides a view of numeric vectors.
  template <class T>
  static PyObject *makeView(T &a_v);

  template <class T>
  static PyObject *makeView(const T &a_v);

  
// unpacking function
  template <class T>
  static inline void unpack(T &t, PyObject *a_pin);
  

  
//packing functions
  template <class T>
  static inline PyObject *pack(const T *t)
  {
    static_assert(alwaysFalse<T>::value, "You have reached the bottom of the recursion with a pointer which is not \
    a cont char *. If you know what you are doing, comment out the static_assert and recompile. \
    But be advised that this can be dangerous.");
    return t->pack();
  }
  template <class T>
  static inline PyObject *pack(T *t)
  {
    static_assert(alwaysFalse<T>::value, "You have reached the bottom of the recursion with a pointer which is not \
    a char *. If you know what you are doing, comment out the static_assert and recompile. \
    But be advised that this can be dangerous.");
    return t->pack();
  }

  template <class T>
  static inline PyObject *pack(T &&t);
  

  

  

private:
  // bottom of recursion
  template <class T>
  static void BuildArgsVector(std::vector<PyObject *> &args, T &&t);
  // recursion definition
  template <class T, class... Args>
  static void BuildArgsVector(std::vector<PyObject *> &args, T &&t, Args &&...ts);
};

#include "PyGlue.hpp"

#endif //!___PyGlue_H__INCLUDED___
