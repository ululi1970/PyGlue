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
#include<vector>
#include<map>
#include<valarray>
// #include "State.H"
#define Real(double)
// Attention shoppers: this is intended to be used as a global variable!
// If the constructor detects that a Python interpreter is already active,
// it will throw an exception

class MayDay // For testing purposes
{public:  
static void Warning(std::string msg){ std::cout << msg << "\n";};
static void Error(std::string msg){
  std::cout << msg  << "\n"; 
                std::exit(0);
                };

};

class Py
{private:
   enum { MULTIPLE_INSTANCES_DETECTED=0, 
   MODULE_NOT_FOUND, 
   FUNCTION_NOT_FOUND, 
   WRONG_NUMBER_OF_ARGUMENTS,
   CANNOT_PACK_TYPE,
   CANNOT_START_INTERPRETER,
   INTERPRETER_CANNOT_BE_INITIALIZED
   };
   void lintcatcher(int a_i);
   void lintcatcher(int a_i, std::string toOutput);

  std::map<std::string,PyObject*> m_Module_map;
  std::vector<PyObject*> m_args;

//PyObject* packFAB(FArrayBox & a_q);
PyObject* packValarray(std::valarray<double>& a_q);
PyObject* packReal(double a_x);
PyObject* packInt(int a_i);
PyObject* packBool(bool  a_b);
//PyObject* runFunction(std::string Module, std::string function, vector<PyObject *> arg); // Eventually, this is going to be used to run generic functions on generic modules
  void runVoidFunction(std::string Module, std::string function, std::vector<PyObject *> arg); // Eventually, this is going to be used to run generic functions on generic modules
// basic packing functions for objects that are are passed by reference
template <class T> PyObject* pack(T&t ){
  this->lintcatcher(CANNOT_PACK_TYPE, " ");
  return static_cast<PyObject *>(nullptr);}; 
 
/*  template <class T > PyObject* pack(T t) {
    this->lintcatcher(CANNOT_PACK_TYPE);
     static_cast<PyObject *>(nullptr);}; // DEfault behavior 
 */ 
// the variadic function to pack the input args.
// basic functions
/*template <class T> void BuildArgsVector(T t){
  PyObject* r=pack(t);
  m_args.push_back(r);
};*/
  template <class T> void BuildArgsVector(T& t){
    PyObject* r=pack(t);
    m_args.push_back(r);
  };
// iterators  
  template <class T,class... Args> void BuildArgsVector(T& t,Args... args){ 
    BuildArgsVector(t);
    BuildArgsVector(args...);
    };
  /*template <class T,class... Args> void BuildArgsVector(T t,Args... args){
    BuildArgsVector(t);
    BuildArgsVector(args...);
  };*/
// final product  
  //template <class... Ts> void BuildArgsVector(Ts... ts);
  PyObject *ImportAndGetModule(std::string ModuleName); //sets and returns Module
public: 
   Py(); //Constructor 
  ~Py(); //Destructor

  //
  
  //void initializePySomar(PyObject *pModule, std::string initFunctionName, State * statePtr); // used to initialize the main module.
  //void Init()
  template <class... Ts> void PythonFunction(std::string Module, std::string function,Ts... ts)//,
  //std::string Module, std::string function)
  {
    this->BuildArgsVector(ts...); 
    //runVoidFunction(Module, function, m_args);
    runVoidFunction(Module, function,m_args);
                    m_args.clear();
                    };
private:

};


//specialization. 
   //template < > inline PyObject* pack(FArrayBox & a_q){ return this->packFAB(a_q);}; // return an object that Python uses to alias a_q to a numpy
  template < > inline PyObject* Py::pack(std::valarray<double>& a_v){return this->packValarray(a_v);};
// basic packing functions for objects that are passed by copy
  template < > inline PyObject* Py::pack(double& a_x) {double x=a_x; return this->packReal(x);}; // just to test.

  template < > inline PyObject* Py::pack(int& a_i) {int i=a_i; return this->packInt(i);}; // pack an int
  template < > inline PyObject* Py::pack(bool& a_b){ bool b=a_b; return this->packBool(b);};

// In the above specializations, we make local copies to mimic the behavior of a function 
// that is called by value. Apparently c++11 does not like mixing and matching of
// templates by reference and by value. 



#endif //!___PyGlue_H__INCLUDED___
