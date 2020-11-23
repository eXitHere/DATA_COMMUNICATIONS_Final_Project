from __future__ import print_function
import requests
import json
import cv2

addr = 'http://192.168.88.21:5000'
test_url = addr + '/api/test'
content_type = 'image/jpeg'
headers = {'content-type': content_type}


def send(image_file):
    img = cv2.imread(r'{}'.format(image_file))
    _, img_encoded = cv2.imencode('.jpg', img)
    response = requests.post(test_url,
                             data=img_encoded.tostring(),
                             headers=headers)
    return json.loads(response.text)
