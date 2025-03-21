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
 *  https://github.com/ululi1970.
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
/**
 * @class Py
 * @brief A static utility class for interfacing with Python functions and handling Python objects.
 * 
 * This class provides a set of static methods to call Python functions, handle Python objects,
 * and manage conversions between C++ and Python types. It is designed to be non-instantiable
 * and non-copyable.
 * 
 * @note This class is static and cannot be instantiated or copied.
 */

/**
 * @fn Py::PythonFunction
 * @brief Calls a Python function with the specified module, function name, and arguments.
 * 
 * @tparam Ts Variadic template for argument types.
 * @param Module The name of the Python module.
 * @param function The name of the Python function to call.
 * @param ts The arguments to pass to the Python function.
 */

/**
 * @fn Py::PythonReturnFunction
 * @brief Calls a Python function with the specified module, function name, and arguments, and returns a value.
 * 
 * @tparam T The return type of the Python function.
 * @tparam Ts Variadic template for argument types.
 * @param Module The name of the Python module.
 * @param function The name of the Python function to call.
 * @param ts The arguments to pass to the Python function.
 * @return The value returned by the Python function.
 */

/**
 * @fn Py::makeView
 * @brief Creates a Python object view of a numeric vector.
 * 
 * @tparam T The type of the numeric vector.
 * @param a_v The numeric vector to create a view for.
 * @return A PyObject pointer representing the view.
 */

/**
 * @fn Py::unpack
 * @brief Unpacks a Python object into a C++ variable.
 * 
 * @tparam T The type of the C++ variable.
 * @param t The C++ variable to unpack into.
 * @param a_pin The Python object to unpack.
 */

/**
 * @fn Py::pack
 * @brief Packs a C++ object into a Python object.
 * 
 * @tparam T The type of the C++ object.
 * @param t The C++ object to pack.
 * @return A PyObject pointer representing the packed object.
 * 
 * @note This function contains static assertions to prevent unsafe usage with unsupported types.
 */

/**
 * @struct Py::TypeOf
 * @brief Provides type information for numeric arrays to specify the corresponding numpy type.
 * 
 * This struct is specialized for supported numeric types (int, float, double, size_t) and
 * for types that define `isConvertibleToNumpy`.
 * 
 * @tparam T The type to provide information for.
 * @tparam void SFINAE parameter for specialization.
 * 
 * @note Only numeric arrays of supported types are allowed. Unsupported types will trigger
 *       a static assertion.
 */

/**
 * @fn Py::BuildArgsVector
 * @brief Recursively builds a vector of Python objects from C++ arguments.
 * 
 * @tparam T The type of the current argument.
 * @tparam Args Variadic template for the remaining argument types.
 * @param args The vector to populate with Python objects.
 * @param t The current argument to process.
 * @param ts The remaining arguments to process.
 */
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
  using std::size_t;
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
