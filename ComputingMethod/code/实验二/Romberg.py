#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Apr 24 18:30:38 2018

@author: paul
"""
import numpy as np
import math

def Romberg(a,b,xita,f):
    T = []
    T0 = (b-a)/2*(f(a)+f(b))
    T.append([T0])
    k = 0
    now_xita = 999999
    while now_xita >= xita:
        k += 1
        Tx = []
        T.append(Tx)
        Tx0 = 1/2*(T[k-1][0] + (b-a)/(2**(k-1))*sum([f(a+(2*i-1)*(b-a)/2**k) for i in range(1,2**(k-1)+1)]))
        Tx.append(Tx0)
        for m in range(1,k+1):
            Txm = (4**m*T[k][m-1]-T[k-1][m-1])/(4**m-1)
            Tx.append(Txm)
        now_xita = abs(T[k][k]-T[k-1][k-1])
    return T[k][k],k
def f(x):
    return math.sqrt(x)

#Final = Romberg(1,9,0.00005)
def test(x):
    return 4/(1+x**2)

Final ,k1= Romberg(0,1,0.01,test)
Final2 ,k2= Romberg(1,9,0.01,f)
print(Final2,k2)

#print(2/3*(9**(3/2)-1))
'''
def g(x):
    return x**2*1
A =sum([g(i) for i in range(3)])
print(A)

A = []
B = []
C = []
A.append(B)
A.append(C)
C.append(2)
B.append(1)
'''