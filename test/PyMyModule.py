import PyGlue as pg
import numpy as np
import sys
@pg.PYGLUE
def noArgs():
    print("No args")
    pass

@pg.PYGLUE
def PrintTuple(v):
    [print(item) for item in v]

@pg.PYGLUE
def returnTuple():
    return (1,2)

@pg.PYGLUE
def MakeC(*args):
    print(args)
    X=np.ones(args[:-1],dtype=args[-1])
    print(X)
    print(hex(X.__array_interface__['data'][0]))
    out = (X,)+(tuple(args[:-1]),)+(X.__array_interface__['data'][0],)
    return out
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
    v.m_arr=np.array([x+2. for x in v.m_arr])
    v.m_int=5
    return v.toCPP()
@pg.PYGLUE
def printB(v):
    print(v)
    v.m_vect-=10

@pg.PYGLUE
def VectorSizeT(v):
    print(type(v))
    print(v[:10])
    #sys.exit(0)

@pg.PYGLUE
def ArraySizeT(v):
    print(type(v))
    print(v)
    #sys.exit(0)    
@pg.PYGLUE
def printD(v):
    print(v)
    print(v.dtype)
    print(v.shape)
    for i in range(len(v)):
        print(type(v[i]))
        v[i]=(0,0,0)
        
    print(v)