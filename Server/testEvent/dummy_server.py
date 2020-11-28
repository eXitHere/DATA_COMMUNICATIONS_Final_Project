import json
from flask import Flask, Response, redirect, request, url_for, jsonify, send_file, render_template
import itertools
import os
import time
import time
import threading
import signal
from flask_cors import CORS
from termcolor import colored

app = Flask(__name__)
CORS(app)

config = {}


@app.route('/senddata', methods=['POST'])
def senddata():
    command = request.get_json()['data']
    print('command')
    return 'OK'


#arduino 2
@app.route('/stream2')
def stream():
    def events():
        for i, c in enumerate(itertools.cycle('\|/-')):
            yield "data: %s %d\n\n" % (c, i)
            time.sleep(.1)  # an artificial delay

    return Response(events(), content_type='text/event-stream')


@app.route('/')
def index():
    return render_template('main.html')


def loadConfig():
    global config
    with open("config.json") as json_data_file:
        config = json.load(json_data_file)
        #print(config)


if __name__ == "__main__":
    app.run(host="0.0.0.0",
            port=5000,
            debug=True,
            use_debugger=False,
            use_reloader=True)
