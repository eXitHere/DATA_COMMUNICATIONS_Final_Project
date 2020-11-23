from PIL import Image
import sys
import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image
import jsonpickle
import cv2
import time
import json
from flask_cors import CORS
from flask import Flask, Response, redirect, request, url_for, jsonify, send_file, render_template
app = Flask(__name__)
CORS(app)
model = []

for i in range(4, 9):
    model.append(load_model('./ML/model_datacom_{}'.format(str(i))))


def process(image_, model_idx):
    array_test_img = image.img_to_array(image_)
    array_test_img = np.expand_dims(array_test_img, axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model[model_idx].predict(target), axis=-1)


def process_image(img):
    ans = []
    for i in range(5):
        ans.append(process(img, i)[0])
    ans.append(ans[len(ans) - 1])
    print("answer from 5 model:", ans)
    return max(ans, key=ans.count)


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
