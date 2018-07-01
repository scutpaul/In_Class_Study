import numpy as np 
import math
def f(x):
    return 2/(1+x**2)

def test(x):
    return math.sqrt(x)

def Ti(f,a,b,xita):
    h = (b-a)/2
    T1 = (f(a)+f(b))*h
    n = 1
    T0 = float('inf')
    while abs(T1-T0)>=3*xita:
        T0 = T1
        S = 0
        for k in range(1,n+1):
            S += f(a+(2*k-1)*h/n)
        T1 = T0/2 + S*h/n
        n = 2*n
    return T1

#print(Ti(f,0,1,0.01))
print(Ti(test,1,9,0.01))