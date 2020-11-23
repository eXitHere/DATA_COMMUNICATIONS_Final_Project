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


def serial_arudino2():
    global stop_threads
    global send_msg
    global string_recever_arduino2
    global send_msg_Arduono2
    global serial_Arduino2
    if not serial_Arduino2.isOpen():
        serial_Arduino2.open()
    #try:
    serialString = ""
    while True:
        #print("In thread!")
        while not serial_Arduino2.isOpen():
            #print("Serial 2 offline")
            time.sleep(0.1)
        #print("Test")
        if serial_Arduino2.isOpen():
            try:
                if send_msg_Arduono2 != "":
                    #print("Writed", send_msg_Arduono2)
                    serial_Arduino2.write(
                        str.encode('{0}'.format(send_msg_Arduono2)))
                    send_msg_Arduono2 = ""
                if serial_Arduino2.inWaiting():
                    serialString = serial_Arduino2.readline()
                    #print("Data in serial", serialString)
                    serial_Arduino2.flush()
                    if serialString[0] != '\r\n':
                        temp = serialString.decode('Ascii').replace('\r\n', '')
                        string_recever_arduino2.append(temp)
            except:
                time.sleep(0.1)
        if stop_threads:
            break

    # except:
    #     pass
    # finally:
    #     print("Something wrong!")
    #     serial_Arduino2.close()
    #     if not serial_Arduino2.isOpen():
    #         print("Program,Serial comm is closed")


@app.route('/senddata', methods=['POST'])
def senddata():
    global send_msg_Arduono2
    command = request.get_json()['data']
    #print(command)
    if command == 'TEST':
        travel_capture()
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
    global send_msg_Arduono2
    global serial_Arduino3
    global serial_Arduino2
    global processing_mode
    processing_mode = get_mode()
    DEGREE = ["L", "M", "R"]
    ans = []
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
            print("processing with ML")
            code, pattern = process_with_ml(serial_Arduino3, DEGREE[j],
                                            config['server_processing'])
        elif processing_mode == PROCESS_WITH_IFELSE:
            print('processing with if else')
            code, pattern = process_with_ifelse(serial_Arduino3, DEGREE[j])
        ans.append(code)
        print(pattern)

    print(ans)


def init_Camera():
    global serial_Arduino3
    # Flush 5 image
    for j in range(4):
        print("Flush ", j + 1)
        flush_image(serial_Arduino3)


def serial_arudino3():
    #init_Camera()
    pass


if __name__ == "__main__":
    if processing_mode == PROCESS_WITH_IFELSE:
        print("PROCESS WITH IF ELSE")
    else:
        print("PROCESS WITH ML")
    loadConfig()
    init_Serial()
    thread_Serial3 = threading.Thread(target=serial_arudino3)
    thread_Serial2 = threading.Thread(target=serial_arudino2)
    thread_Serial3.start()
    thread_Serial2.start()
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
