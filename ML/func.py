import numpy as np
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing import image

model = load_model('model_datacom_2')

def function(parameter):
    array_test_img = image.img_to_array(parameter)
    array_test_img = np.expand_dims(array_test_img,axis=0)
    target = np.vstack([array_test_img])
    return np.argmax(model.predict(target), axis=-1)

