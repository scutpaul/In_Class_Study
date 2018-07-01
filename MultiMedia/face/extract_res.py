# -*- coding: utf-8 -*-

import numpy as np
from numpy import linalg as LA
from keras.applications import ResNet50
from keras.applications.vgg16 import VGG16
from keras.preprocessing import image
from keras.applications.vgg16 import preprocess_input

class RESNet:
    def __init__(self):
        self.input_shape = (350, 350, 3)
        self.model = ResNet50(include_top=False, pooling='avg')
        self.model.predict(np.zeros((1, 350, 350 , 3)))

    def extract_feat(self, img_path):
        img = image.load_img(img_path, target_size=(self.input_shape[0], self.input_shape[1]))
        img = image.img_to_array(img)
        img = np.expand_dims(img, axis=0)
        img = preprocess_input(img)
        feat = self.model.predict(img)
        norm_feat = feat[0]/LA.norm(feat[0])
        return norm_feat
