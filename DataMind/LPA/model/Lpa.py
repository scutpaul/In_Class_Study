# -*- coding: utf-8 -*-

import numpy as np

class Lpa():
    '''
    DM: Distance Matrix
    label: labeled data

    PST:Probability transfer matrix 
    L: num of labeled
    C: num of class
    U: num of unlabeled

    F: algorithm

    '''
    def __init__(self, DM, label, Num_class, threshold = 0.1, sigma=0.33, max_epochs=500):
        self.DM = DM
        self.Num_node = DM.shape[0]

        self.label = label
        self.sigma = sigma
        self.L = label.shape[0]
        self.U = self.Num_node - self.L
        self.max_epochs = max_epochs
        self.threshold = threshold
        self.changed = float("inf")     
        self.C = Num_class
        self.F = np.zeros((self.Num_node,self.C))

        self.getSimilarity()
        self.getPTS()
        self.setlabel()

    def getSimilarity(self):
        self.SM = np.exp(-1*np.multiply(self.DM,self.DM)/self.sigma**2)
    def getPTS(self):
        SM_sum = np.sum(self.SM,axis =1)
        SM_sum = SM_sum.reshape(-1,1)
        self.PTS = self.SM/SM_sum

    
    def setlabel(self):
        for i in range(self.L):
            now = self.label[i,0]
            classes = self.label[i,1]
            for j in range(self.C):
                if(j == classes):
                    self.F[now,j] = 1
                else:
                    self.F[now,j] = 0


    def Process(self):
        old = self.F
        self.F = np.dot(self.PTS,self.F)

        self.setlabel()
        fab = np.fabs(old-self.F)
        changed = np.sum(fab)
        return changed
    
    def train(self):
        for i in range(self.max_epochs):
            if(i%100==1):
                print(i,self.changed)
            if(self.changed<self.threshold):
                break
            self.changed = self.Process()
        return self.fromFtolabel()
    
    def fromFtolabel(self):
        F = self.F
        index = np.argmax(F,axis = 1)
        self.predict = index.reshape(-1,1)
        return self.predict
        
    def RandIndex(self,truelabel):
        A=B=C=D=0
        for i in range(self.Num_node):
            for j in range(self.Num_node):
                if i!=j:
                    if self.predict[i] == self.predict[j]:
                        if truelabel[i] == truelabel[j]:
                            A+=1
                        else:
                            B+=1
                    else:
                        if truelabel[i] == truelabel[j]:
                            C+=1
                        else:
                            D+=1
        return (A+D)/(A+B+C+D)
        