import json
from flask import Flask, Response, redirect, request, url_for, jsonify, send_file, render_template
import itertools
import os
import time
import serial
import time
import threading
import signal
from flask_cors import CORS
from Image_processing import *
from read_mode import get_mode
from show_led import led_freestyle, setup_pin
from termcolor import colored

#define zone
PROCESS_WITH_ML = 0
PROCESS_WITH_IFELSE = 1

app = Flask(__name__)
CORS(app)

state = 0
stop_threads = False
send_msg = ""
config = {}

serial_Arduino3 = None  # Camera
serial_Arduino2 = None  # Servo and FM

string_recever_arduino2 = []

send_msg_Arduono3 = ""
send_msg_Arduono2 = ""

processing_mode = 0  # 0: ML , 1: if else

pattern_led = [0, 0, 0]

travel_capture_thread = None

storage_image_id = [-1, -1, -1]

focus_idx = -1


def serial_arudino2():
    global stop_threads
    global send_msg
    global string_recever_arduino2
    global send_msg_Arduono2
    global serial_Arduino2
    global travel_capture_thread
    global storage_image_id
    global focus_idx
    if not serial_Arduino2.isOpen():
        serial_Arduino2.open()
    try:
        serialString = ""
        while True:
            #print("In thread!")
            while not serial_Arduino2.isOpen():
                #print("Serial 2 offline")
                if stop_threads:
                    break
                time.sleep(0.1)
            #print("Test")
            if serial_Arduino2.isOpen():
                try:
                    #print("Thread ready!")
                    if send_msg_Arduono2 != "":
                        #print("Writed", send_msg_Arduono2)
                        serial_Arduino2.write(
                            str.encode('{0}\n'.format(send_msg_Arduono2)))
                        print(send_msg_Arduono2, "Writed: ",
                              str.encode('{0}\n'.format(send_msg_Arduono2)))
                        send_msg_Arduono2 = ""
                    if serial_Arduino2.inWaiting():
                        #print("Have data!")
                        serialString = serial_Arduino2.readline()
                        print(
                            colored(("Data in serial", serialString),
                                    "magenta"))
                        serial_Arduino2.flush()
                        if serialString[0] != '\r\n':
                            temp = serialString.decode('Ascii').replace(
                                '\n', '')
                            print(colored(str(storage_image_id), "red"))
                            if "capture" in temp:
                                #print("Start capture")
                                if not travel_capture_thread.isAlive():
                                    storage_image_id = [-1, -1, -1]
                                    travel_capture_thread = threading.Thread(
                                        target=travel_capture)
                                    travel_capture_thread.start()
                            elif str(storage_image_id) != '[-1, -1, -1]':
                                #print("Do this")
                                #print(temp)
                                try:
                                    if (temp[21] >= '0' and temp[21] <= '9'
                                        ) or (temp[21] >= 'a'
                                              and temp[21] <= 'f'):
                                        #print("Fucking Doing")
                                        focus_idx = storage_image_id.index(
                                            int(temp[21], 16))
                                        if not travel_capture_thread.isAlive():
                                            travel_capture_thread = threading.Thread(
                                                target=travel_capture)
                                            travel_capture_thread.start()
                                except:

                                    #print("Err")
                                    pass
                            string_recever_arduino2.append(temp)
                except:
                    time.sleep(0.1)
            if stop_threads:
                break

    except:
        pass
    finally:
        print("Something wrong!")
        serial_Arduino2.close()
        if not serial_Arduino2.isOpen():
            print("Program,Serial comm is closed")


@app.route('/senddata', methods=['POST'])
def senddata():
    global send_msg_Arduono2
    global storage_image_id
    global travel_capture_thread
    command = request.get_json()['data']
    #print(command)
    if command == 'TEST':
        if not travel_capture_thread.isAlive():
            storage_image_id = [-1, -1, -1]
            travel_capture_thread = threading.Thread(target=travel_capture)
            travel_capture_thread.start()
        #travel_capture_thread.start()
    else:
        send_msg_Arduono2 = command
    return 'OK'


#arduino 2
@app.route('/stream2')
def stream():
    def read_process():
        while True:
            global string_recever_arduino2
            for idx in range(len(string_recever_arduino2)):
                if len(string_recever_arduino2):
                    yield "data:" + string_recever_arduino2.pop(0) + '\n\n'
            time.sleep(0.1)

    return Response(read_process(), mimetype='text/event-stream')


@app.route('/')
def index():
    return render_template('main.html')


def loadConfig():
    global config
    with open("config.json") as json_data_file:
        config = json.load(json_data_file)
        #print(config)


def init_Serial():
    global serial_Arduino3
    global serial_Arduino2
    global config
    print(config)
    serial_Arduino2 = serial.Serial(port=config['port_Arduino2'],
                                    baudrate=config['buadrate_Arduino2'],
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    bytesize=serial.EIGHTBITS)
    serial_Arduino3 = serial.Serial(
        port=config['port_Arduino3'],
        baudrate=config['buadrate_Arduino3'],
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
    )
    if serial_Arduino3.isOpen():
        #print("Close port 3")
        serial_Arduino3.close()
    #print('Wait for Arduino to set itself up...')
    time.sleep(5)


def travel_capture():
    global focus_idx
    global send_msg_Arduono2
    global serial_Arduino3
    global serial_Arduino2
    global processing_mode
    global pattern_led
    global storage_image_id
    processing_mode = get_mode()
    DEGREE = ["L", "M", "R"]
    ans = []
    loadding_led()
    if focus_idx == -1:
        for j in range(3):
            if serial_Arduino3.isOpen():
                serial_Arduino3.close()
                #print("Close port 3")
            if not serial_Arduino2.isOpen():
                serial_Arduino2.open()
                #print("Open port 3")
            send_msg_Arduono2 = DEGREE[j]
            #print("before delay")
            time.sleep(2)
            #print("After delay")
            if serial_Arduino2.isOpen():
                #print("Close port 2")
                serial_Arduino2.close()
            if not serial_Arduino3.isOpen():
                serial_Arduino3.open()
                #print("Open port 3")
            if processing_mode == PROCESS_WITH_ML:
                print(colored("processing with ML", "yellow"))
                code, pattern = process_with_ml(serial_Arduino3, DEGREE[j],
                                                config['server_processing'])
            elif processing_mode == PROCESS_WITH_IFELSE:
                print(colored('processing with if else', "yellow"))
                code, pattern = process_with_ifelse(serial_Arduino3, DEGREE[j])
            ans.append(code)
            print(colored(code, "magenta"))
            pattern_led[2 - j] = int(code)
            #print(colored(pattern_led, "blue"))
        if serial_Arduino3.isOpen():
            serial_Arduino3.close()
        if not serial_Arduino2.isOpen():
            serial_Arduino2.open()
        storage_image_id = ans
        print(colored(("Storage image id: ", storage_image_id), "cyan"))
        send_msg_Arduono2 = 'X' + ''.join([hex(x)[2:] for x in ans])
        print(colored("Send", "green"), colored(send_msg_Arduono2, "cyan"),
              colored("To arduino1", "green"))
        #print(ans)
    else:  ## forcus on index
        if serial_Arduino3.isOpen():
            serial_Arduino3.close()
            #print("Close port 3")
        if not serial_Arduino2.isOpen():
            serial_Arduino2.open()
            #print("Open port 3")
        send_msg_Arduono2 = DEGREE[focus_idx]
        #print("before delay")
        time.sleep(2)
        #print("After delay")
        if serial_Arduino2.isOpen():
            #print("Close port 2")
            serial_Arduino2.close()
        if not serial_Arduino3.isOpen():
            serial_Arduino3.open()
            #print("Open port 3")
        if processing_mode == PROCESS_WITH_ML:
            print(colored("processing with ML", "yellow"))
            code, pattern = process_with_ml(serial_Arduino3, DEGREE[focus_idx],
                                            config['server_processing'])
        elif processing_mode == PROCESS_WITH_IFELSE:
            print(colored('processing with if else', "yellow"))
            code, pattern = process_with_ifelse(serial_Arduino3,
                                                DEGREE[focus_idx])
        ans.append(code)
        print(colored(pattern, "magenta"))
        pattern_led[2 - focus_idx] = int(code)
        #print(colored(pattern_led, "blue"))
        if serial_Arduino3.isOpen():
            serial_Arduino3.close()
        if not serial_Arduino2.isOpen():
            serial_Arduino2.open()
        send_msg_Arduono2 = encode_dec_to_data(pattern)
        print(colored("Send", "green"), colored(send_msg_Arduono2, "cyan"),
              colored("To arduino1", "green"))
    time.sleep(1)
    focus_idx = -1
    send_msg_Arduono2 = 'C'


def encode_dec_to_data(arr):
    tmp = ''
    arr = arr[0] + arr[1] + arr[2] + arr[3]
    #print(arr)
    for x in arr:
        #print(x, str(chr(x)))
        tmp += str(chr(x))
    #print(str.encode(tmp))
    return '*' + tmp + '*'


def init_Camera():
    global serial_Arduino3
    # Flush 5 image
    for j in range(4):
        print("Flush ", j + 1)
        flush_image(serial_Arduino3)


def serial_arudino3():
    #init_Camera()
    pass


def loadding_led():
    global pattern_led
    pattern_led[0] = 100
    pattern_led[1] = 100
    pattern_led[2] = 100


if __name__ == "__main__":
    setup_pin()
    if processing_mode == PROCESS_WITH_IFELSE:
        print(colored("PROCESS WITH IF ELSE", "yellow"))
    else:
        print(colored("PROCESS WITH ML", "yellow"))
    loadConfig()
    init_Serial()
    travel_capture_thread = threading.Thread(target=travel_capture)
    thread_Serial2 = threading.Thread(target=serial_arudino2)
    loadding_led()
    thread_for_show_led = threading.Thread(target=led_freestyle,
                                           args=(pattern_led, stop_threads))
    thread_Serial2.start()
    thread_for_show_led.start()
    try:
        app.run(host="0.0.0.0",
                port=5000,
                debug=True,
                use_debugger=False,
                use_reloader=False)
    except:
        print("Error!")
    finally:
        stop_threads = True
        if serial_Arduino2.isOpen():
            serial_Arduino2.close()
            print("User,Serial comm2 is closed")
        if serial_Arduino3.isOpen():
            serial_Arduino3.close()
            print("User,Serial comm3 is closed")
