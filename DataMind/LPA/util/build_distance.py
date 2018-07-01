# -*- coding: utf-8 -*-

import numpy as np

def build_distance(AMatrix):
    Num_node = AMatrix.shape[0]
    max_top = 6
    one = dict()
    last = dict()
    this = dict()
    update = dict()
    #changed = 0
    for i in range(Num_node):
        one[i] = set()
        last[i] = set()
        this[i] = set()
        update[i] = set()
        for j in range(Num_node):
            if (AMatrix[i,j,1]==1):
                one[i].add(j)
                last[i].add(j)
            elif (AMatrix[i,j,1]>1):
                this[i].add(j)
    for k in range(1,max_top):
        print('update:k,',k)
        for i in range(Num_node):
            if(len(this[i]) == 0):
                continue
            for ilast in last[i]:
                temp_update = one[ilast]&this[i]
                if(len(temp_update)!=0):
                    update[i] = temp_update|update[i]
                    for updatei in temp_update:
                        ##changed += 1
                        new_distance = AMatrix[ilast,updatei,0] + AMatrix[i,ilast,0]
                        if(AMatrix[i,updatei,0]>new_distance):
                            AMatrix[i,updatei,0] = new_distance
                            AMatrix[updatei,i,0] = new_distance
                            AMatrix[updatei,i,1] = k+1
                            AMatrix[i,updatei,1] = k+1
        for i in range(Num_node):
            if(len(this[i]) == 0):
                continue
            last[i] = update[i]
            this[i] = this[i]-update[i]
            update[i] = set()
    #print(changed) # all = 5928909 #changed = 622398
    saveD = AMatrix[:,:,0]
    #np.savetxt('Distance.txt',saveD,fmt="%d")
    return saveD
    
              

def floyd(AMatrix):
    Num_node = AMatrix.shape[0]
    inf = float("inf")
    for k in range(Num_node):
        for i in range(Num_node):
            for j in range(Num_node):
                if (AMatrix[i,k]<inf and AMatrix[k,j]<inf and
                    AMatrix[i,j]>(AMatrix[i,k]+AMatrix[k,j])):
                    AMatrix[i,j] = AMatrix[i,k]+AMatrix[k,j]
    for i in range(Num_node):
        for j in range(Num_node):
            if(i>j):
                AMatrix[i,j] = AMatrix[j,i]
    return AMatrix 

