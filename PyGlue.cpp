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


#include <Python.h>
#include <unistd.h>
#include <sstream>

#include "PyGlue.H"
#include "FArrayBox.H"

#define addCompName(X) compNames[statePtr->X] = #X;

// imports moduleName into the interpreter
PyObject *loadModule(std::string moduleName)
{

  PyObject *pName = PyUnicode_DecodeFSDefault(moduleName.c_str());
  //std::cerr << moduleName.c_str() << "\n";

  PyObject *r = PyImport_Import(pName);
  Py_DECREF(pName);
  return r;
}

// returns a pointer to a Python object that stores the memory address of the
// buffer of the FAB a_in.
// TODO: Not a very elegant solution: there should be a single template for makeview, 
// but then, there are only a few lines, and the template would probably take as much. 
PyObject *makeViewFAB(FArrayBox &a_in)
{

  unsigned long long size = a_in.size(a_in.box(), a_in.interval());

  PyObject *pView = PyMemoryView_FromMemory((char *)a_in.dataPtr(), size, PyBUF_WRITE);
  return pView;
}

PyObject *makeViewFAB(const FArrayBox &a_in)
{

  unsigned long long size = a_in.size(a_in.box(), a_in.interval());

  PyObject *pView = PyMemoryView_FromMemory((char *)a_in.dataPtr(), size, PyBUF_READ);
  return pView;
}

// returns a pointer to a Python tuple containing a real

PyObject *Py::packDouble(double a_x)
{
  PyObject *pReal = PyTuple_New(1);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x)));
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}

PyObject *Py::packFloat(float a_x)
{
  PyObject *pReal = PyTuple_New(1);
  PyTuple_SetItem(pReal, 0, PyFloat_FromDouble(double(a_x))); 
  return pReal;
  // pReal owns the object returned by PyFloat, so no need to decref that
}
PyObject *Py::packInt(int a_i)
{
  PyObject *pInt = PyTuple_New(1);
  PyTuple_SetItem(pInt, 0, PyLong_FromLong(a_i));
  return pInt;
}

PyObject *Py::packBool(bool a_b)
{
  PyObject *pBool = PyTuple_New(1);
  PyTuple_SetItem(pBool, 0, PyBool_FromLong(int(a_b)));
  return pBool;
}

PyObject *Py::packString(std::string s)
{
  PyObject* pStr=PyUnicode_FromString(s.c_str());
  PyObject *pArgs = PyTuple_New(1);
  PyTuple_SetItem(pArgs, 0, pStr);
  return pArgs;
}

std::string Py::unpackString(PyObject* a_pin)
{ char * o = PyUnicode_AsUTF8(a_pin);
  std::cout << o << std::endl;
  std::string s(o);
  return s;
}

bool Py::unpackBool(PyObject* a_pin)
{
  return (a_pin == Py_True);
}

int Py::unpackInt(PyObject * a_pin)
{
  int i =static_cast<int>(PyLong_AsLong(a_pin));
  if(PyErr_Occurred()){PyErr_Print();}
  return i;
}

float Py::unpackFloat(PyObject * a_pin)
{
  float x= static_cast<float>(PyFloat_AsDouble(a_pin));
   if(PyErr_Occurred()){PyErr_Print();}
  return x;
}

double Py::unpackDouble(PyObject * a_pin)
{
  double x= PyFloat_AsDouble(a_pin);
   if(PyErr_Occurred()){PyErr_Print();}
  return x;
}


PyObject *Py::packFAB(const FArrayBox &a_in)
{

  PyObject *pView = makeViewFAB(a_in); // get the memory address
  
  PyObject *pArgs = PyTuple_New(5);

  PyObject *pLo = PyTuple_New(SpaceDim);
  PyObject *pSize = PyTuple_New(SpaceDim);
  PyObject *pCentering = PyTuple_New(SpaceDim);
  PyObject *nComp = PyTuple_New(1);

  IndexType ind = a_in.box().ixType();

  for (auto i = 0; i < SpaceDim; ++i)
  {

    PyTuple_SetItem(pLo, i, PyLong_FromLong(a_in.box().smallEnd()[i]));
    PyTuple_SetItem(pSize, i, PyLong_FromLong(a_in.size()[i]));
    PyTuple_SetItem(pCentering, i, PyLong_FromLong(ind.test(i)));
  }
  PyTuple_SetItem(nComp, 0, PyLong_FromLong(a_in.nComp()));

  PyTuple_SetItem(pArgs, 0, pLo);
  PyTuple_SetItem(pArgs, 1, pSize);
  PyTuple_SetItem(pArgs, 2, pCentering);
  PyTuple_SetItem(pArgs, 3, nComp);
  PyTuple_SetItem(pArgs, 4, pView);
  return pArgs;
  //again here we do not decref the items stored in pArgs, since the latter owns them now.
};

PyObject *Py::packFAB(FArrayBox &a_in)
{

  PyObject *pView = makeViewFAB(a_in); // get the memory address
  
  PyObject *pArgs = PyTuple_New(5);

  PyObject *pLo = PyTuple_New(SpaceDim);
  PyObject *pSize = PyTuple_New(SpaceDim);
  PyObject *pCentering = PyTuple_New(SpaceDim);
  PyObject *nComp = PyTuple_New(1);

  IndexType ind = a_in.box().ixType();

  for (auto i = 0; i < SpaceDim; ++i)
  {

    PyTuple_SetItem(pLo, i, PyLong_FromLong(a_in.box().smallEnd()[i]));
    PyTuple_SetItem(pSize, i, PyLong_FromLong(a_in.size()[i]));
    PyTuple_SetItem(pCentering, i, PyLong_FromLong(ind.test(i)));
  }
  PyTuple_SetItem(nComp, 0, PyLong_FromLong(a_in.nComp()));

  PyTuple_SetItem(pArgs, 0, pLo);
  PyTuple_SetItem(pArgs, 1, pSize);
  PyTuple_SetItem(pArgs, 2, pCentering);
  PyTuple_SetItem(pArgs, 3, nComp);
  PyTuple_SetItem(pArgs, 4, pView);
  return pArgs;
  //again here we do not decref the items stored in pArgs, since the latter owns them now.
};

/* Initializes python, passing in component names and positions as well as parmParse parameters
 * moduleName: name of the python file minus the ".py" extension
 * initFunctionName: name of the initialization function in python
 * Returns a PyObject pointer to the module which will be passed in to other functions
 */

/*void Py::initializePySomar(PyObject *pModule, std::string initFunctionName, State *statePtr)
{

  // Puts component names in an array to be pulled to Python
  std::string compNames[statePtr->numComps];
  for (int i = 0; i < statePtr->numScalars; i++)
  {
    compNames[i] = "XXX"; // this is a kludge for the moment. getScalarName(i);
  }
  addCompName(nuTComp);
  addCompName(kappaTComp);
  addCompName(uComp);
  addCompName(vComp);
  addCompName(wComp);
  addCompName(bComp);
  addCompName(pComp);
  addCompName(phiComp);

  PyObject *pName, *pInit, *pArgs, *pValue, *pInFile, *pStateNames;

  pInit = PyObject_GetAttrString(pModule, initFunctionName.c_str());
  if (pInit && PyCallable_Check(pInit))
  {
    std::ostringstream o;
    ParmParse pp = ParmParse();
    pp.dumpTable(o);
    std::string ppString = o.str();
    pInFile = PyUnicode_FromString(ppString.c_str());
    pStateNames = PyTuple_New(statePtr->numComps);
    for (int i = 0; i < statePtr->numComps; i++)
    {
      PyTuple_SetItem(pStateNames, i, PyUnicode_FromString(compNames[i].c_str()));
    }
    // Initialize Arguments tuple
    pArgs = PyTuple_Pack(2, pInFile, pStateNames);
    pValue = PyObject_CallObject(pInit, pArgs);
    Py_DECREF(pArgs);
    Py_DECREF(pInFile);
    Py_DECREF(pStateNames);
    // here we decref all of them because PyTuple_Pack does not "Steal" the references.
    // This is different behavior from PyTuple_SetItem!
    if (pValue != nullptr)
    {
      Py_DECREF(pValue);
    }
    else
    {
      Py_DECREF(pInit);
      Py_DECREF(pModule);
      PyErr_Print();
      std::string o;
      o = "Py::initializePySomar(" + initFunctionName + "). Something went wrong ";
      MayDay::Error(o.c_str());
    }
  }
  else
  {
    if (PyErr_Occurred())
      PyErr_Print();
    std::string o;
      o = "Py::initializePySomar(" + initFunctionName + "). Cannot find file";
      MayDay::Error(o.c_str());
  }
  Py_XDECREF(pInit);
}
*/
Py::Py()
{
  if (Py_IsInitialized())
  {
    this->lintcatcher(MULTIPLE_INSTANCES_DETECTED);
  }
  Py_Initialize();
  if (!Py_IsInitialized())
  {
    this->lintcatcher(CANNOT_START_INTERPRETER);
  }
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('.')");
  if (PyErr_Occurred())
  {
    PyErr_Print();
    this->lintcatcher(INTERPRETER_CANNOT_BE_INITIALIZED);
  }
}

Py::~Py()
{

  for (auto itr = m_Module_map.begin(); itr != m_Module_map.end(); ++itr)
  {
    Py_DECREF(itr->second);
  }
  m_Module_map.clear();
  Py_Finalize();
}

PyObject *Py::ImportAndGetModule(std::string ModuleName)
{
  if (m_Module_map.count(ModuleName) > 0)
  {
    return m_Module_map[ModuleName];
  }

  PyObject *pModule = loadModule(ModuleName);
  if (pModule != nullptr)
  {
    m_Module_map.insert(std::pair<std::string, PyObject *>(ModuleName, pModule));
    return pModule;
  }
  else
  {
    PyErr_Print();
    this->lintcatcher(MODULE_NOT_FOUND, ModuleName);

    Py_RETURN_NONE;
  }
}

bool Py::isFuncDefined(std::string Module, std::string function)
{
  
  PyObject *pFunc = PyObject_GetAttrString(this->ImportAndGetModule(Module), function.c_str());
  if(pFunc && PyCallable_Check(pFunc))
  {
    Py_DECREF(pFunc);
    return true;
  }else{
    Py_XDECREF(pFunc);
    return false;
  }
}


PyObject* Py::runFunction(std::string Module, std::string FuncName, std::vector<PyObject *> Arg)
{
  PyObject *pFunc = PyObject_GetAttrString(this->ImportAndGetModule(Module), FuncName.c_str());
  if (pFunc && PyCallable_Check(pFunc))
  { // it's a go, function exists

    PyObject *pArgs = PyTuple_New(Arg.size());
    for (auto i = 0; i < Arg.size(); ++i)
    {
      PyTuple_SetItem(pArgs, i, Arg[i]);
    }
    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    if (PyErr_Occurred())
    {
      PyErr_Print();
      std::string o;
      o = Module + "." + FuncName;
      this->lintcatcher(WRONG_NUMBER_OF_ARGUMENTS, o);
    }
    Py_DECREF(pArgs);
    Py_DECREF(pFunc);
    //Py_XDECREF(pValue);
    return pValue;
  }
  else
  {
    if (PyErr_Occurred())
      PyErr_Print();
    std::string o;
    o = Module + "." + FuncName;
    this->lintcatcher(FUNCTION_NOT_FOUND, o);
    return static_cast<PyObject*>(nullptr);
  }
}
void Py::runVoidFunction(std::string Module, std::string FuncName, std::vector<PyObject *> Arg)
{
  PyObject* r=this->runFunction(Module, FuncName, Arg);
  Py_XDECREF(r);
}
void Py::lintcatcher(int a_i)
{
  switch (a_i)
  {
  case MULTIPLE_INSTANCES_DETECTED:
  {
  std::string o;
  o = "Multiple instances detected. Py is meant to be instantiated only once.";

  MayDay::Warning(o.c_str());
  o = "Redefine it as a global variable or wrap it with a singleton wrapper.";
  MayDay::Error(o.c_str());
  }
  case CANNOT_START_INTERPRETER:
    MayDay::Error("There was a problem starting the Python interpreter");
  case INTERPRETER_CANNOT_BE_INITIALIZED:
    MayDay::Error("There was a problem with basinc initializion of the Python interpreter");
  case VALARRAY_TYPE_NOT_SUPPORTED:
    MayDay::Error("valarray<type> are only for type int, float or double");
  default:
    std::cout << a_i << "\n";
    MayDay::Error("Py:lintcatcher() caught an undefined error.");
  }
}
void Py::lintcatcher(int a_i, std::string name)
{
  switch (a_i)
  {
  case MODULE_NOT_FOUND:
  {
    std::string o;
    o = " Cannot find module " + name + ".";
    MayDay::Error(o.c_str());
  }
  case FUNCTION_NOT_FOUND:
  {
    std::string o;
    o = " Cannot find function " + name;
    MayDay::Error(o.c_str());
  }
  case WRONG_NUMBER_OF_ARGUMENTS:
  {
    std::string o;
    o = "The function " + name + " encountered and error: wrong number/type of arguments?";
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
