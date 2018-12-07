#include "PyGlue.H"
#include "mpi.h"
#include <valarray>
#include <iostream>
#include <Python.h>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

Py Python;
void testconst(const valarray<double>&v )
{
    Python.PythonFunction("PyMyModule", "DoSomethingToV", v);
}

int main(int argc, char* argv[])
{
#ifdef CH_MPI
    MPI_Init(&argc, &argv);
#endif

    std::string Str[3];
    Str[0]="qui";
    Str[1]="quo";
    Str[2]="qua";
    for (auto i=1; i<3; ++i){Str[0]+= Str[i];}

    Python.PythonFunction("PyMyModule", "threestr", Str[0]);
    return 0;
    
    int N = 12 * 25 * 38;
    std::valarray<float> X(N);
    std::valarray<float> Y(N);
    std::valarray<double> XD(N);
    std::valarray<double> YD(N);
    std::valarray<int> XI(N);
    std::valarray<int> YI(N);

    // build a FAB of size 12*25*38
    std::vector<int> lov;
    lov.push_back(0);
     lov.push_back(0);
      lov.push_back(0);
      Vector<int> loV(lov);
    IntVect lo(loV);

    std::vector<int> hiv;
    hiv.push_back(11);
     hiv.push_back(24);
      hiv.push_back(37);
      cout<< hiv[0] << endl;
      Vector<int> hiV(hiv);
    IntVect hi(hiV);
    Box b(lo,hi);
    FArrayBox q(b,1);

    q(hi,0)=5.0;
    q(lo,0)=10.0;
    valarray<Real> qVA(q.dataPtr(0),N);

    cout<< qVA[N-1] << endl;
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
    Y=tanh(X);
    cout << Y[129] << endl;
     cout << X[129] << endl; // so that the optimizer does not play tricks

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
    Python.PythonFunction("PyMyModule", "IntValFAB", i,qVA,q);
    cout << "FAB "<< q(hi,0) << endl;
    cout << "VAL " << qVA[N-1] << endl;
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
    if (!Python.isFuncDefined("PyAnotherModule", "oops"))
    {
        cout << "The following call will try to run a function that does not exist" << endl;
        Python.PythonFunction("PyAnotherModule", "oops", i, F, Y);
    }
    #ifdef CH_MPI
    int r_mpi =MPI_Finalize();
    #else
    int r_mpi =0;
    #endif
    return r_mpi;
};
