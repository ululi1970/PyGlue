#include "PyGlue.H"
#include<valarray>
#include<iostream>
int main(){
    
    double v[]={1. ,2., 3., 4., 5};
    std::valarray<double> V (v,5);
    double x=1.0;

    
    int i=2;
    int j=3;
    int l=1;
    bool t=true;
    bool f=false;
    std::cout << V[4]<< "\n";
    Py Python;
    
    Python.PythonFunction("PyTest","IntIntVal",i,j,V);

    return 0;
};
