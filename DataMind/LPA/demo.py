# -*- coding: utf-8 -*-
import numpy as np
from model.Lpa import Lpa
from model.k_means import K_mean
from util.build_distance import build_distance
from util.getdata import load_distance,getlabel,read_edge1
import random

#----超参数
pcs = [0.05,0.1,0.2,0.5]#labeled percent
sigma = 0.33#LPA -sigma
threshold = 0.1#LPA -stop
max_epochs = 200#LPA -iter
iterior = 5# Test times
Num_classs = [20,30,42]#Kmeans

def main(DMatrix,gLabel):
    truelabel = gLabel[:, 1]
    Num_class = truelabel.shape[0]

    Predict_result = []
    for pc in pcs:
        for iter in range(iterior):
            print("iter",iter)
            aLabel = []
            for i in gLabel:
                if random.random() < pc:
                    aLabel.append(i)

            Label = np.array(aLabel)

            lpa = Lpa(DMatrix, Label, Num_class=Num_class, sigma=sigma, threshold=threshold, max_epochs=max_epochs)
            Predict_label = lpa.train()

            predict_index = lpa.RandIndex(truelabel)
            Predict_result.append(predict_index)
    save_result = np.array(Predict_result)
    np.savetxt('Result.txt',save_result,fmt='%.4f')


def demo_kmeans(DMatrix,gLabel):
    truelabel = gLabel[:, 1]
    Num_node = DMatrix.shape[0]

    Predict_result = []
    for Num_class in Num_classs:
        print("Num_class",Num_class)
        for iter in range(iterior):
            print("iter",iter)
            kmeans = K_mean(Num_class=Num_class, DistanceMatrix=DMatrix, Num_node=Num_node, max_iter=max_epochs)
            kmeans.train()
            Predict_index = kmeans.RandIndex(truelabel)
            Predict_result.append(Predict_index)
            print(Predict_index)
    save_result = np.array(Predict_result)
    np.savetxt("Kmeans1.txt",save_result,fmt='%.4f')

if __name__ == "__main__":
    AMatrix = read_edge1()
    DMatrix = build_distance(AMatrix)
    gLabel = getlabel()
    demo_kmeans(DMatrix,gLabel)
    main(DMatrix,gLabel)


