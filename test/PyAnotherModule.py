import PyGlue as pg
@pg.PYGLUE
def IntBoolVal(i, b, v):

    print("an int",i)
    print("a bool", b)
    print("a vector", v)
@pg.PYGLUE
def PrintPretty(s):

    print("print pretty", s)
@pg.PYGLUE
def PrintStringInt(s, i):
    print("a string", s)
    print("an int ",i)    

