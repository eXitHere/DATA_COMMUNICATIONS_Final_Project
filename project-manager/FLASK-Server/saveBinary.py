f = open("binaryfile.txt", "rb")
t = f.read()
#print(t)
print(t)
from PIL import Image
image = Image.frombytes('P', (120, 120), t)
image = image.transpose(Image.ROTATE_270)
image.save('./image.bmp')