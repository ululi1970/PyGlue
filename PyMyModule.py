import PyGlue as pg
import numpy as np
def ValVal(v,w,s):
    
    V = pg.ValarrayToNumpy(v)
    W = pg.ValarrayToNumpy(w)
    
    V[:]=s*np.tanh(W)
    
def IntIntVal(i, j, w):
   
    W = pg.ValarrayToNumpy(w)
    W[4] = -12.

def retInt(j):
    return j[0]+1    

def retDouble(x):
   
    return x[0]-.5

def sumVA(X):
    x=pg.ValarrayToNumpy(X)
    return x.sum()

def CompInt(i, j):
    return (i[0] > j[0])
    
def RetString(s):
    return s[0]

def PrintStr(s):
    print(s)

def DoSomethingToV(v):
    W = pg.ValarrayToNumpy(v)
    W[0] = -1
    
    