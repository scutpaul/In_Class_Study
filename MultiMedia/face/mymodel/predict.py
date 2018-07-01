# -*- coding: utf-8 -*-


import numpy as np
from keras.applications import ResNet50
from keras.preprocessing import image
from keras.applications.vgg16 import preprocess_input
from keras.layers import Flatten, Dense,Input
from keras.models import Sequential
class Predict:
    def __init__(self):
        # weights: 'imagenet'
        # pooling: 'max' or 'avg'
        # input_shape: (width, height, 3), width and height should >= 48
        input_tensor = Input(shape=(350, 350, 3))
        resnet = ResNet50(include_top=False, pooling='max')
        model = Sequential()
        model.add(resnet)
        model.add(Dense(1))
        model.load_weights('../my_model_weights_1.h5')
        self.model = model

        resnet1 = ResNet50(include_top=False, input_tensor=input_tensor)
        model1 = Sequential()
        model1.add(resnet1)
        model1.add(Flatten(input_shape=resnet.output_shape[1:]))
        model1.add(Dense(1))
        model1.load_weights('../model_res_nopool_train_e7.h5')
        self.model1 = model1
        self.model1.predict(np.zeros((1, 350, 350 , 3)))

    '''
    Use vgg16 model to extract features
    Output normalized feature vector
    '''
    def predict(self, img_path,weight):
        img = image.load_img(img_path, target_size=(350, 350))
        img = image.img_to_array(img)
        img = np.expand_dims(img, axis=0)
        img = preprocess_input(img)
        feat = self.model.predict(img)
        feat1 = self.model1.predict(img)
        new_feat = (weight*feat+(2-weight)*feat1)/2
        #norm_feat = feat[0]/LA.norm(feat[0])
        return new_feat

