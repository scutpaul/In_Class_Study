# -*- coding: utf-8 -*-

from extract_res import RESNet
import Cut
from predict import Predict
import numpy as np
import h5py
from reshapeimage import resizeImg
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import argparse
import os


index = 'feats.h5'
result = 'Images'
copydir = '/search'
args = {'index': index, 'result': result, 'copydir':copydir}

# read in indexed images' feature vectors and corresponding image names
h5f = h5py.File(args["index"],'r')
feats = h5f['dataset_1'][:]
imgNames = h5f['dataset_2'][:]
h5f.close()
        
print ("--------------------------------------------------")
print ("               searching starts")
print ("--------------------------------------------------")

model = RESNet()
predict = Predict()
def query(queryDir):
# read and show query image
    queryImg = mpimg.imread(queryDir)
    plt.title("Query Image")
    plt.imshow(queryImg)
    plt.show()
    print('颜值:',predict.predict(queryDir,weight))


    # extract query image's feature, compute simlarity score and sort
    queryVec = model.extract_feat(queryDir)
    scores = np.dot(queryVec, feats.T)
    rank_ID = np.argsort(scores)[::-1]
    rank_score = scores[rank_ID]
    #print rank_ID
    #print rank_score


    # number of top retrieved images to show
    maxres = 3
    imlist = [imgNames[index] for i,index in enumerate(rank_ID[0:maxres])]
    print ("top %d images in order are: " %maxres, imlist)


    # show top #maxres retrieved result one by one
    for i,im in enumerate(imlist):
        im = im.decode()
        image = mpimg.imread(args["result"]+"/"+str(im))
        plt.title("search output %d" %(i+1))
        plt.imshow(image)
        plt.show()
rootdir = 'Images/'
testdir = 'test/'
weight = 1
while 1:
    x = input('img_index')
    if x == 'exit':
        break
    if x == 'change':
        weight_1 = input('weight')
        weight = float(weight_1)
    else:
        path = testdir + x + '.jpg'
        if os.path.exists(path):
            pass
        else:
            continue
        path = Cut.cut(path)
        if path == 'Fail to cut':
            continue
        image = mpimg.imread(path)
        plt.imshow(image)
        plt.show()
        path = resizeImg(path,350)
        query(path)
