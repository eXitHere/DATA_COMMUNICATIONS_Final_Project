import serial
from PIL import Image
import time
import sys

if len(sys.argv) > 1:
    port = sys.argv[1]
else:
    port='COM1'

width=120
height=120

def read_image(serial, image_name):
    #print("Reading Image...")
    data = serial.read(width * height)
    print("Received Image...", image_name)
    image = Image.frombytes('P', (width, height), data)
    image = image.transpose(Image.ROTATE_270)
    image.save(image_name)

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

# flush data
for i in range(7):
    print("Waiting for RDY from Arduino...")
    print(("Arduino says {}".format(ser_camera.read_until('RDY'.encode('utf-8')))))
    ser_servo.write(str.encode('M'))
    ser_camera.write(str.encode('X'))
    ser_camera.flush()
    read_image(ser_camera, "./images/temp.bmp")

for i in range(20):
    for j in range(3):
        ser_servo.write(str.encode(degree[j]))
        print("Waiting for RDY from Arduino...")
        print(("Arduino says {}".format(ser_camera.read_until('RDY'.encode('utf-8')))))
        ser_camera.write(str.encode('X'))
        ser_camera.flush()
        read_image(ser_camera, "./images/{}/{}_{}.bmp".format(image_name[j], image_name[j], str(i)))
        #time.sleep(0.3)
        print(("Arduino says {}".format(ser_camera.read_until('RDY'.encode('utf-8')))))
        ser_camera.write(str.encode('X'))
        ser_camera.flush()
        read_image(ser_camera, "./images/{}/{}__{}.bmp".format(image_name[j], image_name[j], str(i)))
ser_servo.write(str.encode('M'))
ser_camera.close()
ser_servo.close()
print('Serial closed')