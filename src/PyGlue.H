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
/* {
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


/*

See the attached main.cpp for some examples of use.
*/

// helper classes for type introspection
template <typename T>
struct alwaysFalse : std::false_type
{
};

template <typename T, typename Q = void>
struct isNumeric
{
  static constexpr bool value = (std::is_same<T, typename std::remove_cv<int>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<float>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<double>::type>::value ||
                                 std::is_same<T, typename std::remove_cv<std::size_t>::type>::value );
};

template<typename T>
struct isNumeric<T, typename std::enable_if<T::isConvertibleToNumpy>::type> : std::true_type{};

template<typename T, typename Q = void>
struct is_arrayLike : std::false_type{};

template<typename T>
struct is_arrayLike<T, typename std::enable_if<std::is_base_of<std::array<typename T::value_type,  std::tuple_size<T>::value >, T>::value>::type> : std::true_type{};

template<typename T>
struct is_arrayLike<T, typename std::enable_if<std::is_base_of<std::vector<typename T::value_type>,                             T>::value>::type> : std::true_type{};

// template<typename T>
// struct is_vectorAndNumeric : std::false_type{};
// template<typename T>
// struct is_vectorAndNumeric<std::vector<T>> : isNumeric<T>{};


template<typename T>
struct is_vectorAndNumeric 
{
   static constexpr bool value = (is_arrayLike<T>::value && isNumeric<typename T::value_type>::value);
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
  

  // used to set a str of the correspoding type, to be used to specify what type
  // of numpy should be attached to the memView
template <class T, typename = void>
struct TypeOf
{ 
  TypeOf() = delete;
  TypeOf(const TypeOf &) = delete;
  TypeOf &operator=(const TypeOf) = delete;
  static auto name(){
    return typeToStr().c_str();
  }

  private:
  static std::string typeToStr(){
  using size_t = std::size_t;
  const bool check_type_is_defined = SAME(int) || SAME(float) || SAME(double) || SAME(size_t);
  static_assert(check_type_is_defined,
                "Numeric arrays are only supported for int, float, double and size_t. "
                "Either your type is not supported, or it does not properly define isConvertibleToNumpy");
  TYPEtoSTR(int) TYPEtoSTR(double) TYPEtoSTR(float) TYPEtoSTR(size_t)

      
  return "";
}
};

template <class T>
struct TypeOf<T,typename std::enable_if<T::isConvertibleToNumpy>::type> 
{ 
  TypeOf() = delete;
  TypeOf(const TypeOf &) = delete;
  TypeOf &operator=(const TypeOf) = delete;
  static auto name(){
    return T::numpyName().c_str();
  }
  
};

  

private:
 
  // recursion definition
  template <class T, class... Args>
  static void BuildArgsVector(std::vector<PyObject *> &args, T &&t, Args &&...ts);
};

#include "PyGlue.hpp"

#endif //!___PyGlue_H__INCLUDED___
