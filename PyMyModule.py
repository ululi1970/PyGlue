import PyGlue as pg
import numpy as np
def ValVal(v,w,s):
    
    V = pg.ValarrayToNumpy(v)
    W = pg.ValarrayToNumpy(w)
    
    V[:]=s*np.tanh(W)
    
def IntIntVal(i, j, w):
   
    W = pg.ValarrayToNumpy(w)
    W[4] = -12.
    