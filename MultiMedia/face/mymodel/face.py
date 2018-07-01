
"""
Created on Thu Apr 26 23:05:48 2018

@author: paul
"""
from scipy.misc import imresize
from imageio import imread as imread
import os
from keras.applications import ResNet50
from keras.models import Sequential
from keras.layers import Dense
from keras.optimizers import Adam
import numpy as np
from matplotlib import pyplot as plt
import sys # read txt
import tensorflow as tf
from keras.layers import Dropout, Flatten, Dense,Input
from keras.models import load_model 

tf.device('/gpu:0') 
# DataName = '/home/paul/SCUT-FBP5500_v2/Images/'
# DataType = '/home/paul/SCUT-FBP5500_v2/train_test_files/split_of_60%training and 40%testing/'

# def imshow_noax(img, normalize=True):
#     """ Tiny helper to show images as uint8 and remove axis labels """
#     if normalize:
#         img_max, img_min = np.max(img), np.min(img)
#         img = 255.0 * (img - img_min) / (img_max - img_min)
#     plt.imshow(img.astype('uint8'))
#     plt.gca().axis('off')
    
# def test():
#     F = np.array([ filename for filename in os.listdir(DataName)])
#     print(F[0:5])
#     test = imread(DataName+F[86])
#     imshow_noax(test, normalize=False)
#     plt.show()

def get_data():
    train_X = np.load('train_X.npy')
    train_Y = np.load('train_Y.npy')
    test_X = np.load('test_X.npy')
    test_Y = np.load('test_Y.npy')
    #train_X = []
    #train_Y = []
    return (train_X,train_Y,test_X,test_Y)
    #X_Train_Name = np.array(for filename in os.listdir(DataType)])
    
    
def main():
    input_tensor = Input(shape=(350, 350, 3))
    resnet = ResNet50(include_top=False, input_tensor=input_tensor)#模型二的训练：resnet = ResNet50(include_top=False，pool ='max')
    model = Sequential()
    model.add(resnet)
    model.add(Flatten(input_shape=resnet.output_shape[1:]))
    model.add(Dense(1))
    print(model.summary())
    # model.layers[0].trainable = False

    # model.compile(loss = 'mean_squared_error',optimizer=Adam())
    #model.load_weights('my_model_weights_1.h5')
    #model.load_weights('model_res_nopool_train_e7.h5')

    train_X, train_Y, test_X, test_Y = get_data()
    model.compile(loss='mean_squared_error', optimizer=Adam(), metrics=['accuracy'])
    model.fit(batch_size=32,
              x=train_X, y=train_Y,
              epochs=3,
              validation_data=(test_X, test_Y))
    model.save_weights('model_train_e3.h5')
    #model.save('model.h5')
    #model.save_weights('my_model_weights_2.h5')

    test_X_Result = np.array([])
    for i in range(220):
        temp = model.predict(test_X[i*10:(i+1)*10])
        temp = temp.reshape(1,-1)
        if i == 0:
            test_X_Result = temp
        else:
            test_X_Result = np.hstack((test_X_Result,temp))

    #print(test_Y)

    resnet.save_weights('res.h5')
    np.savetxt('result_nopool_0.txt',test_X_Result,fmt='%.6f')
    #print(np.sum(np.square(test_X_Result-test_Y)))
    #pass



if __name__ == '__main__':
    main()
    #test()
    #get_data()