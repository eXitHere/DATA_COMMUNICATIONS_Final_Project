from flask import Flask, request
import os

app = Flask(__name__)

key = 'd2b66b81d032b320360a3f38fd6d0587'
state = 0

@app.route('/update', methods=['POST'])
def update():
    global state
    if request.headers.get('iam') == key:
        state = os.system('python ./project-manager/FLASK-Server/test.py')
        return 'Running process'
    return 'Who you are?'

app.run(host='0.0.0.0', port=5002, debug=True)