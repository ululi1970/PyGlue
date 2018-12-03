
import numpy as np 
print("module imported")
def make_from_memView(memView, shape, dtype=np.float64, order='C', own_data=True):
    arr = np.ndarray(tuple(shape[:]), dtype, memView, order=order)
    return arr

def make_nparray_fromFAB(Args):
    size=Args[1]
    nComp=Args[3]
    view=Args[4]
    arr = make_from_memView(view,np.append(size,np.array(nComp)),order='F')
    return arr
def make_nparray_FromValarray(v):
    size=v[0]
    view=v[1]
    arr=make_from_memView(view,[size,1],order='F')
    return arr

def ValVal(v,w):
   
    V = make_nparray_FromValarray(v)
    W = make_nparray_FromValarray(w)
    
    V[:]=np.tanh(W)
    
def IntIntVal(i, j, w):
    W = make_nparray_FromValarray(w)
    W[4] = -12.
    
        
    
   
