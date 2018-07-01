# -*- coding: utf-8 -*-
import numpy as np
root = 'D:/本科/大三下/数据挖掘/LPA/dataset'
path = root+'/email-Eu-core.txt'
path2 = root+ "/email-Eu-core-department-labels.txt"

def getlabel():
    c = np.loadtxt(path2,dtype=int)
    return c

def read_edge1():
    All_point = np.loadtxt(path2,dtype=int)
    Num_node = All_point.shape[0]
    Adjacency_matrix = np.zeros((Num_node,Num_node,2))
    Data_edge = np.loadtxt(path,dtype=int)
    for i in range(Data_edge.shape[0]):
        a = Data_edge[i,0]
        b = Data_edge[i,1]
        if (a == b):
            continue
        if(Adjacency_matrix[b,a,1] == 1):
            #Adjacency_matrix[b,a,0] = 1/2 * Adjacency_matrix[b,a,0]
            #Adjacency_matrix[a,b,0] = 1/2 * Adjacency_matrix[a,b,0]
            continue
        Adjacency_matrix[a,b,0] = 1
        Adjacency_matrix[b,a,0] = 1
        Adjacency_matrix[a,b,1] = 1
        Adjacency_matrix[b,a,1] = 1
    for i in range(Num_node):
        for j in range(Num_node):
            if (i==j):
                Adjacency_matrix[i,j,0] = 0
                Adjacency_matrix[i,j,1] = 0
            else:
                if (Adjacency_matrix[i,j,1]==1):
                    continue
                else:
                    Adjacency_matrix[i,j,1] = 10
                    Adjacency_matrix[i,j,0] = 1997
    return Adjacency_matrix
