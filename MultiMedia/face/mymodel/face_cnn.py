#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from keras.models import Sequential
from keras.layers import Conv2D, MaxPooling2D,Activation
from keras.layers import  Dropout, Flatten, Dense
from keras import backend as K
from keras import optimizers
import numpy as np
from matplotlib import pyplot as plt
import sys # read txt
import tensorflow as tf


tf.device('/gpu:0') 

def get_data():
    #train_X = np.load('train_X.npy')
    #train_Y = np.load('train_Y.npy')
    train_X =[]
    train_Y =[]
    test_X = np.load('test_X.npy')
    test_Y = np.load('test_Y.npy')
    return (train_X,train_Y, test_X,test_Y)

img_width, img_height = 350,350
def main():
    if K.image_data_format() == 'channels_first':
        input_shape = (3, img_width, img_height)
    else:
        input_shape = (img_width, img_height, 3)
    model = Sequential()
    model.add(Conv2D(32, (3, 3), input_shape=input_shape))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Conv2D(32, (3, 3)))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Conv2D(64, (3, 3)))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Flatten())
    model.add(Dense(64))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(1))

    model.compile(loss='mean_squared_error',
                  optimizer=optimizers.SGD(lr=1e-5, momentum=0.9),
                  #optimizer=optimizers.Adam(),
                  metrics=['accuracy'])
    model.load_weights('face_cnn_epochs_2.h5')
    train_X, train_Y ,test_X,test_Y= get_data()
    
    # model.fit(batch_size=16,
    #           x=train_X, y=train_Y,
    #           epochs=3)
    #           #validation_data=(test_X, test_Y))
    # model.save_weights('face_cnn_epochs_2.h5')

    test_X_Result = np.array([])
    for i in range(220):
        temp = model.predict(test_X[i*10:(i+1)*10])
        temp = temp.reshape(1,-1)
        if i == 0:
            test_X_Result = temp
        else:
            test_X_Result = np.hstack((test_X_Result,temp))
    np.savetxt('result_cnn_1.txt', test_X_Result, fmt='%.6f')


if __name__ == "__main__":
    main()