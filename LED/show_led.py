import RPi.GPIO as GPIO  # Import Raspberry Pi GPIO library
from time import sleep  # Import the sleep function from the time module
import random
import threading
GPIO.setwarnings(False)  # Ignore warning for now
GPIO.setmode(GPIO.BCM)  # Use physical pin numbering

led_lst = [26, 19, 13, 6, 5, 11, 9, 10, 22, 27, 17, 18]

pin_select_mode = 23

virtual_led = [0 for _ in range(12)]

pattern = [0, 0, 0]
is_thread_run = True


def setup_pin():
    for x in led_lst:
        GPIO.setup(x, GPIO.OUT, initial=GPIO.HIGH)
    GPIO.setup(pin_select_mode, GPIO.IN)
    # pass


def onLED(n):
    GPIO.output(led_lst[n], GPIO.LOW)
    # print('LED :', n, "ON")
    # virtual_led[n] = 1


def offLED(n):
    GPIO.output(led_lst[n], GPIO.HIGH)
    # print('LED :', n, "OFF")
    # virtual_led[n] = 0


def freestyle():
    global pattern
    global is_thread_run

    run, direction = 1, 0

    while is_thread_run:
        # for k in range(12):
        #     onLED(k)
        #     sleep(0.01)
        #     offLED(k)
        #     sleep(0.01)
        for j in range(3):
            #print(x,end=' ')
            if pattern[j] < 0 or pattern[j] > 15:  # running LED
                x = run
                for i in range(4):
                    if x & 1:
                        onLED(4 * j + i)
                    else:
                        offLED(4 * j + i)
                    x >>= 1
                if run == 8:
                    direction = 1
                elif run == 1:
                    direction = 0
                if direction == 0:
                    run <<= 1
                else:
                    run >>= 1
            else:
                x = pattern[j]
                for i in range(4):
                    if x & 1:
                        onLED(4 * j + i)
                    else:
                        offLED(4 * j + i)
                    x >>= 1
        sleep(0.25)
        # print()
        # print(' 0  1  2  3  4  5  6  7  8  9 10 11')
        # print(virtual_led)


def clear():
    for k in range(11, -1, -1):
        onLED(k)
        sleep(0.01)
        offLED(k)
        sleep(0.01)


if __name__ == "__main__":
    print("Start program")
    setup_pin()
    thread_for_show_led = threading.Thread(target=freestyle)
    thread_for_show_led.start()
    stack = []
    while True:
        # print(type(GPIO.input(pin_select_mode)))
        left, middle, right = input(
            'Enter number between [0,15] ( L M R ) :  ').strip().split()
        left, middle, right = int(left), int(middle), int(right)
        # t = 15 if GPIO.input(pin_select_mode) == 0 else 0
        # print(t)
        if left == -1:
            is_thread_run = False
            break
        pattern[0] = right
        pattern[1] = middle
        pattern[2] = left
        sleep(2)
        # clear()
    print("End program")
