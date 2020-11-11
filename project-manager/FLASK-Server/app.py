from flask import Flask, Response, redirect, request, url_for, jsonify, send_file
import itertools
import os
import time
import serial as Serial
import time
import threading
import signal
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

key = 'd2b66b81d032b320360a3f38fd6d0587'
state = 0

stop_threads = False
send_msg = ""
ser = Serial.Serial("COM5", 115200)

string_recever = []

def serial_run():
    if not ser.isOpen():
        ser.open()
    try:
        if ser.isOpen():
            serialString = ""                           # Used to hold data coming over UART
            global stop_threads 
            global send_msg
            global string_recever
            while(1):
                time.sleep(0.1)
                if send_msg != "":
                    ser.write(str.encode('{0}'.format(send_msg)))
                    send_msg = ""
                    time.sleep(1)
                    print("Writed : ", send_msg)
                if ser.in_waiting > 0:
                    serialString = ser.readline()
                    if serialString[0] != '\r\n':
                        #print("resever: " + str(string_recever))
                        temp = serialString.decode('Ascii').replace('\r\n', '')
                        string_recever.append(temp)
                        #print(temp, end='')
                if stop_threads:
                    break
    except KeyboardInterrupt:
        ser.close()
        if not ser.isOpen():
            print("User,Serial comm is closed")
    finally:
        ser.close()
        if not ser.isOpen():
            print("Program,Serial comm is closed")

@app.route('/update', methods=['POST'])
def update():
    global state
    if request.headers.get('iam') == key:
        state = os.system('python ./project-manager/FLASK-Server/test.py')
        return 'Running process'
    return 'Who you are?'

@app.route('/senddata', methods=['POST'])
def senddata():
    global send_msg
    print(request.get_json()['data'])
    send_msg = request.get_json()['data']
    return 'OK'

@app.route( '/stream' )
def stream():
    def read_process():
        global string_recever
        if len(string_recever):
            yield "data:" + string_recever.pop(0) +'\n\n'
            

    return Response( read_process(), mimetype= 'text/event-stream' )

@app.route('/')
def index():
    return send_file('index.html')

@app.route('/shutdown', methods=['POST'])
def shutdown():
    sig = getattr(signal, "SIGKILL", signal.SIGTERM)
    os.kill(os.getpid(), sig)

if __name__ == "__main__":
    x = threading.Thread(target=serial_run)
    x.start()
    try:
        app.run(host="0.0.0.0", port=5000, debug=True, use_debugger=False, use_reloader=False)
    except:
        #pass
        stop_threads = True
        ser.close()
        if not ser.isOpen():
            print("User,Serial comm is closed")
        sys.exit()