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
#define CH_USE_PYTHON
#ifdef CH_USE_PYTHON

#include <Python.h>
#include <unistd.h>
#include <sstream>

#include "PyGlue.H"



#define addCompName(X) compNames[statePtr->X] = #X;

// imports moduleName into the interpreter
PyObject *loadModule(std::string moduleName) {
  PyObject *pName = PyUnicode_DecodeFSDefault(moduleName.c_str());
  // std::cerr << moduleName.c_str() << "\n";

  PyObject *r = PyImport_Import(pName);
  Py_DECREF(pName);
  return r;
}


// returns a pointer to a Python tuple containing a real

  PyObject *Py::packDouble(double a_x, bool tagIt) {
  if(!tagIt) return PyFloat_FromDouble(double(a_x));
  PyObject *pReal = PyTuple_New(2);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x)));
  
  std::string Label = "double";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pReal, 1, pLabel);
  
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}

PyObject *Py::packFloat(float a_x,bool tagIt) {
  if(!tagIt) return PyFloat_FromDouble(double(a_x));
  PyObject *pReal = PyTuple_New(2);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x)));
  std::string Label = "double";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pReal, 1, pLabel);
  
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}
PyObject *Py::packInt(int a_i, bool tagIt) {
  if(!tagIt) return PyLong_FromLong(a_i);
  PyObject *pInt = PyTuple_New(2);
  PyTuple_SetItem(pInt, 0, PyLong_FromLong(a_i));
  
  std::string Label = "int";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pInt, 1, pLabel);
  
  return pInt;
}

PyObject *Py::packBool(bool a_b, bool tagIt) {
  if(!tagIt) return PyBool_FromLong(int(a_b));
  PyObject *pBool = PyTuple_New(2);
  PyTuple_SetItem(pBool, 0, PyBool_FromLong(int(a_b)));
  
    std::string Label = "bool";
    PyObject *pLabel = PyUnicode_FromString(Label.c_str());
    PyTuple_SetItem(pBool, 1, pLabel);
  
  return pBool;
}

PyObject *Py::packString(std::string s, bool tagIt) {
  if(!tagIt) return PyUnicode_FromString(s.c_str());
  PyObject *pArgs = PyTuple_New(2);
  PyObject *pStr = PyUnicode_FromString(s.c_str());
  PyTuple_SetItem(pArgs, 0, pStr);
  {
  std::string Label = "str";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pArgs, 1, pLabel);
  }
  return pArgs;
}

std::string Py::unpackString(PyObject *a_pin) {
  const char *o = PyUnicode_AsUTF8(a_pin);

  std::string s(o);
  return s;
}

bool Py::unpackBool(PyObject *a_pin) { return (a_pin == Py_True); }

int Py::unpackInt(PyObject *a_pin){

int x = static_cast<int>(PyLong_AsLong(a_pin));
return x;
}


float Py::unpackFloat(PyObject *a_pin) {
  float x = static_cast<float>(PyFloat_AsDouble(a_pin));
  if (PyErr_Occurred()) {
    PyErr_Print();
  }
  return x;
}

double Py::unpackDouble(PyObject *a_pin) {
  double x = PyFloat_AsDouble(a_pin);
  if (PyErr_Occurred()) {
    PyErr_Print();
  }
  return x;
}



Py::Py() {
  if (Py_IsInitialized()) {
    this->lintcatcher(MULTIPLE_INSTANCES_DETECTED);
  }
  Py_Initialize();
  if (!Py_IsInitialized()) {
    this->lintcatcher(CANNOT_START_INTERPRETER);
  }
  // the purpose of this block is to 
  // configure the python interpreter
  
  PyRun_SimpleString("import os");
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.argv=['']");
  PyRun_SimpleString("import site");
  PyRun_SimpleString("sys.path.append('.')");
  PyRun_SimpleString("sys.path.append('../PythonScripts')");
  PyRun_SimpleString("sys.path.insert(0, site.USER_SITE)");

  if (PyErr_Occurred())
  {
    PyErr_Print();
    this->lintcatcher(INTERPRETER_CANNOT_BE_INITIALIZED);
  }
}

Py::~Py() {
  for (auto itr = m_Module_map.begin(); itr != m_Module_map.end(); ++itr) {
    Py_DECREF(itr->second);
  }
  m_Module_map.clear();
  Py_Finalize();
}

PyObject *Py::ImportAndGetModule(std::string ModuleName) {
  if (m_Module_map.count(ModuleName) > 0) {
    return m_Module_map[ModuleName];
  }

  PyObject *pModule = loadModule(ModuleName);
  if (pModule != nullptr) {
    m_Module_map.insert(
        std::pair<std::string, PyObject *>(ModuleName, pModule));
    return pModule;
  } else {
    PyErr_Print();
    this->lintcatcher(MODULE_NOT_FOUND, ModuleName);

    Py_RETURN_NONE;
  }
}

bool Py::isFuncDefined(std::string Module, std::string function) {
  PyObject *pFunc = PyObject_GetAttrString(this->ImportAndGetModule(Module),
                                           function.c_str());
  if (pFunc && PyCallable_Check(pFunc)) {
    Py_DECREF(pFunc);
    return true;
  } else {
    Py_XDECREF(pFunc);
    return false;
  }
}

PyObject *Py::runFunction(std::string Module, std::string FuncName,
                          std::vector<PyObject *> Arg) {
  PyObject *pFunc = PyObject_GetAttrString(this->ImportAndGetModule(Module),
                                           FuncName.c_str());
  if (pFunc && PyCallable_Check(pFunc)) {  // it's a go, function exists

    PyObject *pArgs = PyTuple_New(Arg.size());
    for (unsigned int i = 0; i < Arg.size(); ++i) {
      PyTuple_SetItem(pArgs, i, Arg[i]);
    }
    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred()) {
      PyErr_Print();
      std::string o;
      o = Module + "." + FuncName;
      this->lintcatcher(WRONG_NUMBER_OF_ARGUMENTS, o);
    }
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    // Py_XDECREF(pValue);
    return pValue;
  } else {
    if (PyErr_Occurred()) PyErr_Print();
    std::string o;
    o = Module + "." + FuncName;
    this->lintcatcher(FUNCTION_NOT_FOUND, o);
    return static_cast<PyObject *>(nullptr);
  }
}
void Py::runVoidFunction(std::string Module, std::string FuncName,
                         std::vector<PyObject *> Arg) {
  PyObject *r = this->runFunction(Module, FuncName, Arg);
  Py_XDECREF(r);
}
void Py::lintcatcher(int a_i) {
  switch (a_i) {
    case MULTIPLE_INSTANCES_DETECTED: {
      std::string o;
      o = "Multiple instances detected. Py is meant to be instantiated only "
          "once.";

      MayDay::Warning(o.c_str());
      o = "Redefine it as a global variable or wrap it with a singleton "
          "wrapper.";
      MayDay::Error(o.c_str());
    }
    case CANNOT_START_INTERPRETER:
      MayDay::Error("There was a problem starting the Python interpreter");
    case INTERPRETER_CANNOT_BE_INITIALIZED:
      MayDay::Error(
          "There was a problem with basinc initializion of the Python "
          "interpreter");
    case VALARRAY_TYPE_NOT_SUPPORTED:
      MayDay::Error("valarray<type> are only for type int, float or double");
    default:
      std::cout << a_i << "\n";
      MayDay::Error("Py:lintcatcher() caught an undefined error.");
  }
}
void Py::lintcatcher(int a_i, std::string name) {
  switch (a_i) {
    case MODULE_NOT_FOUND: {
      std::string o;
      o = " Cannot find module " + name + ".";
      MayDay::Error(o.c_str());
    }
    case FUNCTION_NOT_FOUND: {
      std::string o;
      o = " Cannot find function " + name;
      MayDay::Error(o.c_str());
    }
    case WRONG_NUMBER_OF_ARGUMENTS: {
      std::string o;
      o = "The function " + name +
          " encountered and error: wrong number/type of arguments?";
      std::exit(0);
      MayDay::Error(o.c_str());
    }
    case CANNOT_PACK_TYPE:
      MayDay::Error(" Cannot pack type");
    default:
      std::cout << a_i << "\n";
      MayDay::Warning("Py::lintcatcher() caught something wrong.");
      MayDay::Error(name.c_str());
  }
}

#endif
