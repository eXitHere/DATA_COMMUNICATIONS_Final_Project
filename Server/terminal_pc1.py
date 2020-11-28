import os
import time
import serial
import time
import threading
import json
from termcolor import colored
from flask import Flask, Response, redirect, request, url_for, jsonify, send_file, render_template
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

serial_Arduino = None
stop_thread = False
config = None
string_recever_arduino = ""
send_msg_Arduono = ""


def serial_run():
    global stop_thread
    global string_recever_arduino
    global send_msg_Arduono
    global serial_Arduino
    if not serial_Arduino.isOpen():
        serial_Arduino.open()
    try:
        serialString = ""
        while True:
            #print("In thread!")
            while not serial_Arduino.isOpen():
                #print("Serial 1 offline")
                if stop_thread:
                    print("stop threaded")
                time.sleep(0.1)
            #print("Test")
            if serial_Arduino.isOpen():
                try:
                    #print("Thread ready!")
                    if send_msg_Arduono != "":
                        #print("Writed", send_msg_Arduono2)
                        serial_Arduino.write(
                            str.encode('{0}\n'.format(send_msg_Arduono)))
                        print(
                            colored(
                                (send_msg_Arduono, "Writed: ",
                                 str.encode('{0}\n'.format(send_msg_Arduono))),
                                "cyan"))
                        send_msg_Arduono = ""
                    if serial_Arduino.inWaiting():
                        #print("Have data!")
                        serialString = serial_Arduino.readline()
                        print(
                            colored(("Data in serial", serialString),
                                    "magenta"))
                        serial_Arduino.flush()
                        #print(serialString.decode().replace('\n', ''))
                        if serialString[0] != '\r\n':
                            temp = serialString.decode().replace('\n', '')
                            print(temp, len(temp))
                            decode_dec_to_data(temp[21:21 + 22])
                            decode_three_degree(temp[21:25])
                            string_recever_arduino.append(temp)
                except:
                    time.sleep(0.1)
            if stop_thread:
                print("stop threaded")
                break

    except:
        pass
    finally:
        serial_Arduino.close()
        if not serial_Arduino.isOpen():
            print("Program,Serial comm is closed")


def loadConfig():
    global config
    with open("config.json") as json_data_file:
        config = json.load(json_data_file)


def init():
    global serial_Arduino
    serial_Arduino = serial.Serial(port=config['port_Arduino1'],
                                   baudrate=config['buadrate_Arduino1'],
                                   parity=serial.PARITY_NONE,
                                   stopbits=serial.STOPBITS_ONE,
                                   bytesize=serial.EIGHTBITS)


def decode_three_degree(code):
    if len(code) == 4 and code[0] == 'X':
        tmp = ("*** Answer from PC2 ***") + '\n'
        tmp += '   -45 Degree: ' + str(bin(int(code[1],
                                               16)))[2:].zfill(4) + '\n'
        tmp += '     0 Degree: ' + str(bin(int(code[2],
                                               16)))[2:].zfill(4) + '\n'
        tmp += '    45 Degree: ' + str(bin(int(code[3],
                                               16)))[2:].zfill(4) + '\n'
        print(colored(tmp + "*** --------------- ***", "green"))


def decode_dec_to_data(code):
    #print("Call function")
    postition = [[40, 20], [40, 70], [100, 20], [100, 70]]
    tmp = ''
    tmp += ("*** Answer from PC2 ***") + '\n'
    if code[0] == '*' and code[-1] == '*' and len(code) == 22:
        #print("Yes this code")
        idx = 0
        idy = 0
        for x in code[1:-1]:
            if idx < 4:
                tmp += ('     ') + (
                    ('(' + str(postition[idy][0] + (idx * 4)) + ',' +
                     str(postition[idy][1] +
                         (idx * 4)) + '): ' + str(ord(x)))) + '\n'
            else:
                tmp += (
                    ('mean of QUADRANT {}: '.format(idy) + str(ord(x)))) + '\n'
            idx += 1
            if idx == 5:
                idx = 0
                idy += 1
        print(colored(tmp[:-1] + '\n' + "*** --------------- ***", "green"))


## Server route
@app.route('/senddata', methods=['POST'])
def senddata():
    global send_msg_Arduono
    try:
        command = request.get_json()['data']
        #print(command)
        send_msg_Arduono = command
        return 'OK'
    except:
        return 'Error'


#arduino 1
@app.route('/stream1')
def stream():
    def read_process():
        while True:
            global string_recever_arduino
            for idx in range(len(string_recever_arduino)):
                if len(string_recever_arduino):
                    yield "data:" + string_recever_arduino.pop(0) + '\n\n'
            time.sleep(0.1)

    return Response(read_process(), mimetype='text/event-stream')


#thread for terminal input
def user_inp():
    global stop_thread
    global send_msg_Arduono
    try:
        while True:
            tmp = input()
            if len(tmp) == 4:
                try:
                    send_msg_Arduono = hex(int(tmp, 2))[2]
                    #print(send_msg_Arduono)
                except:
                    send_msg_Arduono = tmp
            else:
                send_msg_Arduono = tmp
            #print('command', x)
            if send_msg_Arduono == '#' or stop_thread:
                stop_thread = True
                break
    except:
        stop_thread = True


if __name__ == "__main__":
    loadConfig()
    init()
    thread_serial = threading.Thread(target=serial_run).start()
    thread_inp = threading.Thread(target=user_inp).start()
    send_msg_Arduono = ''
    try:
        app.run(host="0.0.0.0",
                port=5000,
                debug=True,
                use_debugger=False,
                use_reloader=False)
    except:
        pass
    finally:
        stop_thread = True
        if serial_Arduino.isOpen():
            serial_Arduino.close()
