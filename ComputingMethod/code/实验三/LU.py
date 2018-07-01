#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Apr 23 15:55:53 2018

@author: paul
"""

import numpy as np 
def check(x,xita):
    if abs(x) <= xita:
        print("failed")
        exit(0)

def LU(A,b,xita):
    n = A.shape[0]
    L = np.zeros([n,n])
    U = np.zeros([n,n])
    for k in range(n):
        for j in range(k,n):
            U[k,j] = A[k,j] - np.sum(L[k,:k]*U[:k,j].T)
        check(U[k,k],xita)
        for i in range(k+1,n):
            L[i,k] = (A[i,k] - np.sum(L[i,:k]*U[:k,k].T))/U[k,k]
            L[k,k] = 1
    L[n-1,n-1] = 1
    Y = np.zeros(n)
    Y[0] = b[0]
    for i in range(1,n):
        Y[i] = b[i] - np.sum(L[i,:i]*Y[:i])
    X = np.zeros(n)
    X[n-1] = Y[n-1]/U[n-1,n-1]
    for i in range(n-2,-1,-1):
        X[i] = (Y[i] - np.sum(U[i,(i+1):(n)]*X[(i+1):(n)]))/U[i,i]
    print(L)
    print("----------")
    print(U)
    print("----------")
    print(Y)
    print("----------")
    print(X)
    return X



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
    LU(A,b,xita)
# A = np.array([[2.0,3.0,5.0],[3.0,4.0,7.0],[1.0,3.0,3.0]])
# b = np.array([5.0,6.0,5.0])
# Guass(A,b,0.001)
# A = np.array([[1,2,1],[2,2,3],[-1,-3,0]])
# b = np.array([0,3,2])
# LU(A,b,0.01)
if __name__ == '__main__':
    main()