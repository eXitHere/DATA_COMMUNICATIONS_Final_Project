from test import *
from tensorflow.keras.preprocessing import image
import numpy as np

img1 = image.load_img(r'../ML/images/train/1100/1100__19.bmp')
img2 = image.load_img(r'../ML/images/train/1111/1111__5.bmp')
img3 = image.load_img(r'../ML/images/train/0101/0101__5.bmp')
img4 = image.load_img(r'../ML/images/train/0010/0010_19.bmp')


print(function(img1))
print(function(img2))
print(function(img3))
print(function(img4))