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
        PyObject* p2=PyTuple_New(N); // since m_arr contains multiple elements, we create a tuple of the same size 
        for(int i=0; i<N; ++i)
        {
            PyTuple_SetItem(p2,i,Py::packDouble(m_arr[i], false));
            // at the end, this will be (m_arr[0],...,m_arr[N-1])
        }
        PyTuple_SetItem(pArg,1,p2);// and we set it as the second element
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
int main()
{
    A<5> newA = Python.PythonReturnFunction<A<5>>("PyMyModule","printA",A<5>());

    B b(8);
    Python.PythonFunction("PyMyModule", "printB", B(8));

    
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
