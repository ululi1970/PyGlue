/*******************************************************************************
 *  PYGLUE
 *  Developed by Alberto Scotti
 *  Copyright (C) 2022
 *  University of North Carolina at Chapel Hill
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

#include <unistd.h>
#include <sstream>
#include <iostream>
#include "PyBase.H"

std::map<std::string, PyObject *> PyBase::m_Module_map;
const std::vector<std::string> PyBase::DefaultInitCommands{"import os",
                                                           "import sys",
                                                           "sys.argv=['']",
                                                           "import site",
                                                           "sys.path.append('.')",
                                                           "sys.path.insert(0, site.USER_SITE)"};

// imports moduleName into the interpreter
PyObject *PyBase::loadModule(std::string moduleName)
{
  PyObject *pName = PyUnicode_DecodeFSDefault(moduleName.c_str());
  // std::cerr << moduleName.c_str() << "\n";

  PyObject *r = PyImport_Import(pName);
  Py_DECREF(pName);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return r;
}

// returns a pointer to a Python tuple containing a real

PyObject *PyBase::packDouble(double a_x, bool tagIt)
{
  if (!tagIt)
    return PyFloat_FromDouble(double(a_x));
  PyObject *pReal = PyTuple_New(2);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x)));

  std::string Label = "double";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pReal, 1, pLabel);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}

PyObject *PyBase::packFloat(float a_x, bool tagIt)
{
  if (!tagIt)
    return PyFloat_FromDouble(double(a_x));
  PyObject *pReal = PyTuple_New(2);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x)));
  std::string Label = "double";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pReal, 1, pLabel);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}
PyObject *PyBase::packInt(int a_i, bool tagIt)
{
  if (!tagIt)
    return PyLong_FromLong(a_i);
  PyObject *pInt = PyTuple_New(2);
  PyTuple_SetItem(pInt, 0, PyLong_FromLong(a_i));

  std::string Label = "int";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pInt, 1, pLabel);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return pInt;
}

PyObject *PyBase::packBool(bool a_b, bool tagIt)
{
  if (!tagIt)
    return PyBool_FromLong(int(a_b));
  PyObject *pBool = PyTuple_New(2);
  PyTuple_SetItem(pBool, 0, PyBool_FromLong(int(a_b)));

  std::string Label = "bool";
  PyObject *pLabel = PyUnicode_FromString(Label.c_str());
  PyTuple_SetItem(pBool, 1, pLabel);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return pBool;
}

PyObject *PyBase::packString(std::string s, bool tagIt)
{
  if (!tagIt)
    return PyUnicode_FromString(s.c_str());
  PyObject *pArgs = PyTuple_New(2);
  PyObject *pStr = PyUnicode_FromString(s.c_str());
  PyTuple_SetItem(pArgs, 0, pStr);
  {
    std::string Label = "str";
    PyObject *pLabel = PyUnicode_FromString(Label.c_str());
    PyTuple_SetItem(pArgs, 1, pLabel);
  }
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return pArgs;
}

std::string PyBase::unpackString(PyObject *a_pin)
{

  const char *o = PyUnicode_AsUTF8(a_pin);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  std::string s(o);
  return s;
}

bool PyBase::unpackBool(PyObject *a_pin) { return (a_pin == Py_True); }

int PyBase::unpackInt(PyObject *a_pin)
{

  int x = static_cast<int>(PyLong_AsLong(a_pin));
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return x;
}

float PyBase::unpackFloat(PyObject *a_pin)
{
  float x = static_cast<float>(PyFloat_AsDouble(a_pin));
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return x;
}

double PyBase::unpackDouble(PyObject *a_pin)
{
  double x = PyFloat_AsDouble(a_pin);
  if (PyErr_Occurred())
  {
    PyErr_Print();
  }
  return x;
}

void PyBase::start(std::vector<std::string> initCommands)
{
  if (Py_IsInitialized())
    return;

  Py_Initialize();
  if (!Py_IsInitialized())
  {
    lintcatcher(CANNOT_START_INTERPRETER);
  }
  // run the commands provided in initCommands
  for (auto &i : initCommands)
  {
    PyRun_SimpleString(i.c_str());
  }

  if (PyErr_Occurred())
  {
    PyErr_Print();
    lintcatcher(Errors::INTERPRETER_CANNOT_BE_INITIALIZED);
  }
}

void PyBase::stop()
{
  Py_Finalize();
  m_Module_map.clear();
}

PyObject *PyBase::ImportAndGetModule(std::string ModuleName)
{
  if (m_Module_map.count(ModuleName) > 0)
  {
    return m_Module_map[ModuleName];
  }

  PyObject *pModule = loadModule(ModuleName);
  if (pModule != nullptr)
  {
    m_Module_map.insert(
        std::pair<std::string, PyObject *>(ModuleName, pModule));
    return pModule;
  }
  else
  {
    PyErr_Print();
    lintcatcher(MODULE_NOT_FOUND, ModuleName);

    return static_cast<PyObject *>(nullptr);
  }
}

bool PyBase::isFuncDefined(std::string Module, std::string function)
{
  PyObject *pFunc = PyObject_GetAttrString(ImportAndGetModule(Module),
                                           function.c_str());
  if (pFunc && PyCallable_Check(pFunc))
  {
    Py_DECREF(pFunc);
    return true;
  }
  else
  {
    Py_XDECREF(pFunc);
    return false;
  }
}

PyObject *PyBase::runFunction(std::string Module, std::string FuncName,
                              std::vector<PyObject *> Arg)
{
  if (!Py_IsInitialized())
    lintcatcher(Errors::INTERPRETER_NEEDS_TO_BE_INITIALIZED);
  PyObject *pFunc = PyObject_GetAttrString(ImportAndGetModule(Module),
                                           FuncName.c_str());
  if (pFunc && PyCallable_Check(pFunc))
  { // it's a go, function exists

    PyObject *pArgs = PyTuple_New(Arg.size());
    for (unsigned int i = 0; i < Arg.size(); ++i)
    {
      PyTuple_SetItem(pArgs, i, Arg[i]);
    }
    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred())
    {
      PyErr_Print();
      std::string o;
      o = Module + "." + FuncName;
      lintcatcher(Errors::WRONG_NUMBER_OF_ARGUMENTS, o);
    }
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    // Py_XDECREF(pValue);
    return pValue;
  }
  else
  {
    if (PyErr_Occurred())
      PyErr_Print();

    std::string o;
    o = Module + "." + FuncName;
    lintcatcher(Errors::FUNCTION_NOT_FOUND, o);

    return static_cast<PyObject *>(nullptr);
  }
}
void PyBase::runVoidFunction(std::string Module, std::string FuncName,
                             std::vector<PyObject *> Arg)
{
  if (Py_IsInitialized())
  {
    PyObject *r = runFunction(Module, FuncName, Arg);
    Py_XDECREF(r);
  }
  else
  {
    lintcatcher(Errors::INTERPRETER_NEEDS_TO_BE_INITIALIZED);
  }
}
void PyBase::lintcatcher(int a_i)
{
  switch (a_i)
  {
  case Errors::MULTIPLE_INSTANCES_DETECTED:
  {
    std::string o;
    o = "Multiple instances detected. Py is meant to be instantiated only "
        "once.";

    MayDay::Warning(o.c_str());
    o = "Redefine it as a global variable or wrap it with a singleton "
        "wrapper.";
    MayDay::Error(o.c_str());
    break;
  }
     
  case Errors::CANNOT_START_INTERPRETER:
    MayDay::Error("There was a problem starting the Python interpreter");
    break;
  case Errors::INTERPRETER_CANNOT_BE_INITIALIZED:
    MayDay::Error(
        "There was a problem with basinc initializion of the Python "
        "interpreter");
    break;
  case Errors::VALARRAY_TYPE_NOT_SUPPORTED:
    MayDay::Error("array<type> are only for type int, float or double");
    break;
  case Errors::TUPLE_HAS_WRONG_NUMBER_OF_ELEMENTS:
    MayDay::Error("Expected a different number of items in the tuple");
    break;
  case Errors::INTERPRETER_NEEDS_TO_BE_INITIALIZED:
    MayDay::Error("Python Script Invoked before running Py::start()");
    break;
  default:
    std::cout << a_i << "\n";
    MayDay::Error("Py:lintcatcher() caught an undefined error.");
    break;
  }
}
void PyBase::lintcatcher(int a_i, std::string name)
{
  switch (a_i)
  {
  case Errors::MODULE_NOT_FOUND:
  {
    std::string o;
    o = " Cannot find module " + name + ".";
    MayDay::Error(o.c_str());
    break;
  }
  case Errors::FUNCTION_NOT_FOUND:
  {
    std::string o;
    o = " Cannot find function " + name;
    MayDay::Error(o.c_str());
    break;
  }
  case Errors::WRONG_NUMBER_OF_ARGUMENTS:
  {
    std::string o;
    o = "The function " + name +
        " encountered and error: wrong number/type of arguments?";

    MayDay::Error(o.c_str());
    break;
  }
  case Errors::CANNOT_PACK_TYPE:
    MayDay::Error(" Cannot pack type");
    break;  
  case Errors::TUPLE_HAS_WRONG_NUMBER_OF_ELEMENTS:
    MayDay::Error("Expected a different number of items in the tuple");
    break;
  default:
    std::cout << a_i << "\n";
    MayDay::Warning("Py::lintcatcher() caught something wrong.");
    MayDay::Error(name.c_str());
    break;
  }
}
