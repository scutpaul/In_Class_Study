# -*- coding: utf-8 -*-

import numpy as np
from numpy import linalg as LA

from keras.applications.vgg16 import VGG16
from keras.preprocessing import image
from keras.models import Sequential
from keras.layers import Dropout, Flatten, Dense,Input
from keras import optimizers
from keras.layers import Conv2D, MaxPooling2D,Activation

rootdir  = '/home/paul/Dataset/final_weight/'
input_tensor = Input(shape=(150,150,3))
# 直接使用VGG16提取出的特征
class Myfeature:
    def __init__(self,classname):
        self.input_shape = (150, 150, 3)
        self.classname = classname
        self.weight = rootdir+'VGG'+classname+'.h5'
        self.pooling = 'max'
        self.model = VGG16(weights = self.weight, input_shape = (self.input_shape[0], self.input_shape[1], self.input_shape[2]), pooling = self.pooling, include_top = False)
        self.model.predict(np.zeros((1, 150, 150 , 3)))
        print('load_done: model_feats_'+classname)
        
    def extract_feat(self, img_path):
        img = image.load_img(img_path, target_size=(self.input_shape[0], self.input_shape[1]))
        img = image.img_to_array(img)
        img = img / 255
        img = np.array(img)
        img = np.expand_dims(img, axis=0)
        #img = preprocess_input(img)
        feat = self.model.predict(img)
        norm_feat = feat[0]/LA.norm(feat[0])
        return norm_feat
# 使用3层卷积的分类网络
class simple:
    def __init__(self, classname,classnum):
        self.input_shape = (150, 150, 3)
        self.classname = classname
        self.weight = rootdir + 'first_try_' + classname + '.h5'
        self.model = Sequential()
        self.model.add(Conv2D(32, (3, 3), input_shape = (self.input_shape[0], self.input_shape[1], self.input_shape[2])))
        self.model.add(Activation('relu'))
        self.model.add(MaxPooling2D(pool_size=(2, 2)))

        self.model.add(Conv2D(32, (3, 3)))
        self.model.add(Activation('relu'))
        self.model.add(MaxPooling2D(pool_size=(2, 2)))

        self.model.add(Conv2D(64, (3, 3)))
        self.model.add(Activation('relu'))
        self.model.add(MaxPooling2D(pool_size=(2, 2)))

        self.model.add(Flatten())
        # model.add(Dense(64))
        self.model.add(Activation('relu'))
        # model.add(Dropout(0.5))
        self.model.add(Dense(classnum))
        self.model.add(Activation('softmax'))
        self.model.load_weights(self.weight)
        self.model.predict(np.zeros((1, 150, 150, 3)))
        print('load_done: model_sims_' + classname)

    def extract_feat(self, img_path):
        img = image.load_img(img_path, target_size=(self.input_shape[0], self.input_shape[1]))
        img = image.img_to_array(img)
        img = img / 255
        img = np.array(img)
        img = np.expand_dims(img, axis=0)
        #img = preprocess_input(img)
        feat = self.model.predict(img)
        norm_feat = feat[0] #/ LA.norm(feat[0])
        #print(self.classname,norm_feat)
        return norm_feat
# 使用VGG预训练的分类网络
class MyClassifier:
    def __init__(self,classname,classnum):
        self.input_shape = (150, 150, 3)
        self.classname = classname
        self.classnum = classnum
        self.weight = rootdir + classname+'.h5'
        self.base_model = VGG16(weights='imagenet', include_top=False, input_tensor = input_tensor)
        self.model = Sequential()
        self.model.add(self.base_model)
        self.top_model = Sequential()
        self.top_model.add(Flatten(input_shape=self.base_model.output_shape[1:]))
        self.top_model.add(Dense(self.classnum, activation='softmax'))
        self.model.add(self.top_model)
        self.model.compile(loss='categorical_crossentropy',
                            optimizer=optimizers.SGD(lr=1e-4, momentum=0.9),
                            #optimizer=optimizers.Adam(),
                            metrics=['accuracy'])
        self.model.load_weights(self.weight)
        self.model.predict(np.zeros((1, 150, 150, 3)))
        print('load_done: model_classi_'+classname)

    def extract_feat(self, img_path):
        img = image.load_img(img_path, target_size=(self.input_shape[0], self.input_shape[1]))
        img = image.img_to_array(img)
        img = img / 255
        img = np.array(img)
        img = np.expand_dims(img, axis=0)
        #img = preprocess_input(img)
        feat = self.model.predict(img)
        norm_feat = feat[0] #/ LA.norm(feat[0])
        return norm_feat