import cv2
from PIL import Image
import sys
import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image
import jsonpickle
import time
import json
from flask_cors import CORS
from flask import Flask, Response, redirect, request, url_for, jsonify, send_file, render_template
app = Flask(__name__)
CORS(app)

model = load_model('./ML/model_datacom')


def process(image_):
    array_test_img = image.img_to_array(image_)
    array_test_img = np.expand_dims(array_test_img, axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model.predict(target), axis=-1)


def process_image(img):
    return process(img)[0]


def process_with_ml(img):
    ans1 = "x"
    ans2 = "y"
    while ans1 != ans2:
        ans1 = process_image(img)
        time.sleep(0.5)
        ans2 = process_image(img)
    return ans1


@app.route('/api/test', methods=['POST'])
def test():
    r = request
    nparr = np.fromstring(r.data, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    return str(process_with_ml(img))


if __name__ == "__main__":
    app.run(host="0.0.0.0",
            port=5500,
            debug=True,
            use_debugger=False,
            use_reloader=False)
