#!/usr/bin/env python3
# -*- coding: utf-8 -*-
""" 
Created on Mon Apr 23 22:21:30 2018

@author: paul
"""

import numpy as np
def check(x,xita):
    if abs(x) <= xita:
        print("failed")
        exit(0)

def Guass(A,b,xita):
    #消元
    n = A.shape[0]
    for k in range(n-1):
        #选主元
        m = np.argmax(A[k:n,k])
        #换顺序
        if m != 0:
            A[[k,m+k],:] = A[[m+k,k],:]
            b[[k,m+k]] = b[[m+k,k]]
        check(A[k,k],xita)
        for i in range(k+1,n):
            temp = A[i,k]/A[k,k]
            b[i] -= b[k]*temp
            A[i,(i-1):n] -= temp * A[k,(i-1):n]

    # 回代
    check(A[n - 1, n - 1], xita)
    X = np.zeros(n)
    X[n-1] = b[n-1]/A[n-1,n-1]
    for i in range(n-2,-1,-1):
        X[i] = (b[i] - np.sum(A[i,(i+1):(n)]*X[(i+1):(n)]))/A[i,i]
    print(X)



def main():
    n = input("输入n")
    n = int(n)
    A = []
    for i in range(n):
        temp = input('输入X'+str(i+1)+'行 空格空开，如：1 1 1 1')
        temp = temp.split(' ')
        for j in range(n):
            temp[j] = float(temp[j])
        A.append(temp)
    A = np.array(A)
    b = input('输入b:')
    b = b.split(' ')
    for i in range(n):
        b[i] = float(b[i])
    b = np.array(b)
    xita = input('输入误差：')
    xita = float(xita)
    Guass(A,b,xita)
# A = np.array([[2.0,3.0,5.0],[3.0,4.0,7.0],[1.0,3.0,3.0]])
# b = np.array([5.0,6.0,5.0])
# Guass(A,b,0.001)

if __name__ == '__main__':
    main()