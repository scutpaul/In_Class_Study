# -*- coding: utf-8 -*-

import numpy as np

#from scipy.misc import imresize
from imageio import imread as imread

DataName = '/home/paul/SCUT-FBP5500_v2/Images/'
DataType = '/home/paul/SCUT-FBP5500_v2/train_test_files/split_of_60%training and 40%testing/'

def get_data_name(iswhat):
    File_Name = DataType + iswhat+'.txt'
    f = open(File_Name)
    lines = f.readlines()
    X = []
    Y = []
    for line in lines:
        line = line.split('\n')
        X1 = line[0].split(' ')
        X.append(X1[0])
        Y.append(float(X1[1]))
    A = np.array(X)
    B = np.array(Y)
    return (A,B)

def saveDataToNpy():
    X_Train_Name,train_Y = get_data_name('train')
    X_Test_Name,test_Y = get_data_name('test')
    N_train = X_Train_Name.shape[0]
    N_test = X_Test_Name.shape[0]
    train_X = np.array([imread(DataName+X_Train_Name[i]) for i in range(N_train)])
    np.save('train_X',train_X)
    np.save('train_Y',train_Y)
    test_X = np.array([imread(DataName+X_Test_Name[i]) for i in range(N_test)])
    np.save('test_X',test_X)
    np.save('test_Y',test_Y)
    
def loadDataFromNpy():
    train_X = np.load('train_X.npy')
    train_Y = np.load('train_Y.npy')
    test_X = np.load('test_X.npy')
    test_Y = np.load('test_Y.npy')
    return (train_X,train_Y,test_X,test_Y)
    #print(train_X.shape,test_X.shape,train_Y.shape,test_Y.shape)
    #(3300, 350, 350, 3) (2200, 350, 350, 3) (3300,) (2200,)
    
if __name__ == '__main__':
    #saveDataToNpy()
    loadDataFromNpy()