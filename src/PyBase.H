#pragma once
#include "MayDay.H"
#include "Python.h"
#include <map>
#include <vector>
class PyBase
{
  // data members
protected:
  // stores the pointers of already loaded modules
  static std::map<std::string, PyObject *> m_Module_map;
public:
  static const std::vector<std::string> DefaultInitCommands;

  // public interface
public:
  PyBase()=delete;  //deleted, since this class is all static
  virtual ~PyBase(); // Destructor

  PyBase(const PyBase &) = delete;
  PyBase &operator=(const PyBase) = delete;

static void start(std::vector<std::string> initCommands=PyBase::DefaultInitCommands);
static void stop();

  // accessor to inquire about existence of function
  // This will load the Module (if it has not been loaded yet)
  // and check if the function is present.
  static bool isFuncDefined(std::string Module, std::string function);

  // these functions are used to pack and unpack simple types.
  // if tagIt is true, the appropriate tag is added at the end to be used
  // by the PyGlue decorator to infer type.
  static PyObject *packDouble(double a_x, bool tagIt = true);
  static PyObject *packFloat(float a_x, bool tagIt = true);
  static PyObject *packInt(int a_i, bool tagIt = true);
  static PyObject *packBool(bool a_b, bool tagIt = true);
  static PyObject *packString(std::string s, bool tagIt = true);
  static PyObject *packSize_t(std::size_t a_i, bool tagIt = true);

  static int unpackInt(PyObject *a_pin);
  static float unpackFloat(PyObject *a_pin);
  static double unpackDouble(PyObject *a_pin);
  static std::string unpackString(PyObject *a_pin);
  static bool unpackBool(PyObject *a_pin);
  static std::size_t unpackSize_t(PyObject *a_pin);


protected:
  static void runVoidFunction(std::string Module, std::string function,
                              std::vector<PyObject *> arg);
  static PyObject *runFunction(std::string Module, std::string function,
                               std::vector<PyObject *> arg);


  enum Errors
  {
    MULTIPLE_INSTANCES_DETECTED = 0,
    MODULE_NOT_FOUND,
    FUNCTION_NOT_FOUND,
    WRONG_NUMBER_OF_ARGUMENTS,
    VALARRAY_TYPE_NOT_SUPPORTED,
    CANNOT_PACK_TYPE,
    CANNOT_UNPACK_TYPE,
    CANNOT_START_INTERPRETER,
    INTERPRETER_CANNOT_BE_INITIALIZED,
    TUPLE_HAS_WRONG_NUMBER_OF_ELEMENTS,
    INTERPRETER_NEEDS_TO_BE_INITIALIZED
  }; // stores labels for error and warning messages
protected:
  // handlers for exceptions
  static void lintcatcher(int a_i);
  static void lintcatcher(int a_i, std::string toOutput);
  // sets or returns Module, whichever is needed
private:
  static PyObject *ImportAndGetModule(std::string ModuleName);
  static PyObject *loadModule(std::string moduleName);
};