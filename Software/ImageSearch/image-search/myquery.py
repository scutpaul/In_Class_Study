# -*- coding: utf-8 -*-

from extract import Myfeature,MyClassifier,simple
import Cut
import numpy as np
import h5py

import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import argparse

# age color design lingzi length style type
weight = [1, 2, 1, 1, 1, 1, 2]
model_weight = [0.5,1,2]

testdir = '/home/paul/Dataset/search/'
index = 'featureFinal.h5'
result = '/home/paul/Dataset/test'
copydir = '/home/paul/Dataset/search'
args = {'index': index, 'result': result, 'copydir':copydir}

age = {'child':0,'girl':1,'orded':2}
color = {'black':0,'blue':1,'brown':2,'green':3,'orange':4,'pink':5,'purple':6,'red':7,'white':8,'yellow':9}
design = {'floral':0,'printing':1,'pure':2,'stripe':3}
lingzi = {'fangling':0,'vling':1,'yuanling':2}
length = {'long':0,'short':1}
style = {'catoon':0,'korea':1,'sport':2}
type = {'loose':0,'tight':1}



smallclass = [age,color,design,lingzi,length,style,type]

all_class = 7
classnames = ['age', 'color', 'design', 'lingzi', 'length', 'style', 'type']
classnum = [3, 10, 4, 3, 2, 3, 2]

# read in indexed images' feeature vectors and corresponding image names
h5f = h5py.File(args["index"], 'r')
imgNames = h5f['dataset_0'][:]

feats = []
sims = []
classifier_result = []
#显示的图片数
maxres = 5

for i in range(all_class):
    now_feats_dataset = 'dataset_' + str(i + 1) + '_feats'
    now_classi_dataset = 'dataset_' + str(i + 1) + '_classi'
    now_sims_dataset = 'dataset_' + str(i + 1) + '_sims'
    now_feats = h5f[now_feats_dataset][:]
    now_sims = h5f[now_sims_dataset][:]
    now_classi = h5f[now_classi_dataset][:]
    feats.append(now_feats)
    sims.append(now_sims)
    classifier_result.append(now_classi)
h5f.close()

# init VGGNet16 model
model_feats = []
model_sims = []
model_Classifiers = []  # 模型的总集
for i in range(all_class):
    model_Classifiers.append(MyClassifier(classnames[i], classnum[i]))
    model_feats.append(Myfeature(classnames[i]))
    model_sims.append(simple(classnames[i],classnum[i]))

print ("--------------------------------------------------")
print ("               searching starts")
print ("--------------------------------------------------")

def cal_cos(A,B):
    n = int(A.shape[0])
    ans = np.zeros(n)
    for i in range(n):
        if np.sum(B) == 0:
            ans[i] = 0
        else:
            #ans[i] = np.linalg.norm((A[i] - B),ord = 1)
            ans[i] = np.dot(A[i],B)/(np.linalg.norm(A[i])*(np.linalg.norm(B)))
    return ans
import os,shutil
def mycopyfile(srcfile,dstfile):
    if not os.path.isfile(srcfile):
        print ("%s not exist!"%(srcfile))
    else:
        fpath,fname=os.path.split(dstfile)    #分离文件名和路径
        if not os.path.exists(fpath):
            os.makedirs(fpath)                #创建路径
        shutil.copyfile(srcfile,dstfile)      #复制文件
        print ("copy %s -> %s"%( srcfile,dstfile))

def get_key (dict, value):
    return [k for k, v in dict.items() if v == value]
def query(queryDir):
    # read and show query image
    queryImg = mpimg.imread(queryDir)
    plt.title("Query Image")
    plt.imshow(queryImg)
    plt.show()

    # extract query image's feature, compute simlarity score and sort
    queryVec_classi = []
    queryVec_feats = []
    queryVec_sims = []
    for j in range(all_class):
        norm_classifier = model_Classifiers[j].extract_feat(queryDir)
        norm_feats = model_feats[j].extract_feat(queryDir)
        norm_sims = model_sims[j].extract_feat(queryDir)
        queryVec_classi.append(norm_classifier)
        queryVec_feats.append(norm_feats)
        queryVec_sims.append(norm_sims)

    scores_feats = None
    scores_classi = None
    scores_sims = None
    #n = sims[0].shape[0]
    #scores_all = np.zeros(n)
    for i in range(all_class):
        if i == 0:
            scores_feats = np.dot(queryVec_feats[i],feats[i].T) * weight[i]
            scores_classi = cal_cos(classifier_result[i], queryVec_classi[i])*weight[i]
            scores_sims = cal_cos(sims[i],queryVec_sims[i])*weight[i]
        else:
            scores_feats += np.dot(queryVec_feats[i], feats[i].T) * weight[i]
            scores_classi += cal_cos(classifier_result[i], queryVec_classi[i]) * weight[i]
            scores_sims += cal_cos(sims[i], queryVec_sims[i]) * weight[i]
    #print  (queryVec_classi)
    scores_all = model_weight[0]*scores_sims + model_weight[1]*scores_classi + model_weight[2] * scores_feats

    rank_ID_classi = np.argsort(scores_classi)[::-1]
    rank_ID_feats = np.argsort(scores_feats)[::-1]
    rank_ID_sims = np.argsort(scores_sims)[::-1]
    rank_ID_all = np.argsort(scores_all)[::-1]
    rank_score_all = scores_all[rank_ID_all]
    rank_score_feats = scores_feats[rank_ID_feats]
    rank_score_sims = scores_feats[rank_ID_sims]
    rank_score_classi = scores_classi[rank_ID_classi]
    # print(rank_score_all[:100])
    # print (rank_score_sims[:100])
    # print (rank_score_classi[:100])
    # print (rank_score_feats[:100])
    # print rank_ID
    # print rank_score

    # number of top retrieved images to show
    imlist_all = [imgNames[index] for i, index in enumerate(rank_ID_all[0:maxres])]
    imlist = [imgNames[index] for i, index in enumerate(rank_ID_sims[0:maxres])]
    imlist0 = [imgNames[index] for i, index in enumerate(rank_ID_classi[0:maxres])]
    imlist1 = [imgNames[index] for i, index in enumerate(rank_ID_feats[0:maxres])]
    print ("top %d images in order sims are: " % maxres, imlist)
    print("top %d images in order classi are: " % maxres, imlist0)
    print("top %d images in order feats are: " % maxres, imlist1)

    # show top #maxres retrieved result one by one
    for i, im in enumerate(imlist_all):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        fpath, fname = os.path.split(queryDir)
        fname = fname.split('.')
        desdir = args["copydir"] + "/"+fname[0]+"/all"
        despath = desdir+'/'+str(im)
        mycopyfile(srcpath,despath)


        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
    for i, im in enumerate(imlist):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        fpath, fname = os.path.split(queryDir)
        fname = fname.split('.')
        desdir = args["copydir"] + "/"+fname[0]+"/simple"
        despath = desdir+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
    for i, im in enumerate(imlist0):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        fpath, fname = os.path.split(queryDir)
        fname = fname.split('.')
        desdir = args["copydir"] + "/"+fname[0]+"/classi"
        despath = desdir+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
    for i, im in enumerate(imlist1):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        fpath, fname = os.path.split(queryDir)
        fname = fname.split('.')
        desdir = args["copydir"] + "/"+fname[0]+"/feats"
        despath = desdir+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
def querybyvec(queryVec_classi,searchname):
    scores_classi = None
    scores_sims = None
    queryVec_sims = queryVec_classi
    for i in range(all_class):
        if i == 0:
            scores_sims = cal_cos(sims[i], queryVec_sims[i]) * weight[i]
            scores_classi = cal_cos(classifier_result[i], queryVec_classi[i])*weight[i]
        else:
            scores_sims += cal_cos(sims[i], queryVec_sims[i]) * weight[i]
            scores_classi += cal_cos(classifier_result[i], queryVec_classi[i]) * weight[i]
    #print  (queryVec_classi)
    scores_all = model_weight[0] * scores_sims + model_weight[1] * scores_classi

    rank_ID_classi = np.argsort(scores_classi)[::-1]
    rank_score_classi = scores_classi[rank_ID_classi]
    rank_ID_all = np.argsort(scores_all)[::-1]
    rank_score_all = scores_all[rank_ID_all]
    rank_ID_sims = np.argsort(scores_sims)[::-1]
    rank_score_sims = scores_sims[rank_ID_sims]
    print (rank_score_sims[:100])
    print (rank_score_classi[:100])
    #print (rank_score_sims)
    # print rank_ID
    # print rank_score

    # number of top retrieved images to show
    imlist_0 = [imgNames[index] for i, index in enumerate(rank_ID_all[0:maxres])]
    imlist = [imgNames[index] for i, index in enumerate(rank_ID_sims[0:maxres])]
    imlist0 = [imgNames[index] for i, index in enumerate(rank_ID_classi[0:maxres])]
    print ("top %d images in order sims are: " % maxres, imlist)
    print("top %d images in order classi are: " % maxres, imlist0)


    # show top #maxres retrieved result one by one
    for i, im in enumerate(imlist_0):
        im = im.decode()
        #print(im)

        srcpath = args["result"] + "/" + str(im)
        temp_search_name = ''
        for j in searchname:
            print(j[0])
            temp_search_name = temp_search_name +'_'+ j[0]
        despath = args["copydir"]+"/"+temp_search_name+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
    for i, im in enumerate(imlist):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        temp_search_name = ''
        for j in searchname:
            print(j[0])
            temp_search_name = temp_search_name +'_'+ j[0]
        despath = args["copydir"]+"/"+temp_search_name+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        # plt.title("search output %d" % (i + 1))
        # plt.imshow(image)
        # plt.show()
    for i, im in enumerate(imlist0):
        im = im.decode()

        srcpath = args["result"] + "/" + str(im)
        temp_search_name = ''
        for j in searchname:
            print(j[0])
            temp_search_name = temp_search_name +'_'+ j[0]
        despath = args["copydir"]+"/"+temp_search_name+'/'+str(im)
        mycopyfile(srcpath,despath)

        image = mpimg.imread(args["result"] + "/" + str(im))
        #plt.title("search output %d" % (i + 1))
        #plt.imshow(image)
        #plt.show()

while 1:

    x = input('img_index')
    if x == 'exit':
        break
    if x == 'change':
        input_change_type = input('type: weight, maxres, model')
        if  input_change_type == 'weight':
            new_weight = input('weight: 1 2 3 4 5 6 7').split(' ')
            for i in range(all_class):
                weight[i] = int(new_weight[i])
        elif input_change_type == 'maxres':
            new_maxres = input('maxres: 0-n')
            if int(new_maxres)>0:
                maxres = new_maxres
        elif input_change_type == 'model':
            new_model = input('model: 1 1 1').split(' ')
            for i in range(3):
                model_weight[i] = int(new_model[i])
        else:
            break
    if x == 'wenzi':
        rank_scores = []
        for i in range(all_class):
            rank_scores.append(np.zeros(len(smallclass[i])))
            input_wenzi_search = []
        while 1:
            input_wenzi = input('wenzi: 0:age,1:color,2:design,3:lingzi,4:length,5:style,6:type,7:query')
            if input_wenzi == 'exit':
                break
            elif int(input_wenzi) <7 and int(input_wenzi)>-1:
                input_wenzi_type = int(input_wenzi)
                print (smallclass[input_wenzi_type])
                input_wenzi_key = input('in:')
                input_wenzi_key = int(input_wenzi_key)
                if input_wenzi_key>-1 and input_wenzi_key<len(smallclass[input_wenzi_type]):
                    rank_scores[input_wenzi_type][input_wenzi_key] =1
                    temp_search = get_key(smallclass[input_wenzi_type],input_wenzi_key)
                    input_wenzi_search.append(temp_search)
                else:
                    print('input_key_error')
            elif int(input_wenzi)==7:
                querybyvec(rank_scores,input_wenzi_search)
                break
            else:
                print('input_wenzi_error')
        continue
    else:
        path = testdir + x + '.jpg'
        path = Cut.cut(path)
        if path == 'Fail to cut':
            continue
        image = mpimg.imread(path)
        plt.imshow(image)
        plt.show()
        query(path)
