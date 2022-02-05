#include "PyGlue.H"
#include <algorithm>
#include <iostream>
#include <Python.h>
#include <array>
using namespace std;


Py Python; //note the global definition of Python
// there should only be one instance of this class.

//example of class which is assignable
template<int N>
struct A {
    int m_int;
    array<double,N> m_arr;
    // 
    A():m_int(0),m_arr{0.}{}
    // if we expect to create an instance of A with data passed from Python we need
    // the following constructor and the class must have both a copy constructor 
    // and an assignment operator. 
    A(PyObject* a_pin){
        // Python sends a tuple with N+1 elements.
        m_int=Py::unpackInt(PyTuple_GetItem(a_pin,0)); // the first is m_int
        for (int i=0; i<N; ++i){
            m_arr[i]=Py::unpackDouble(PyTuple_GetItem(a_pin,i+1));
            // the other are sent to the entries of m_arr
        }

    }
    // the function that packs data to Python
    PyObject* pack(){
        PyObject* pArg=PyTuple_New(2+1); // On the Python side this will appear as a tuple
                                        // with three elements. The arguments (m_int and m_arr) and label
        PyObject* p1=Py::packInt(m_int,false); //we use packInt to pack m_int      
        PyTuple_SetItem(pArg,0,p1); // and we set it as the first element of the tuple
        PyTuple_SetItem(pArg,1,Py::pack(m_arr));
        PyTuple_SetItem(pArg,2,Py::packString("A", false)); //the third contains the label "A"
        return pArg; // On the Python side, it will be (m_int, (m_arr[0],...,m_arr[N-1]), 'A' )
    }
};
struct B {
    A<4> m_a;
    vector<double> m_vec;
    B(const int n): m_a(), m_vec(n){
        for (int i=0; i<n; ++i){
            m_vec[i]=10.+i;
        }
    }
    //
    B(const B&) = delete;
    B & operator=(const B&)=delete;
    //since operator= is deleted, calling PythonReturnFunction<B>(...) will result in a compilation error
    PyObject* pack(){
        PyObject* pArg=PyTuple_New(2+1);
        PyTuple_SetItem(pArg,0,m_a.pack());// here we use A::pack() to pack A into the first element 
        PyTuple_SetItem(pArg,1,Py::packCont(m_vec));// and we use Py::packCont() to pack the vector.
                                                    // in such a way that a numpy can be aliased to it.    
        PyTuple_SetItem(pArg,2,Py::packString("B",false)); // the label for identification. 
        
        return pArg;
        }
    

};
// a class which uses Python to allocate memory on heap. 
template <int dim, class T=double>
struct C {
    T* m_ptr;
    array<int,dim> m_size;
    PyObject* m_py;
    C():m_size({0}),m_ptr(nullptr),m_py(nullptr){};
    ~C(){
        Py_DECREF(m_py);
    }
    C& operator=(const C&other){
       if (m_py) Py_DECREF(m_py);
       m_ptr=other.m_ptr;
       m_size=other.m_size;
       m_py=other.m_py;
    }
    C(const C& other):m_size(other.m_size),m_ptr(other.m_ptr),m_py(other.m_py){
        Py_INCREF(m_py);
    }
    C(PyObject* a_pin):m_py(a_pin){
        Py::unpack(m_size,PyTuple_GetItem(a_pin,1));
        m_ptr=static_cast<T *>(PyLong_AsVoidPtr(PyTuple_GetItem(a_pin,2)));
        Py_INCREF(m_py);

    }
    T & operator[](const array<int, dim> iv){
        int index=iv[0];
        for (int dir=1; dir<dim; ++dir)
        {
            index+=iv[dir]*m_size[dir-1];
            
        }
        return *(m_ptr+index);}
};

int main()
{
    std::array<int,2>(Python.PythonReturnFunction<std::array<int,2>>("PyMyModule", "returnTuple"));
    

    std::array<A<5>,2> tuple;
    tuple[0] = Python.PythonReturnFunction<A<5>>("PyMyModule","printA",A<5>());
    tuple[1] = Python.PythonReturnFunction<A<5>>("PyMyModule","printA",A<5>());
    Python.PythonFunction("PyMyModule", "PrintTuple", tuple);
    B b(8);

    std::array<int,6> newTuple={-1,-2,-3,-4,-5,-6};
    std::vector<A<5>> newVec(tuple.size());
    for (int i=0; i<2; ++i)
    {
        newVec[i]=tuple[i];
    }
    Python.PythonFunction("PyMyModule", "PrintTuple", newTuple);
    Python.PythonFunction("PyMyModule", "PrintTuple", newVec);
    Python.PythonFunction("PyMyModule", "printB", B(8));

    C<2,double> c=Python.PythonReturnFunction<C<2,double>>("PyMyModule", "MakeC", 2,2, "double");
    
    array<int,2> iv;
    for (int j=0; j<2; ++j){
        iv[1]=j;
        for (int i=0; i<2; ++i)
        {   iv[0]=i;
            cout << i << " , " << j << "  -- " << c[iv] << endl; }
    }
    int N = 12 * 25 * 38;
    std::vector<float> X(N);
    std::vector<float> Y(N);
    std::vector<double> XD(N);
    std::vector<double> YD(N);
    std::vector<int> XI(N);
    std::vector<int> YI(N);
    std::vector<float> AI(N);
    // cout << boolalpha;
    // cout<< "Are valarrays sizeable ? " << has_size_func<valarray<double>>::value << endl;
    // cout<< "Are valarrays sizeable (alternative formulation) ? " << has_size<valarray<double>>::value << endl;
    // cout << "Are integers sizeable ? " << has_size_func<int>::value << endl;
    // cout << "Are integers sizeable (alternative formulation) ? " << has_size<int>::value << endl;
    // cout << "Are maps sizeable ? " << has_size_func<map<std::string,int>>::value << endl;
    MayDay::StopOnExit(true);

    {
        auto f = []() -> float { return (rand() % 10) / 10.; };
        generate(begin(X), end(X), f);
    }
    {
        auto f = []() -> double { return (rand() % 10) / 10.; };
        generate(begin(XD), end(XD), f);
    }
    {
        auto f = []() -> int { return (rand() % 10); };
        generate(begin(XI), end(XI), f);
    }
    {
        auto f=[]() ->float {return (rand() %10)/10.;};
        generate(AI.begin(), AI.end(), f);
    }
    
   
    
    
    const int i = 2;
    int j = 3;
    int l = 1;
    double s = 3.0;
    bool T = true;
    bool F = false;

    uint m = 0;
    

    const vector<float> &XX = X;

    Python.PythonFunction("PyMyModule", "IntIntVal", i, j, X); // preload module for fairness
    Python.PythonFunction("PyMyModule", "ValVal", XI, YI,s);
    Python.PythonFunction("PyMyModule", "ValVal", X, Y,s);
    
    Python.PythonFunction("PyMyModule", "IntIntVal",i,j, AI);

    std::string o = "hello world";
    Python.PythonFunction("PyAnotherModule", "PrintStringInt", o, l);
    Python.PythonFunction("PyAnotherModule", "IntBoolVal", i, F, Y);

    int ip1 = Python.PythonReturnFunction<int>("PyMyModule", "retInt", j);
    double yy=1.0;
    double xx = Python.PythonReturnFunction<double>("PyMyModule", "retDouble", yy);
    cout << ip1 << " = " << j+1 << endl;
    cout << xx << " = " << yy-0.5 << endl;
    xx=Python.PythonReturnFunction<double>("PyMyModule","sumVA",Y);
    float sum=0.;
    for (auto &x: Y)
    {
        sum+=x;
    }
    cout << xx << " = " << sum << endl;
    
    T=Python.PythonReturnFunction<bool>("PyMyModule", "CompInt", i, j);
    cout << "Is " <<  i  << " > " << j << "? " << T << endl;

    std::string stringa = "Hello!";
    Python.PythonFunction("PyMyModule", "PrintStr", stringa);
    std::string sb = Python.PythonReturnFunction<std::string>("PyMyModule", "RetString", stringa);
    Python.PythonFunction("PyMyModule", "PrintStr", sb);

    if (!Python.isFuncDefined("PyAnotherModule", "oops"))
    {
        cout << "The following call will try to run a function that does not exist" << endl;
        Python.PythonFunction("PyAnotherModule", "oops", i, F, Y);
    }
    return 0;
};
