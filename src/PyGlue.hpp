template <class T>
 inline PyObject* Py::pack(T &&t)
  {
    PyObject* pArg;
    if constexpr (is_arrayLike<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value)
    {
      if constexpr (is_vectorAndNumeric<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value)
      {
          using type = typename std::remove_reference<T>::type::value_type;
          PyObject *pView = Py::makeView(t);
          PyObject *pSize = PyLong_FromLong(t.size());
          // std::cout << TypeOf<T>().c_str() << std::endl;
          PyObject *pTypeOfT = PyUnicode_FromString(Py::TypeOf<type>().c_str());
          pArg = PyTuple_New(3 + 1);
          PyTuple_SetItem(pArg, 0, pSize);
          PyTuple_SetItem(pArg, 1, pTypeOfT);
          PyTuple_SetItem(pArg, 2, pView);
          std::string label = "Numpy";
          PyTuple_SetItem(pArg, 3, Py::packString(label, false));
          if (PyErr_Occurred())
          {
            PyErr_Print();
          }
      
      
      } 
      else 
      {

        pArg = PyTuple_New(t.size() + 1);
        int pos = 0;
        for (auto &item : t)
        {
          PyTuple_SetItem(pArg, pos++, Py::pack(item));
        }
        PyTuple_SetItem(pArg, pos, PyUnicode_FromString(std::string("tuple").c_str()));
        if (PyErr_Occurred())
        {
          PyErr_Print();
        }
        
      }
    } // end block for array like objects
    else // fall back on type packing member function
    {
      pArg = t.pack();
    }
    return pArg;
  }

template <class T>
inline void Py::unpack(T &t, PyObject *a_pin)
  {
    if constexpr (is_arrayLike<typename std::remove_reference<T>::type>::value)
    {
      for (int i = 0; i < t.size(); ++i)
        Py::unpack(t[i], PyTuple_GetItem(a_pin, i));
      if (PyErr_Occurred())
      {
       PyErr_Print();
      }
    }
    else
    {
      t = T(a_pin);
    }
  }
//
template <class... Ts>
void Py::PythonFunction(std::string Module, std::string function, Ts &&...ts)
{
  std::vector<PyObject *> args;
  if constexpr (sizeof...(ts) != 0)
    Py::BuildArgsVector(args, std::forward<Ts>(ts)...); // std::forward preserves the nature of the args
  runVoidFunction(Module, function, args);
}
//
template <class T, class... Ts>
T Py::PythonReturnFunction(std::string Module, std::string function, Ts &&...ts)
{
  std::vector<PyObject *> args;
  if constexpr (sizeof...(ts) != 0)
    Py::BuildArgsVector(args, std::forward<Ts>(ts)...);
  PyObject *return_value = runFunction(Module, function, args);
  T t;
  Py::unpack(t,return_value);
  Py_DECREF(return_value);
  return t;
}
//
template <class T>
inline std::string Py::TypeOf()
{
  const bool check_type_is_defined = SAME(int) || SAME(float) || SAME(double);
  static_assert(check_type_is_defined,
                "Numeric arrays are only supported for int, float and double. "
                "Feel free to add to the list and post back.");
  TYPEtoSTR(int) TYPEtoSTR(double) TYPEtoSTR(float)

      Py::lintcatcher(Errors::VALARRAY_TYPE_NOT_SUPPORTED);
  return "";
}
//
template <class T>
PyObject *Py::makeView(T &a_v)
{
  using type = typename T::value_type;
  Py::TypeOf<type>(); // will stop compilation if type is not in the list

  unsigned long long size = a_v.size() * sizeof(type);
  char *p = reinterpret_cast<char *>(&a_v[0]);
  return PyMemoryView_FromMemory(p, size, PyBUF_WRITE);
}
//
template <class T>
PyObject *Py::makeView(const T &a_v)
{
  using type = typename T::value_type;
  Py::TypeOf<type>(); // will stop compilation if type is not in the list

  unsigned long long size = a_v.size() * sizeof(type);
  auto it(a_v.begin());

  char *p = reinterpret_cast<char *>(const_cast<type *>(&a_v[0]));

  return PyMemoryView_FromMemory(p, size, PyBUF_READ);
}

// 

// bottom of recursion
template <class T>
void Py::BuildArgsVector(std::vector<PyObject *> &args, T &&t)
{
  args.push_back(pack(std::forward<T>(t)));
}
// recursion definition
template <class T, class... Args>
void Py::BuildArgsVector(std::vector<PyObject *> &args, T &&t, Args &&...ts)
{

  BuildArgsVector(args, std::forward<T>(t));
  if constexpr (sizeof...(ts) != 0)
    BuildArgsVector(args, std::forward<Args>(ts)...);
}

// specializations
template <>
inline void Py::unpack(bool &x, PyObject *a_pin)
{
  x = Py::unpackBool(a_pin);
}
template <>
inline void Py::unpack(int &x, PyObject *a_pin)
{
  x = Py::unpackInt(a_pin);
}
template <>
inline void Py::unpack(float &x, PyObject *a_pin)
{
  x = Py::unpackFloat(a_pin);
}
template <>
inline void Py::unpack(double &x, PyObject *a_pin)
{
  x = Py::unpackDouble(a_pin);
}
template <>
inline void Py::unpack(std::string &x, PyObject *a_pin)
{
  x = Py::unpackString(a_pin);
}
template <>
inline PyObject *Py::pack(const bool &x)
{
  return Py::packBool(x);
}
template <>
inline PyObject *Py::pack(const int &x)
{
  return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(const float &x)
{
  return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(const double &x)
{
  return Py::packDouble(x);
}
template <>
inline PyObject *Py::pack(const std::string &x)
{
  return Py::packString(x);
}
template <>
inline PyObject *Py::pack(bool &x)
{
  return Py::packBool(x);
}
template <>
inline PyObject *Py::pack(int &x)
{
  return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(float &x)
{
  return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(double &x)
{
  return Py::packDouble(x);
}
template <>
inline PyObject *Py::pack(std::string &x)
{
  return Py::packString(x);
}
template <>
inline PyObject *Py::pack(const char *x)
{
  const std::string s(x);
  return Py::packString(s);
}
template <>
inline PyObject *Py::pack(int &&x)
{
  return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(float &&x)
{
  return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(double &&x)
{
  return Py::packDouble(x);
}
