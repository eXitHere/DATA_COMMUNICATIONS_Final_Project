import RPi.GPIO as GPIO
from time import sleep
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
pin_select_mode = 23
GPIO.setup(pin_select_mode, GPIO.IN)


def get_mode():
    return not GPIO.input(pin_select_mode)