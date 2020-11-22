from PIL import Image
import sys
import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image
import time
from getValue import *

model = []
for i in range(4, 9):
    model.append(load_model('./ML/model_datacom_{}'.format(str(i))))
width = 160
height = 120


def process(filename, model_idx):
    img1 = image.load_img(r'{}'.format(filename))
    array_test_img = image.img_to_array(img1)
    array_test_img = np.expand_dims(array_test_img, axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model[model_idx].predict(target), axis=-1)


def process_with_two(serial, x):
    # ans1 = "x"
    # ans2 = "y"
    # while ans1 != ans2:
    #     ans1 = read_image(serial, x)
    #     time.sleep(0.5)
    #     ans2 = read_image(serial, x)
    #print("Check", ans1, ans2)
    return read_image(serial, x)


def read_image(serial, x):
    flush_image(serial)
    serial.read_until('RDY'.encode('utf-8'))
    serial.write(str.encode('X'))
    serial.flush()
    data = serial.read(width * height)
    _image = Image.frombytes('P', (width, height), data)
    _image = _image.transpose(Image.ROTATE_270)
    _image.save('./{}.bmp'.format(x))
    ans, pattern = process_without_ML('./{}.bmp'.format(x))
    return pattern
    #ans = []
    # for i in range(5):
    #     ans.append(process('./{}.bmp'.format(x), i)[0])
    # ans.append(ans[len(ans) - 1])
    # print("answer from 5 model:", ans)
    #str(bin(process('./{}.bmp'.format(x), i)[0]))[2:].zfill(4)
    #return max(ans, key=ans.count)


def flush_image(serial):
    serial.read_until('RDY'.encode('utf-8'))
    serial.write(str.encode('X'))
    serial.flush()
    #print("Reading Image...")
    data = serial.read(width * height)
    #print("Received Image...")
    _image = Image.frombytes('P', (width, height), data)