#include "PyGlue.H"
#include <algorithm>
#include <iostream>
#include <Python.h>
#include <array>
using namespace std;




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
    PyObject* pack() const {
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
        PyTuple_SetItem(pArg,1,Py::pack(m_vec));// and we use Py::packCont() to pack the vector.
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
       return *this;
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

struct D{
    static const int m_d = 3;
    int m_vect[3];
    D(): m_vect{7,8,9}{};
    static constexpr bool isConvertibleToNumpy = true;
    static std::string numpyName() {return std::to_string(m_d)+"DStruc";}
};

int main()
{
    std::cout << is_arrayLike<decltype(A<5>())>::value << std::endl;
    std::cout << is_arrayLike<decltype(std::vector<decltype(A<5>())>())>::value <<std::endl;
    std::cout << is_vectorAndNumeric<decltype(std::vector<decltype(A<5>())>())>::value <<std::endl;
    std::cout << is_vectorAndNumeric<decltype(std::vector<decltype(D())>())>::value <<std::endl;
    std::cout << is_vectorAndNumeric<decltype(std::array<decltype(D()),8>())>::value <<std::endl;


    auto InitCommands=Py::DefaultInitCommands;
    InitCommands.push_back("print(sys.path)");
    Py::start(InitCommands);
    
    Py::PythonFunction("PyMyModule", "noArgs");
    
    std::array<D,5> myD;
    for (auto i : myD){
        for (auto x: i.m_vect){
            std::cout << x << std::endl; 
        }
    }
    Py::PythonFunction("PyMyModule", "printD", myD);
    
    for (auto i : myD){
        for (auto x: i.m_vect){
            std::cout << x << std::endl; 
        }
    }
    
    
    std::array<int,2>(Py::PythonReturnFunction<std::array<int,2>>("PyMyModule", "returnTuple"));
    

    std::array<A<5>,2> tuple;
    tuple[0] = Py::PythonReturnFunction<A<5>>("PyMyModule","printA",A<5>());
    tuple[1] = Py::PythonReturnFunction<A<5>>("PyMyModule","printA",A<5>());
    
    Py::PythonFunction("PyMyModule", "PrintTuple", tuple);
    B b(8);

    std::array<int,6> newTuple={-1,-2,-3,-4,-5,-6};
    std::vector<A<5>> newVec(tuple.size());
    for (int i=0; i<2; ++i)
    {
        newVec[i]=tuple[i];
    }
    const auto& dummy = newVec;
    Py::PythonFunction("PyMyModule", "PrintTuple", newTuple);
    Py::PythonFunction("PyMyModule", "PrintTuple", dummy);
    Py::PythonFunction("PyMyModule", "printB", B(8));

    C<2,double> c=Py::PythonReturnFunction<C<2,double>>("PyMyModule", "MakeC", 2,2, "double");
    
    array<int,2> iv;
    for (int j=0; j<2; ++j){
        iv[1]=j;
        for (int i=0; i<2; ++i)
        {   iv[0]=i;
            cout << i << " , " << j << "  -- " << c[iv] << endl; }
    }
    constexpr int N = 12 * 25 * 38;
    std::vector<float> X(N);
    std::vector<float> Y(N);
    std::vector<double> XD(N);
    std::vector<double> YD(N);
    std::vector<int> XI(N);
    std::vector<int> YI(N);
    std::vector<float> AI(N);
    std::array<std::size_t, 10> SI;
    std::vector<std::size_t> SSI(10);
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
    {
        auto f =[]() -> std::size_t{return (rand() %10);};
        generate(SI.begin(), SI.end(), f);
        generate(SSI.begin(), SSI.end(), f);
    }
        
   
    
    
    const int i = 2;
    int j = 3;
    int l = 1;
    double s = 3.0;
    bool T = true;
    bool F = false;

    uint m = 0;
    

    const vector<float> &XX = X;

    Py::PythonFunction("PyMyModule", "IntIntVal", i, j, X); // preload module for fairness
    Py::PythonFunction("PyMyModule", "ValVal", XI, YI,s);
    Py::PythonFunction("PyMyModule", "ValVal", X, Y,s);
    
    Py::PythonFunction("PyMyModule", "IntIntVal",i,j, AI);
    
    for (auto i: SSI)
    {std::cout << i << " "; }
    std::cout << std::endl;
    Py::PythonFunction("PyMyModule", "VectorSizeT", SSI);


    std::string o = "hello world";
    Py::PythonFunction("PyAnotherModule", "PrintStringInt", o, l);
    Py::PythonFunction("PyAnotherModule", "IntBoolVal", i, F, Y);

    int ip1 = Py::PythonReturnFunction<int>("PyMyModule", "retInt", j);
    double yy=1.0;
    double xx = Py::PythonReturnFunction<double>("PyMyModule", "retDouble", yy);
    cout << ip1 << " = " << j+1 << endl;
    cout << xx << " = " << yy-0.5 << endl;
    xx=Py::PythonReturnFunction<double>("PyMyModule","sumVA",Y);
    float sum=0.;
    for (auto &x: Y)
    {
        sum+=x;
    }
    cout << xx << " = " << sum << endl;
    
    T=Py::PythonReturnFunction<bool>("PyMyModule", "CompInt", i, j);
    cout << "Is " <<  i  << " > " << j << "? " << T << endl;

    std::string stringa = "Hello!";
    Py::PythonFunction("PyMyModule", "PrintStr", stringa);
    std::string sb = Py::PythonReturnFunction<std::string>("PyMyModule", "RetString", stringa);
    Py::PythonFunction("PyMyModule", "PrintStr", sb);

    if (!Py::isFuncDefined("PyAnotherModule", "oops"))
    {
        cout << "The following call will try to run a function that does not exist" << endl;
        Py::PythonFunction("PyAnotherModule", "oops", i, F, Y);
    }

    std::cout << "Ending program" << std::endl;
    Py::stop();
    return 0;
};
