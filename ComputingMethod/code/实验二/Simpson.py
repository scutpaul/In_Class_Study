import numpy as np 
import math
def f(x):
    return math.sqrt(x)
def simpson(f,a,b,xita):
    fa = f(a)
    fb = f(b)
    h = b-a
    T = h/2*(fa+fb)
    T2 = 0
    eps = xita+1
    n = 1
    x = T
    S1 = 0
    S2 = 0
    while eps>=xita:
        p = 0
        for k in range(n):
            x = a+(k+0.5)*h
            p = p+f(x)
        T2 = (T+h*p)/2
        S2 = (4*T2 - T)/3
        T = T2
        eps = abs(S2-S1)
        S1 = S2
        n *= 2
        h /= 2
    return S2

reslut = simpson(f,1,9,0.01)
print(reslut)
