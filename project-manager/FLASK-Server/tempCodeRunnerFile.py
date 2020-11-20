import serial
from PIL import Image
import time
import sys

if len(sys.argv) > 1:
    port = sys.argv[1]
else:
    port='COM5'

print("Using port {}".format(port))
width=120
height=120

def read_image(serial):
    print("Reading Image...")
    data = serial.read(width * height)
    print("Received Image...")
    image = Image.frombytes('P', (width, height), data)
    image = image.transpose(Image.ROTATE_270)
    image.save('./image.bmp')

ser = serial.Serial(
    port=port,
    baudrate=1000000,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)

print('Wait for Arduino to set itself up...')
time.sleep(5)

if ser.isOpen() == False:
    ser.open()
    print('Serial opened')
else:
    print('Serial already open')


str_ = ""
counter = 0
while True:
    print("Waiting for RDY from Arduino...")
    print(("Arduino says {}".format(ser.read_until('RDY'.encode('utf-8')))))
    #inp = input('Hit enter to fetch image, any other key to exit >')
    #if inp == "":
    ser.write(str.encode('X'))
    ser.flush()
    read_image(ser)
    #else:
    #    break
    #temp = ser.read()
    #if temp[0] != '\r\n':
    #    str_ += str(temp)[2:-1]
    #    if str_ == "RDY":
    #        print("RDY!")
    #        ser.write(str.encode('X'))
    #        ser.flush()
    #        read_image(ser)
    #        print("completed")
    #        str_ = ""

ser.close()
print('Serial closed')