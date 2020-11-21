from func import *
from tensorflow.keras.preprocessing import image
import numpy as np

img1 = image.load_img(r'image.bmp')

print(type(img1))
print(function(img1))