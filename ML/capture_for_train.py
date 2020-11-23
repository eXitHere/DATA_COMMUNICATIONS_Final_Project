import serial
from PIL import Image
import time
import sys
import random
import asyncio
import os
width=160
height=120

try:
    os.mkdir('C:/Users/Thana/Documents/Data_comm_project/ML/images/capture/train')
except:
    pass

try:
    os.mkdir('C:/Users/Thana/Documents/Data_comm_project/ML/images/capture/validation')
except:
    pass

path = "C:/Users/Thana/Documents/Data_comm_project/ML/images/capture/train/"
for i in ["0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010","1011", "1100", "1101", "1110", "1111"]:
    try:
        os.mkdir(path + i)
    except OSError:
        print ("Creation of the directory %s failed" % path + i)
    else:
        print ("Successfully created the directory %s " % path + i)

path = "C:/Users/Thana/Documents/Data_comm_project/ML/images/capture/validation/"
for i in ["0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010","1011", "1100", "1101", "1110", "1111"]:
    try:
        os.mkdir(path + i)
    except OSError:
        print ("Creation of the directory %s failed" % path + i)
    else:
        print ("Successfully created the directory %s " % path + i)

def read_image(serial, image_name):
    #print("Reading Image...")
    data = serial.read(width * height)
    print("Received Image...", image_name)
    image = Image.frombytes('P', (width, height), data)
    image = image.transpose(Image.ROTATE_270)
    image.save(image_name)

ser_servo = serial.Serial(
    port='Com15',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)

ser_camera = serial.Serial(
    port='Com5',
    baudrate=1000000,
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

image_name = ["1101", "1111", "0001"]
image_name = ["1111", "0001", "1101"]
image_name = ["0001", "1101", "1111"]
degree = ["L", "M", "R"]
delay  = [2.5, 1.5, 1.5]

for x in degree:
    ser_servo.write(str.encode(x))
    time.sleep(1)
    #ser_servo.write(str.encode('{0}'.format(degree[j])))
    #print("Waiting for RDY from Arduino...")
    print(("Arduino says {}".format(ser_camera.read_until('RDY'.encode('utf-8')))))
    #ser_servo.write(str.encode('M'))
    ser_camera.write(str.encode('X'))
    ser_camera.flush()
    read_image(ser_camera, "./temp.bmp")

for i in range(0,20,2):
    idx = random.randint(0, 1000000)
    for j in range(3):
        ser_servo.write(str.encode('{0}'.format(degree[j])))
        #print("Move servo")
        time.sleep(1)
        #print("Captured")
        for k in range(3):
            ser_camera.read_until('RDY'.encode('utf-8'))
            ser_camera.write(str.encode('X'))
            ser_camera.flush()
            read_image(ser_camera, "./images/capture/train/{}/{}_{}_{}.bmp".format(image_name[j], image_name[j], str(i), idx))
        time.sleep(0.5)
    for j in range(2,-1,-1):
        ser_servo.write(str.encode('{0}'.format(degree[j])))
        #print("Move servo")
        time.sleep(1)
        #print("Captured")
        for k in range(3):
            ser_camera.read_until('RDY'.encode('utf-8'))
            ser_camera.write(str.encode('X'))
            ser_camera.flush()
            read_image(ser_camera, "./images/capture/train/{}/{}_{}_{}.bmp".format(image_name[j], image_name[j], str(i+1), idx))
        time.sleep(0.5)
for i in range(0,6,2):
    idx = random.randint(0, 1000000)
    for j in range(3):
        ser_servo.write(str.encode('{0}'.format(degree[j])))
        #print("Move servo")
        time.sleep(1)
        #print("Captured")
        for k in range(3):
            ser_camera.read_until('RDY'.encode('utf-8'))
            ser_camera.write(str.encode('X'))
            ser_camera.flush()
            read_image(ser_camera, "./images/capture/validation/{}/{}_{}_{}.bmp".format(image_name[j], image_name[j], str(i), idx))
        time.sleep(0.5)
    for j in range(2,-1,-1):
        ser_servo.write(str.encode('{0}'.format(degree[j])))
        #print("Move servo")
        time.sleep(1)
        #print("Captured")
        for k in range(3):
            ser_camera.read_until('RDY'.encode('utf-8'))
            ser_camera.write(str.encode('X'))
            ser_camera.flush()
            read_image(ser_camera, "./images/capture/validation/{}/{}_{}_{}.bmp".format(image_name[j], image_name[j], str(i+1), idx))
        time.sleep(0.5)
    
ser_servo.write(str.encode('M'))
ser_camera.close()
ser_servo.close()
print('Serial closed')