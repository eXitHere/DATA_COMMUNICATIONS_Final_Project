import RPi.GPIO as GPIO  # Import Raspberry Pi GPIO library
from time import sleep  # Import the sleep function from the time module
import random
GPIO.setwarnings(False)  # Ignore warning for now
GPIO.setmode(GPIO.BCM)  # Use physical pin numbering

led_lst = [26, 19, 13, 6, 5, 11, 9, 10, 22, 27, 17, 18]

pin_select_mode = 23

for x in led_lst:
    GPIO.setup(x, GPIO.OUT, initial=GPIO.HIGH)
GPIO.setup(pin_select_mode, GPIO.IN)


def freestyle(pattern):
    for k in range(12):
        GPIO.output(led_lst[k], GPIO.LOW)
        sleep(0.01)
        GPIO.output(led_lst[k], GPIO.HIGH)
        sleep(0.01)
    for j in range(3):
        #print(x,end=' ')
        for i in range(4):
            #print(i, (4*(j+1))-i-1)
            GPIO.output(led_lst[(4 * (j + 1)) - i - 1],
                        GPIO.HIGH if pattern[j] & 1 else GPIO.LOW)
            pattern[j] >>= 1
    #print()


def clear():
    for k in range(11, -1, -1):
        GPIO.output(led_lst[k], GPIO.LOW)
        sleep(0.01)
        GPIO.output(led_lst[k], GPIO.HIGH)
        sleep(0.01)


while True:
    #print(type(GPIO.input(pin_select_mode)))
    t = 15 if GPIO.input(pin_select_mode) == 0 else 0
    #print(t)
    freestyle([t, t, t])
    sleep(2)
    clear()