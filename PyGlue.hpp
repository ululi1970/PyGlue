
//
template <class T>
  T Py::PythonReturnFunction(std::string Module, std::string function) {
    PyObject *return_value = runFunction(Module, function, m_args);
    T t;
    unpack(t, return_value);
    Py_DECREF(return_value);
    return t;
  }
//
  template <class... Ts>
  void Py::PythonFunction(std::string Module, std::string function, Ts &&... ts) {
    this->BuildArgsVector(std::forward<Ts>(ts)...); // std::forward preserves the nature of the args
    this->runVoidFunction(Module, function, m_args);
    m_args.clear();
  }
//
  template <class T, class... Ts>
  T Py::PythonReturnFunction(std::string Module, std::string function, Ts &&... ts) {
    this->BuildArgsVector(std::forward<Ts>(ts)...);
    PyObject *return_value = this->runFunction(Module, function, m_args);
    m_args.clear();
    T t;
    this->unpack<T>(t, return_value);
    Py_DECREF(return_value);
    return t;
  }
//
template <class T>
  inline std::string Py::TypeOf() {
    const bool check_type_is_defined = SAME(int) || SAME(float) || SAME(double);
    static_assert(check_type_is_defined,
                  "Valarrays are only supported for int, float and double. "
                  "Feel free to add to the list and post back.");
    TYPEtoSTR(int) TYPEtoSTR(double) TYPEtoSTR(float)

        Py::lintcatcher(VALARRAY_TYPE_NOT_SUPPORTED);
    return "";
  }
//
template <class T>
  PyObject* Py::makeView(T &a_v) {
    using type = typename T::value_type;
    Py::TypeOf<type>();  // will stop compilation if type is not in the list

    unsigned long long size = a_v.size() * sizeof(type);
    char *p = reinterpret_cast<char *>(&a_v[0]);
    return PyMemoryView_FromMemory(p, size, PyBUF_WRITE);
  }
//
  template <class T>
  PyObject* Py::makeView(const T &a_v) {
    using type = typename T::value_type;
    Py::TypeOf<type>();  // will stop compilation if type is not in the list

    unsigned long long size = a_v.size() * sizeof(type);
    auto it(a_v.begin());

    char *p = reinterpret_cast<char *>(const_cast<type *>(&a_v[0]));

    return PyMemoryView_FromMemory(p, size, PyBUF_READ);
  }

// these functions are used to pack contiguous data 
template <class T>
  PyObject* Py::packCont(T &a_v) {
    using type = typename T::value_type;
    PyObject *pView = Py::makeView(a_v);
    PyObject *pSize = PyLong_FromLong(a_v.size());
    //std::cout << TypeOf<T>().c_str() << std::endl;
    PyObject *pTypeOfT = PyUnicode_FromString(Py::TypeOf<type>().c_str());
    PyObject *pArgs = PyTuple_New(3+1);
    PyTuple_SetItem(pArgs, 0, pSize);
    PyTuple_SetItem(pArgs, 1, pTypeOfT);
    PyTuple_SetItem(pArgs, 2, pView);
    std::string label="Numpy";
    PyTuple_SetItem(pArgs,3, Py::packString(label,false));
    if (PyErr_Occurred()) {
    PyErr_Print();
  }
    return pArgs;
  }

  template <class T>
  PyObject* Py::packCont(const T &a_v) {
    using type = typename T::value_type;
    PyObject *pView = Py::makeView(a_v);
    PyObject *pSize = PyLong_FromLong(a_v.size());
    //std::cout << TypeOf<T>().c_str() << std::endl;
    PyObject *pTypeOfT = PyUnicode_FromString(Py::TypeOf<type>().c_str());
    PyObject *pArgs = PyTuple_New(4);
    PyTuple_SetItem(pArgs, 0, pSize);
    PyTuple_SetItem(pArgs, 1, pTypeOfT);
    PyTuple_SetItem(pArgs, 2, pView);
    std::string label="Numpy";
    PyTuple_SetItem(pArgs,3, Py::packString(label,false));
    if (PyErr_Occurred()) {
    PyErr_Print();
  }
    return pArgs;
  }

 // bottom of recursion
  template <class T>
  void Py::BuildArgsVector(T&& t) {
    PyObject *r = pack(std::forward<T>(t));
    m_args.push_back(r);
  }
  // recursion definition
  template <class T, class... Args>
  void Py::BuildArgsVector(T &&t, Args&&... ts) {
    BuildArgsVector(std::forward<T>(t));
    BuildArgsVector(std::forward<Args>(ts)...);
  }

// specializations
template <>
inline void Py::unpack(bool &x, PyObject *a_pin){
  x=Py::unpackBool(a_pin);
}
template <>
inline void Py::unpack(int &x, PyObject *a_pin){
  x=Py::unpackInt(a_pin);
}
template <>
inline void Py::unpack(float &x, PyObject *a_pin){
  x=Py::unpackFloat(a_pin);
}
template <>
inline void Py::unpack(double &x, PyObject *a_pin){
  x=Py::unpackDouble(a_pin);
}
template <>
inline void Py::unpack(std::string &x, PyObject *a_pin){
  x=Py::unpackString(a_pin);
}
template <>
inline PyObject *Py::pack(const bool &x){
  return Py::packBool(x);
}
template <>
inline PyObject *Py::pack(const int &x){
  return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(const float &x){
  return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(const double &x){
  return Py::packDouble(x);
}
template <>
inline PyObject *Py::pack(const std::string &x){
  return Py::packString(x);
}
template <>
inline PyObject *Py::pack(bool &x){
  return Py::packBool(x);
}
template <>
inline PyObject *Py::pack(int &x){
  return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(float &x){
  return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(double &x){
  return Py::packDouble(x);
}
template <>
inline PyObject *Py::pack(std::string &x){
  return Py::packString(x);
}
template <>
inline PyObject *Py::pack(const char* x){
    const std::string s(x);
    return Py::packString(s);
}
template <>
inline PyObject *Py::pack(int&& x){
    return Py::packInt(x);
}
template <>
inline PyObject *Py::pack(float&& x){
    return Py::packFloat(x);
}
template <>
inline PyObject *Py::pack(double&& x){
    return Py::packDouble(x);
}


