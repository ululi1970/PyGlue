
import numpy as np
from functools import wraps

class A:
    def __init__(self, Arg):
        if Arg[-1]!= "A":
            raise ValueError("A constructor called with wrong Arg")
        self.m_int=Arg[0] # first element of the incoming tuple is a single integer
        self.m_arr=tuplifier(Arg[1]) # the second element contains the tuple in vec
    def __str__(self):
        string = "Class A \n"
        string+= "m_int = " + str(self.m_int) + "\n"
        string+= "m_arr = " + str(self.m_arr) + "\n"
        return string
    def toCPP(self):
        out=(self.m_int,)+self.m_arr
        return out



class B:
    def __init__(self,Arg):
        if Arg[-1]!="B":
            raise ValueError("B constructor called with wrong Arg")
        self.A=A(Arg[0])
        self.m_vect=ArrayToNumpy(Arg[1])

    def __str__(self):
        string = "Class B \n"
        string+= str(self.A) + "\n"
        string+= str(self.m_vect)
        return string
    def doSomething(self):
        self.m_vect*=2




def strip(v):
    return v[0]
def id(v):
    return v    

def make_from_memView(memView, shape, dtype=np.float64, order='C', own_data=True):
    arr = np.ndarray(tuple(shape[:]), dtype, memView, order=order)
    return arr


def ArrayToNumpy(v):
    size=v[0]
    TypeOfObject=v[1]
    view=v[2]
    arr=make_from_memView(view,[size],order='F', dtype=ArrayTypes[TypeOfObject])
    return arr

def tuplifier(arg):
    return tuple([Transmogrifiers[WhatIs(a)](a) for a in arg[:-1]])

ArrayTypes = {'int': np.int32,'float': np.float32,'double': np.float64, 'size_t' : np.uint64}

Transmogrifiers ={
                 'int' : strip,
                 'double' : strip,
                 'float' : strip,
                 'bool' : strip,
                 'str' : strip,
                 'size_t' : strip,
                 'id' : id,
                 'A' : A,
                 'B' : B,
                 'Numpy' : ArrayToNumpy,
                 'tuple' : tuplifier
}


def WhatIs(arg):
    ''' Inspects arg and decide what type it is.
    Tuples should be avoided as native arguments, since in this 
    case the transmogrifier will just try to apply the dictionary 
    correspoding to the last element of the tuple to the tuple itself.'''


    if isinstance(arg, (str, int, bool, float, list, A, B)):
        return 'id'
    try:
        return arg[-1]
    except:
        return 'id'

def PYGLUE(func): # this is the decorator
    @wraps(func)
    def wrapper(*args, **kargs):
        try:
            args = tuple([Transmogrifiers[WhatIs(a)](a) for a in args])
        except:
            print([a for a in args])
            print([WhatIs(a) for a in args])
           
            raise(ValueError(""))
            
        return func(*args, **kargs)
    return wrapper




        
    
   
