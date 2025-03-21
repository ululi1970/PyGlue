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

#pragma once
#include "MayDay.H"
#include "Python.h"
#include <map>
#include <vector>
#include <string>
class PyBase
/**
 * @class PyBase
 * @brief A static utility class for managing Python modules and functions in C++.
 * 
 * This class provides functionality to interact with Python modules and functions,
 * including loading modules, checking function existence, and packing/unpacking
 * data types between C++ and Python. The class is entirely static and cannot be
 * instantiated.
 * 
 * @note The class is designed to work with Python's C API and assumes that the
 *       Python interpreter is properly initialized and finalized.
 */
 
/**
 * @var PyBase::m_Module_map
 * @brief A static map storing pointers to already loaded Python modules.
 * 
 * The key is the module name (std::string), and the value is a pointer to the
 * corresponding PyObject representing the module.
 */

/**
 * @var PyBase::DefaultInitCommands
 * @brief A static constant vector of default initialization commands for the Python interpreter.
 */

/**
 * @fn PyBase::PyBase()
 * @brief Deleted constructor to prevent instantiation of the class.
 */

/**
 * @fn PyBase::~PyBase()
 * @brief Virtual destructor for the PyBase class.
 */

/**
 * @fn PyBase::start(std::vector<std::string> initCommands)
 * @brief Starts the Python interpreter with the specified initialization commands.
 * 
 * @param initCommands A vector of initialization commands to execute. Defaults to
 *                     PyBase::DefaultInitCommands.
 */

/**
 * @fn PyBase::stop()
 * @brief Stops the Python interpreter and performs cleanup.
 */

/**
 * @fn PyBase::isFuncDefined(std::string Module, std::string function)
 * @brief Checks if a function is defined in a specified Python module.
 * 
 * @param Module The name of the Python module.
 * @param function The name of the function to check.
 * @return True if the function is defined, false otherwise.
 */

/**
 * @fn PyBase::packDouble(double a_x, bool tagIt)
 * @brief Packs a double value into a Python object.
 * 
 * @param a_x The double value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed double value.
 */

/**
 * @fn PyBase::packFloat(float a_x, bool tagIt)
 * @brief Packs a float value into a Python object.
 * 
 * @param a_x The float value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed float value.
 */

/**
 * @fn PyBase::packInt(int a_i, bool tagIt)
 * @brief Packs an integer value into a Python object.
 * 
 * @param a_i The integer value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed integer value.
 */

/**
 * @fn PyBase::packBool(bool a_b, bool tagIt)
 * @brief Packs a boolean value into a Python object.
 * 
 * @param a_b The boolean value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed boolean value.
 */

/**
 * @fn PyBase::packString(std::string s, bool tagIt)
 * @brief Packs a string value into a Python object.
 * 
 * @param s The string value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed string value.
 */

/**
 * @fn PyBase::packSize_t(std::size_t a_i, bool tagIt)
 * @brief Packs a size_t value into a Python object.
 * 
 * @param a_i The size_t value to pack.
 * @param tagIt If true, adds a tag for type inference. Defaults to true.
 * @return A PyObject representing the packed size_t value.
 */

/**
 * @fn PyBase::unpackInt(PyObject *a_pin)
 * @brief Unpacks an integer value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked integer value.
 */

/**
 * @fn PyBase::unpackFloat(PyObject *a_pin)
 * @brief Unpacks a float value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked float value.
 */

/**
 * @fn PyBase::unpackDouble(PyObject *a_pin)
 * @brief Unpacks a double value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked double value.
 */

/**
 * @fn PyBase::unpackString(PyObject *a_pin)
 * @brief Unpacks a string value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked string value.
 */

/**
 * @fn PyBase::unpackBool(PyObject *a_pin)
 * @brief Unpacks a boolean value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked boolean value.
 */

/**
 * @fn PyBase::unpackSize_t(PyObject *a_pin)
 * @brief Unpacks a size_t value from a Python object.
 * 
 * @param a_pin The Python object to unpack.
 * @return The unpacked size_t value.
 */

/**
 * @fn PyBase::runVoidFunction(std::string Module, std::string function, std::vector<PyObject *> arg)
 * @brief Executes a Python function that does not return a value.
 * 
 * @param Module The name of the Python module containing the function.
 * @param function The name of the function to execute.
 * @param arg A vector of arguments to pass to the function.
 */

/**
 * @fn PyBase::runFunction(std::string Module, std::string function, std::vector<PyObject *> arg)
 * @brief Executes a Python function and returns its result.
 * 
 * @param Module The name of the Python module containing the function.
 * @param function The name of the function to execute.
 * @param arg A vector of arguments to pass to the function.
 * @return A PyObject representing the result of the function.
 */

/**
 * @enum PyBase::Errors
 * @brief Enumeration of error codes used for exception handling.
 * 
 * - MULTIPLE_INSTANCES_DETECTED: Multiple instances of the interpreter detected.
 * - MODULE_NOT_FOUND: The specified module could not be found.
 * - FUNCTION_NOT_FOUND: The specified function could not be found.
 * - WRONG_NUMBER_OF_ARGUMENTS: Incorrect number of arguments passed to a function.
 * - VALARRAY_TYPE_NOT_SUPPORTED: The valarray type is not supported.
 * - CANNOT_PACK_TYPE: Failed to pack a type into a Python object.
 * - CANNOT_UNPACK_TYPE: Failed to unpack a type from a Python object.
 * - CANNOT_START_INTERPRETER: Failed to start the Python interpreter.
 * - INTERPRETER_CANNOT_BE_INITIALIZED: The interpreter could not be initialized.
 * - TUPLE_HAS_WRONG_NUMBER_OF_ELEMENTS: A tuple has an incorrect number of elements.
 * - INTERPRETER_NEEDS_TO_BE_INITIALIZED: The interpreter needs to be initialized.
 */

/**
 * @fn PyBase::lintcatcher(int a_i)
 * @brief Handles exceptions by catching errors based on an error code.
 * 
 * @param a_i The error code to handle.
 */

/**
 * @fn PyBase::lintcatcher(int a_i, std::string toOutput)
 * @brief Handles exceptions by catching errors based on an error code and outputs a message.
 * 
 * @param a_i The error code to handle.
 * @param toOutput The message to output.
 */

/**
 * @fn PyBase::ImportAndGetModule(std::string ModuleName)
 * @brief Imports a Python module and retrieves its PyObject representation.
 * 
 * @param ModuleName The name of the module to import.
 * @return A PyObject representing the imported module.
 */

/**
 * @fn PyBase::loadModule(std::string moduleName)
 * @brief Loads a Python module and stores it in the module map.
 * 
 * @param moduleName The name of the module to load.
 * @return A PyObject representing the loaded module.
 */
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
