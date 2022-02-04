import PyGlue as pg
import numpy as np

@pg.PYGLUE
def PrintTuple(v):
    [print(item) for item in v]

@pg.PYGLUE
def MakeC(*args):
    print()
    X=np.ones(args[:-1],dtype=args[-1])
    print(X)
    print(hex(X.__array_interface__['data'][0]))
    return (X,)+args[:-1]+(X.__array_interface__['data'][0],)
@pg.PYGLUE
def ValVal(v,w,s):
    

    v[:]=s*np.tanh(w)

@pg.PYGLUE
def IntIntVal(i, j, w):
   
    print("intint val",i,j,w.shape)
    w[4] = -12.

@pg.PYGLUE
def retInt(j):
    return j+1    

@pg.PYGLUE
def retDouble(x):
   
    return x-.5

@pg.PYGLUE
def sumVA(X):
    print(X.shape)
    return X.sum()

@pg.PYGLUE
def CompInt(i, j):
    return (i > j)

@pg.PYGLUE    
def RetString(s):
    return s

@pg.PYGLUE
def PrintStr(s):
    print(s)

@pg.PYGLUE
def DoSomethingToV(v):
    
    v[0] = -1

@pg.PYGLUE    
def printA(v):
    print(v)
    v.m_arr=tuple(x+2. for x in v.m_arr)
    v.m_int=5
    return v.toCPP()
@pg.PYGLUE
def printB(v):
    print(v)
    v.m_vect-=10


