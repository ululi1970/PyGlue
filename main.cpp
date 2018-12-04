#include "PyGlue.H"
#include <valarray>
#include <iostream>
#include <Python.h>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;

int main()
{

    int N = 128 * 256 * 384;
    std::valarray<float> X(N);
    std::valarray<float> Y(N);
    std::valarray<double> XD(N);
    std::valarray<double> YD(N);
    std::valarray<int> XI(N);
    std::valarray<int> YI(N);

    cout << "size of int " << sizeof(int) << endl;
    cout << "size of float " << sizeof(float) << endl;
    cout << "size of double " << sizeof(double) << endl;

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

    auto start = high_resolution_clock::now();
    YD = tanh(XD);
    auto stop = high_resolution_clock::now();
    //cout << Y[129] << endl;
    // cout << X[129] << endl; // so that the optimizer does not play tricks

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "time used by native C++ tanh operating on a valarray<double>" << endl;
    cout << duration.count() << " milliseconds" << endl;
    cout << "value of Y at random location" << endl;
    cout << Y[100] << endl;
    int i = 2;
    int j = 3;
    int l = 1;
    bool T = true;
    bool F = false;

    Py Python;

    Python.PythonFunction("PyTest", "IntIntVal", i, j, X); // preload module for fairness
    Python.PythonFunction("PyTest", "ValVal", XI, YI);
    Python.PythonFunction("PyTest", "ValVal", X, Y);
    start = high_resolution_clock::now();
    Python.PythonFunction("PyTest", "ValVal", XD, YD);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    cout << "time used by Python np.tanh operating on a valarray<double>" << endl;
    cout << duration.count() << " milliseconds" << endl;
    cout << "value of Y at random location" << endl;
    cout << Y[100] << endl;

    std::string o = "hello world";
    Python.PythonFunction("PyAnotherModule", "PrintStringInt", o, l);
    Python.PythonFunction("PyAnotherModule", "IntBoolVal", i, F, Y);
    if (!Python.isFuncDefined("PyAnotherModule", "oops"))
    {
        cout << "The following call will try to run a function that does not exist" << endl;
        Python.PythonFunction("PyAnotherModule", "oops", i, F, Y);
    }
    return 0;
};
