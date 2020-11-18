import serial
import time
from PIL import Image
import sys
import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image

model = load_model('model_datacom')

def function(parameter):
    array_test_img = image.img_to_array(parameter)
    array_test_img = np.expand_dims(array_test_img,axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model.predict(target), axis=-1)

if len(sys.argv) > 1:
    port = sys.argv[1]
else:
    port='COM1'

width=120
height=120

def read_image(serial):
    #print("Reading Image...")
    data = serial.read(width * height)
    print("Received Image...")
    _image = Image.frombytes('P', (width, height), data)
    _image = _image.transpose(Image.ROTATE_270)
    print("Processing")
    #print(type(image))
    _image.save('./temp.bmp')
    img1 = image.load_img(r'./temp.bmp')
    print(function(img1))


def flush_image(serial):
    #print("Reading Image...")
    data = serial.read(width * height)
    print("Received Image...")
    _image = Image.frombytes('P', (width, height), data)

ser_camera = serial.Serial(
    port=port,
    baudrate=1000000,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)

ser_servo = serial.Serial(
    port='COM5',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)

print('Wait for Arduino to set itself up...')
time.sleep(5)

if ser_camera.isOpen() == False:
    ser_camera.open()
    print('Serial camera opened')
else:
    print('Serial camera already open')

if ser_servo.isOpen() == False:
    ser_servo.open()
    print('Serial servo opened')
else:
    print('Serial servo already open')

#image_name = ["0101", "0100", "0011"]
image_name = ["1111", "1110", "1001"]
degree = ["R", "M", "L"]

for j in range(5):
    ser_camera.read_until('RDY'.encode('utf-8'))
    print("Flush ", j+1)
    #ser_servo.write(str.encode('M'))
    ser_camera.write(str.encode('X'))
    ser_camera.flush()
    flush_image(ser_camera)
    

# flush data
for i in range(1):
    for j in range(3):
        ser_servo.write(str.encode(degree[j]))
        time.sleep(2)
        print("Ready to take a photo")
        print(("Arduino says {}".format(ser_camera.read_until('RDY'.encode('utf-8')))))
        #ser_servo.write(str.encode('M'))
        ser_camera.write(str.encode('X'))
        ser_camera.flush()
        read_image(ser_camera)