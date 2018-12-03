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

//#include "FArrayBox.H"
#include <Python.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <valarray>
// #include "State.H"
#define Real(double)

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

class MayDay // For testing purposes
{
public:
  static void Warning(std::string msg) { std::cout << msg << "\n"; };
  static void Error(std::string msg)
  {
    std::cout << msg << "\n";
    std::exit(0);
  };
};

class Py
{
private:
  enum
  {
    MULTIPLE_INSTANCES_DETECTED = 0,
    MODULE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    WRONG_NUMBER_OF_ARGUMENTS,
    CANNOT_PACK_TYPE,
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
  //PyObject* packFAB(FArrayBox & a_q);
  PyObject *packValarray(std::valarray<double> &a_q);
  PyObject *packReal(double a_x);
  PyObject *packInt(int a_i);
  PyObject *packBool(bool a_b);

  PyObject* runFunction(std::string Module, std::string function, std::vector<PyObject *> arg) { return static_cast<PyObject *>(nullptr); }; // Eventually, this is going to be used to run generic functions on generic modules
  void runVoidFunction(std::string Module, std::string function, std::vector<PyObject *> arg); // Eventually, this is going to be used to run generic functions on generic modules
  // basic packing functions for objects that are are passed by reference
  template <class T>
  PyObject *pack(T &t)
  {
    this->lintcatcher(CANNOT_PACK_TYPE, " "); //TODO: catch exception at compile time. 
    return static_cast<PyObject *>(nullptr);
  };

  // bottom of iteration
  template <class T>
  void BuildArgsVector(T &t)
  {
    PyObject *r = pack(t);
    m_args.push_back(r);
  };
  // iterators
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

  //
  bool isFuncDefined(std::string Module, std::string function);
  // the functions that actually do the work
  template <class... Ts>
  void PythonFunction(std::string Module, std::string function, Ts &... ts) 
  {
    this->BuildArgsVector(ts...);
    //runVoidFunction(Module, function, m_args);
    runVoidFunction(Module, function, m_args);
    m_args.clear();
  };
  template<class... Ts>
  void PythonFunction(PyObject* return_value,std::string Module, std::string function, Ts&... ts)
  {
    this -> BuildArgsVector(ts...);
    return_value=runFunction(Module, function, m_args);
    m_args.clear();
    
  }
private:
};

//specializations
//template < > inline PyObject* pack(FArrayBox & a_q){ return this->packFAB(a_q);}; // return an object that Python uses to alias a_q to a numpy
template <>
inline PyObject *Py::pack(std::valarray<double> &a_v) { return this->packValarray(a_v); };
// basic packing functions for objects that are passed by copy
template <>
inline PyObject *Py::pack(double &a_x)
{
  double x = a_x;
  return this->packReal(x);
}; // just to test.

template <>
inline PyObject *Py::pack(int &a_i)
{
  int i = a_i;
  return this->packInt(i);
}; // pack an int
template <>
inline PyObject *Py::pack(bool &a_b)
{
  bool b = a_b;
  return this->packBool(b);
};

  // In the above specializations, we make local copies to mimic the behavior of a function
  // that is called by value. Apparently c++11 does not like mixing and matching of
  // templates by reference and by value.

#endif //!___PyGlue_H__INCLUDED___
