import cv2
from 
 import Image
import sys
import numpy as np
import time
from send_to_server import *

width = 160
height = 120


def process_without_ML(image):
    img = cv2.imread(r'{}'.format(image), 0)
    #print("loaded image")
    ans = []
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[40 + (i * 4):40 + (i * 4) + 1,
                    20 + (i * 4):20 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[40 + (i * 4):40 + (i * 4) + 1,
                    70 + (i * 4):70 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[100 + (i * 4):100 + (i * 4) + 1,
                    20 + (i * 4):20 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[100 + (i * 4):100 + (i * 4) + 1,
                    70 + (i * 4):70 + (i * 4) + 1].mean()))
    ans.append(tmp)
    pattern = ""
    for x in ans:
        avg = int(sum(x) / len(x))
        x.append(avg)
        pattern += '0' if avg < 70 else '1'
    #print(ans, pattern)
    return ans, pattern


def process_with_ml(serial, x, target_ip):
    ans1 = "x"
    ans2 = "y"
    while ans1 != ans2:
        ans1 = process_image(serial, x, target_ip)
        time.sleep(0.5)
        ans2 = process_image(serial, x, target_ip)
        print(ans1, ans2)
    avg, ansX = process_without_ML('./{}.bmp'.format(x))
    return ans1, avg


def process_image(serial, x, target_ip):
    read_image(serial, x)
    read_image(serial, x)
    read_image(serial, x)
    read_image(serial, x)
    read_image(serial, x)
    return send(target_ip, './{}.bmp'.format(x))


def process_with_ifelse(serial, x):
    ans1 = "x"
    ans2 = "y"
    avg = []
    while ans1 != ans2:
        read_image(serial, x)
        avg, ans1 = process_without_ML('./{}.bmp'.format(x))
        time.sleep(0.5)
        avg, ans2 = process_without_ML('./{}.bmp'.format(x))
        print(ans1, ans2)
    return ans1, avg


def read_image(serial, x):
    #print("wait for arduino rdy")
    serial.flush()
    serial.read_until('*RDY*'.encode('utf-8'))
    #print("captured")
    serial.write(str.encode('X'))
    data = serial.read(width * height)
    _image = Image.frombytes('P', (width, height), data)
    _image = _image.transpose(Image.ROTATE_270)
    _image.save('./{}.bmp'.format(x))
    #print('save image')

def flush_image(serial):
    #print("wait for arduino rdy")
    #time.sleep(0.1)
    serial.flush()
    serial.read_until('*RDY*'.encode('utf-8'))
    print("captured")
    serial.write(str.encode('X'))
    print("flush")
    print("Reading Image...")
    data = serial.read(width * height)
    #print("Received Image...")
    #print("Flush image!")