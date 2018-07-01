# -*- coding: utf-8 -*-

import os
import h5py
import numpy as np


from extract import Myfeature,MyClassifier,simple

database = '/home/paul/Dataset/test'
index = 'featureFinal.h5'
args = {'database':database ,'index':index}

'''
 Returns a list of filenames for all jpg images in a directory. 
'''
def get_imlist(path):
    return [os.path.join(path,f) for f in os.listdir(path) if f.endswith('.jpg')]

'''
 Extract features and index the images
'''

all_class = 7
classnames = ['age','color','design','lingzi','length','style','type']
classnum = [3,10,4,3,2,3,2]
if __name__ == "__main__":

    db = args["database"]
    img_list = get_imlist(db)
    
    print ("--------------------------------------------------")
    print ("         feature extraction starts")
    print ("--------------------------------------------------")

    names = []
    feats = []
    classicifier_result = []
    sims = []

    model_feats = []
    model_Classifiers = []#模型的总集
    model_sim = []
    for i in range(all_class):
        now_class = []
        now_feats = []
        now_sim = []
        feats.append(now_feats)
        sims.append(now_sim)
        classicifier_result.append(now_class)
        model_Classifiers.append(MyClassifier(classnames[i],classnum[i]))
        model_feats.append(Myfeature(classnames[i]))
        model_sim.append(simple(classnames[i],classnum[i]))

    for i, img_path in enumerate(img_list):
        img_name = os.path.split(img_path)[1]

        for j in range(all_class):
            norm_classifier = model_Classifiers[j].extract_feat(img_path)
            norm_feats = model_feats[j].extract_feat(img_path)
            norm_sim = model_sim[j].extract_feat(img_path)
            classicifier_result[j].append(norm_classifier)
            feats[j].append(norm_feats)
            sims[j].append(norm_sim)

        names.append(img_name.encode())
        print ("extracting feature from image No. %d , %d images in total" %((i+1), len(img_list)))

    for i in range(all_class):
        feats[i] = np.array(feats[i])
        classicifier_result[i] = np.array(classicifier_result[i])
        sims[i] = np.array(sims[i])

    # directory for storing extracted features
    output = args["index"]
    
    print ("--------------------------------------------------")
    print ("      writing feature extraction results ...")
    print ("--------------------------------------------------")
    
    h5f = h5py.File(output, 'w')
    h5f.create_dataset('dataset_0', data = names)
    for i in range(all_class):
        sims_dataset = 'dataset_' + str(i + 1) + '_sims'
        h5f.create_dataset(sims_dataset, data=sims[i])
        feats_dataset = 'dataset_'+str(i+1)+'_feats'
        h5f.create_dataset(feats_dataset,data = feats[i])
        classi_dataset = 'dataset_'+str(i+1)+'_classi'
        h5f.create_dataset(classi_dataset,data = classicifier_result[i])
    h5f.close()
