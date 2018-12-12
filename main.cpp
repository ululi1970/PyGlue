#include "PyGlue.H"
#include <valarray>
#include <iostream>
#include <Python.h>
#include <algorithm>
#include <chrono>
#include <array>
using namespace std;
using namespace std::chrono;

Py Python;
void testconst(const valarray<double>&v )
{
    Python.PythonFunction("PyMyModule", "DoSomethingToV", v);
}
class A {};
int main()
{

    int N = 12 * 25 * 38;
    std::valarray<float> X(N);
    std::valarray<float> Y(N);
    std::valarray<double> XD(N);
    std::valarray<double> YD(N);
    std::valarray<int> XI(N);
    std::valarray<int> YI(N);
    std::vector<float> AI(N);
    cout << boolalpha;
    cout<< "Are valarrays sizeable ? " << has_size_func<valarray<double>>::value << endl;
    cout << "Are integers sizeable ? " << has_size_func<int>::value << endl;
    cout << "Are maps sizeable ? " << has_size_func<map<std::string,int>>::value << endl;
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
    auto start = high_resolution_clock::now();
    YD = tanh(XD);
    auto stop = high_resolution_clock::now();
    Y=tanh(X);
    //cout << Y[129] << endl;
    // cout << X[129] << endl; // so that the optimizer does not play tricks

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "time used by native C++ tanh operating on a valarray<double>" << endl;
    cout << duration.count() << " milliseconds" << endl;
    cout << "value of Y at random location" << endl;
    
    const int i = 2;
    int j = 3;
    int l = 1;
    double s = 3.0;
    bool T = true;
    bool F = false;

    uint m = 0;
    //testconst(XD);

    const valarray<float> &XX = X;

    Python.PythonFunction("PyMyModule", "IntIntVal", i, j, X); // preload module for fairness
    Python.PythonFunction("PyMyModule", "ValVal", XI, YI,s);
    Python.PythonFunction("PyMyModule", "ValVal", X, Y,s);
    start = high_resolution_clock::now();
    Python.PythonFunction("PyMyModule", "ValVal", XD, YD, s);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "time used by Python np.tanh operating on a valarray<double>" << endl;
    cout << duration.count() << " milliseconds" << endl;
    cout << "value of Y at random location" << endl;
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
    cout << xx << " = " << Y.sum() << endl;
    
    T=Python.PythonReturnFunction<bool>("PyMyModule", "CompInt", i, j);
    cout << "Is " <<  i  << " > " << j << "? " << T << endl;

    std::string stringa = "ciao.";
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
