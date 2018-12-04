
import numpy as np
valarrayTypes = {'int': np.int32,'float': np.float32,'double': np.float64}


def make_from_memView(memView, shape, dtype=np.float64, order='C', own_data=True):
    arr = np.ndarray(tuple(shape[:]), dtype, memView, order=order)
    return arr

def FABToNumpy(Args):
    size=Args[1]
    nComp=Args[3]
    view=Args[4]
    arr = make_from_memView(view,np.append(size,np.array(nComp)),order='F')
    return arr
def ValarrayToNumpy(v):
    size=v[0]
    TypeOfObject=v[1]
    view=v[2]
    
    arr=make_from_memView(view,[size],order='F', dtype=valarrayTypes[TypeOfObject])
    return arr


        
    
   
