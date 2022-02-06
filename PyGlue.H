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

/* little helper class that always return false at compile time.
Useful when there is the need to trigger a static_assert in the definition
of a template function which needs to be triggered only if the template is
actually instantiated. The Intel compiler does not seem to act on a static_assert
unless the template is initiated. GNU and CLang  parse it.
Using static_assert<alwaysFalse<T>::value, "Error") prevents spurious stoppages
since only when T is known at instantiation the argument can be evaluated by the compiler.
*/
template <typename T>
struct alwaysFalse : std::false_type
{
};

template <typename T>
struct isNumeric
{
  static constexpr bool value = (std::is_same<T, int>::value ||
                                 std::is_same<T, float>::value ||
                                 std::is_same<T, double>::value);
};

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

  // these are specializations used for classes that have a vector-like structure (.size() and iterators)
  template <class T>
  static PyObject *packCont(T &a_v);
  template <class T>
  static PyObject *packCont(const T &a_v);

  // used to set a str of the correspoding type, to be used to specify what type
  // of numpy should be attached to the memView
  template <class T>
  static std::string TypeOf();

  // Provides a view of numeric vectors.
  template <class T>
  static PyObject *makeView(T &a_v);

  template <class T>
  static PyObject *makeView(const T &a_v);

  // basic unpacking template for objects that provide T(PyObject*) and PyObject* T::pack()

  template <class T>
  static inline void unpack(T &t, PyObject *a_pin)
  {
    t = T(a_pin);
  }

  // sepcialization for unpacking arrays
  template <class T, size_t N>
  static inline void unpack(std::array<T, N> &t, PyObject *a_pin)
  {
    if (PyTuple_Size(a_pin) != N)
      Py::lintcatcher(TUPLE_HAS_WRONG_NUMBER_OF_ELEMENTS);
    for (int i = 0; i < N; ++i)
      Py::unpack(t[i], PyTuple_GetItem(a_pin, i));
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
  }

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
  static inline PyObject *pack(T &&t)
  {
    return t.pack();
  }

  // partial specializations for vector-like objects.
  // Note the different treatment for vectors. If numeric type,
  // it is packaged to be aliased by a Numpy.
  // Otherwise, it is meant to appear as a tuple of (Ts)
  // arrays are always appear as tuple of Ts.

  template <class T, size_t N>
  static inline PyObject *pack(std::array<T, N> &a_v)
  {
    PyObject *pArg = PyTuple_New(N + 1);
    int pos = 0;
    for (auto &item : a_v)
    {

      PyTuple_SetItem(pArg, pos++, Py::pack(item));
    }
    PyTuple_SetItem(pArg, pos, PyUnicode_FromString(std::string("tuple").c_str()));
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

  template <class T, size_t N>
  static inline PyObject *pack(const std::array<T, N> &a_v)
  {
    PyObject *pArg = PyTuple_New(N + 1);
    int pos = 0;
    for (auto &item : a_v)
    {
      PyTuple_SetItem(pArg, pos++, Py::pack(item));
    }
    PyTuple_SetItem(pArg, pos, PyUnicode_FromString(std::string("tuple").c_str()));
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

  template <class T, size_t N>
  static inline PyObject *pack(std::array<T, N> &&a_v)
  {
    PyObject *pArg = PyTuple_New(N + 1);
    int pos = 0;
    for (auto &item : a_v)
    {
      PyTuple_SetItem(pArg, pos++, Py::pack(item));
    }
    std::string label = "tuple";
    PyTuple_SetItem(pArg, pos, PyUnicode_FromString(label.c_str()));
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

  template <class T>
  static inline PyObject *pack(std::vector<T> &a_v)
  {
    PyObject *pArg = nullptr;
    if constexpr (isNumeric<T>::value)
    {
      pArg = Py::packCont(a_v);
    }
    else
    {
      int size = a_v.size();
      pArg = PyTuple_New(size + 1);
      int pos = 0;
      for (auto &item : a_v)
      {
        PyTuple_SetItem(pArg, pos++, item.pack());
      }
      std::string label = "tuple";
      PyTuple_SetItem(pArg, pos, PyUnicode_FromString(label.c_str()));
    }
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

  template <class T>
  static inline PyObject *pack(const std::vector<T> &a_v)
  {
    PyObject *pArg = nullptr;
    if constexpr (isNumeric<T>::value)
    {
      pArg = Py::packCont(a_v);
    }
    else
    {
      int size = a_v.size();
      pArg = PyTuple_New(size + 1);
      int pos = 0;
      for (auto &item : a_v)
      {
        PyTuple_SetItem(pArg, pos++, item.pack());
      }
      std::string label = "tuple";
      PyTuple_SetItem(pArg, pos, PyUnicode_FromString(label.c_str()));
    }
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

  template <class T>
  static inline PyObject *pack(std::vector<T> &&a_v)
  {
    PyObject *pArg = nullptr;
    if constexpr (isNumeric<T>::value)
    {
      pArg = Py::packCont(std::move(a_v));
    }
    else
    {
      int size = a_v.size();
      pArg = PyTuple_New(size + 1);
      int pos = 0;
      for (auto &item : a_v)
      {
        PyTuple_SetItem(pArg, pos++, item.pack());
      }
      std::string label = "tuple";
      PyTuple_SetItem(pArg, pos, PyUnicode_FromString(label.c_str()));
    }
    if (PyErr_Occurred())
    {
      PyErr_Print();
    }
    return pArg;
  }

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
