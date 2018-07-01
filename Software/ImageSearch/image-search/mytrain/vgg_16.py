from keras import applications
from keras.preprocessing.image import ImageDataGenerator
from keras import optimizers
from keras.models import Sequential,Model
from keras.layers import Dropout, Flatten, Dense,Input
from keras import backend as K
# path to the model weights files.
weights_path = '/home/paul/Dataset/weight/vgg16_weights_tx_style_test.h5'
#top_model_weights_path = 'bottleneck_fc_model_lz.h5'
# dimensions of our images.
img_width, img_height = 150, 150

#train_data_dir = 'data/train'
train_data_dir = '/home/paul/Dataset/Tx_class/new/style/train'
#validation_data_dir = 'data/validation'
validation_data_dir = '/home/paul/Dataset/Tx_class/new/style/validation'
test_data_dir = '/home/paul/Dataset/Tx_class/test'
nb_train_samples = 4314#2458#3302
nb_validation_samples = 1440#800#502
epochs = 1
batch_size = 8

#fix
input_tensor = Input(shape=(150,150,3))
model = Sequential()
# build the VGG16 network
base_model = applications.VGG16(weights='imagenet', include_top=False, input_tensor = input_tensor)
print('Model loaded.')
model.add(base_model)
# build a classifier model to put on top of the convolutional model
top_model = Sequential()
top_model.add(Flatten(input_shape=base_model.output_shape[1:]))
#top_model.add(Dense(256, activation='relu'))
#top_model.add(Dropout(0.5))
top_model.add(Dense(3, activation='softmax'))
model.add(top_model)
model.layers[0].trainable = True
print (model.summary())


# compile the model with a SGD/momentum optimizer
# and a very slow learning rate.
model.compile(loss='categorical_crossentropy',
              optimizer=optimizers.SGD(lr=1e-4, momentum=0.9),
              #optimizer=optimizers.Adam(),
              metrics=['accuracy'])
#model.load_weights(weights_path)

#layer_1 = K.function([model.layers[0].input], [model.layers[0].output])
'''
test_datagen = ImageDataGenerator(rescale=1. / 255)
validation_generator = test_datagen.flow_from_directory(
    test_data_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical',
    shuffle=False)
'''
#test_X_Result = model.predict_generator(validation_generator,3)
#base_model.save_weights('test_one_design.h5')
#
#print(test_X_Result)


# prepare data augmentation configuration

train_datagen = ImageDataGenerator(
    rescale=1. / 255,
    shear_range=0.2,
    zoom_range=0.2,
    horizontal_flip=True)

test_datagen = ImageDataGenerator(rescale=1. / 255)

train_generator = train_datagen.flow_from_directory(
    train_data_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical')

validation_generator = test_datagen.flow_from_directory(
    validation_data_dir,
    target_size=(img_height, img_width),
    batch_size=batch_size,
    class_mode='categorical')

# fine-tune the model
model.fit_generator(
    train_generator,
    steps_per_epoch=nb_train_samples // batch_size,
    epochs=epochs,
    validation_data=validation_generator,
    validation_steps=nb_validation_samples // batch_size)
model.save_weights(weights_path)
