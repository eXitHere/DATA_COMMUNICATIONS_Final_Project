from PIL import Image
import sys
import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image

model = load_model('./ML/model_datacom')

width=120
height=120

def process():
    img1 = image.load_img(r'./temp.bmp')
    array_test_img = image.img_to_array(img1)
    array_test_img = np.expand_dims(array_test_img,axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model.predict(target), axis=-1)

def read_image(serial):
    serial.read_until('RDY'.encode('utf-8'))
    serial.write(str.encode('X'))
    serial.flush()
    #print("Reading Image...")
    data = serial.read(width * height)
    #print("Received Image...")
    _image = Image.frombytes('P', (width, height), data)
    _image = _image.transpose(Image.ROTATE_270)
    #print("Processing")
    #print(type(image))
    _image.save('./temp.bmp')
    print(process())

def flush_image(serial):
    serial.read_until('RDY'.encode('utf-8'))
    serial.write(str.encode('X'))
    serial.flush()
    #print("Reading Image...")
    data = serial.read(width * height)
    #print("Received Image...")
    _image = Image.frombytes('P', (width, height), data)