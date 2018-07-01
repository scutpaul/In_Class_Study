# -*- coding: utf-8 -*-

import numpy as np
import random


class K_mean:
    def __init__(self, Num_class, DistanceMatrix, Num_node, max_iter=500):
        self.Num_class = Num_class
        self.DM = DistanceMatrix
        self.Num_node = Num_node
        self.max_iter = max_iter

    def generate_center(self, k):
        gen = set()
        for i in range(k):
            temp_center = random.randint(0, self.Num_node - 1)
            if temp_center in gen:
                i -= 1
            else:
                gen.add(temp_center)
        return gen

    def train(self):
        self.center = self.generate_center(self.Num_class)
        changed = True
        Cluster = dict()
        for iter in range(self.max_iter):
            if changed == False:
                break
            print(iter,self.center)
            changed = False
            Cluster = dict()
            for i in self.center:
                Cluster[i] = []
            # all data point add to cluster
            for i in range(self.Num_node):
                min_dis = 1998
                min_index = -1
                for j in self.center:
                    if i != j and self.DM[i, j] < min_dis:
                        min_dis = self.DM[i, j]
                        min_index = j
                    elif i == j:
                        min_index = j
                        break
                Cluster[min_index].append(i)

            # find new center
            temp_set = set()
            for i in self.center:
                decision = np.zeros(self.Num_node)
                for node in Cluster[i]:
                    decision[node] += np.sum(self.DM[node,Cluster[i]])
                new_center = np.argmin(decision[Cluster[i]])
                new_center = Cluster[i][new_center]
                if new_center != i:
                    changed = True
                temp_set.add(new_center)
            self.center = temp_set

        self.predict = np.zeros((self.Num_node))
        for i in Cluster:
            for j in Cluster[i]:
                self.predict[j] = i

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


