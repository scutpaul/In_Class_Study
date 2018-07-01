# -*- coding: utf-8 -*-

import numpy as np
t= np.loadtxt(r'E:\face\face\imagesearch\mymodel\result_0.txt')
#test= np.loadtxt(r'E:\face\face\imagesearch\mymodel\result_0.txt')
testY = np.loadtxt(r'E:\face\face\imagesearch\mymodel\testY.txt')
test = np.loadtxt(r'E:\face\face\imagesearch\mymodel\result_nopool_0.txt')
#test = np.loadtxt(r'E:\face\face\imagesearch\mymodel\result_cnn_1.txt')

weight = 1.3
t1 = (weight*t+(2-weight)*test)/2
test = t1
print(t.shape)
#t = np.loadtxt('result_0.txt')
#np.savetxt('test.txt',test,fmt='%.4f')
print(np.sum(np.square(t-testY)))
t = t
#PC
def PC(X,Y):
    n = X.shape[0]
    EXY = np.sum(X*Y)/n
    EX = np.sum(X)/n
    EY = np.sum(Y)/n
    EX2 = np.sum(X*X)/n
    EY2 = np.sum(Y*Y)/n
    pc = (EXY-EX*EY)/(np.sqrt(EX2-EX**2)*np.sqrt(EY2-EY**2))
    print(X,Y,EXY,EX,EY,EX2,EY2)
    return pc
print('PC',PC(test,testY))
print('MAE',np.sum(abs(test-testY))/2200)
print('RMSE',np.sqrt(np.sum(np.square(test-testY))/2200))
print('MSE',np.sum(np.square(test-testY))/2200)
'''
PC 0.8644851136748057
MAE 0.2667214069772727
RMSE 0.3484187844159356
MSE 0.1213956493338782'''
'''
PC 0.861955020864345
MAE 0.26709381018181816
RMSE 0.3495798044860791
MSE 0.1222060397045253'''
'''
PC 0.7860953808619425
MAE 0.3339501640909091
RMSE 0.4331215369602693
MSE 0.1875942657788259'''
'''
PC 0.8563790008862189
MAE 0.2855847886363636
RMSE 0.3671731674839884
MSE 0.134816134920225'''
'''
PC 0.8545268882468278
MAE 0.2710514493181818
RMSE 0.3549193230763774
MSE 0.12596772589299396'''
'''
A = np.array([1,1,1])
B = np.array([2,2,2])
C = np.hstack((A,B))
print(A)
print(B)
print(C)'''



import matplotlib.pyplot as plt

plt.figure("lena")

plt.hist(testY, bins=256, normed=1,facecolor='r',edgecolor='r',hold=1)
plt.hist(test, bins=256, normed=1,facecolor='g',edgecolor='g',hold=1)
#plt.hist(t, bins=256, normed=1,facecolor='b',edgecolor='b',hold=1)
plt.show()