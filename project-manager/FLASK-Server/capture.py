import serial as Serial
import time
import binascii
import cv2
import numpy as np
ser = Serial.Serial("COM1", 1000000)
def serial_run():
    size_w = 160
    size_l = 160
    image = []
    tmp_image = []
    started = False
    str_ = ""
    str_lst = ""
    counter = 0
    if not ser.isOpen():
        ser.open()
    try:
        if ser.isOpen():
            serialString = ""                           # Used to hold data coming over UART
            while(1):
                #time.sleep(0.1)
                if ser.in_waiting > 0:
                    serialString = ser.read()
                    if serialString[0] != '\r\n':
                        #print("resever: " + str(string_recever))
                        temp = serialString
                        
                        if not started:
                            str_ += str(temp)[2:-1]
                            if str_ == "*RDY*":
                                print("Captured")
                                started = True
                                headers = ['42', '4D', '00', '03', '84', '36', '00', '00', '00', '00', '36', '00', '00', '00', '28', '00', '00', '00', 'F0', '00', '00', '00', '40', '01', '00', '00', '01', '00', '18', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00', '00']
                                #print(''.join(headers))
                                #str_lst = ''.join(headers)
                        elif str(temp)[2:-1][2:] != "":
                            #print(str(temp)[2:-1][2:])
                            counter += 1
                            #print(str(temp)[2:-1][2:])
                            tmp_image.append(int(str(temp)[2:-1][2:], 16))
                            if counter % size_w == 0:
                                image.append(tmp_image)
                                tmp_image = []
                            #print(str_lst)
                            if counter == size_w*size_l:
                                #print(counter*3, 320*240, (len(str_lst)/2-54))
                                #print((str_lst))
                                #cv2.imshow('image', str_lst)
                                #with open('image.bmp', 'wb') as file:
                                #    file.write(binascii.unhexlify(str_lst))
                                #arr = np.array(image, dtype="int16")
                                image = np.uint8(image)
                                cv2.imshow('image', image)
                                cv2.waitKey(0)
                                cv2.destroyAllWindows()
                                print(image)
                                print("Successed")
                                break
    except KeyboardInterrupt:
        ser.close()
        data = bytes.fromhex(data[2:])
        print(str_)
        if not ser.isOpen():
            print("User,Serial comm is closed")
    finally:
        ser.close()
        if not ser.isOpen():
            print("Program,Serial comm is closed")

#serial_run()
if __name__ == "__main__":
    serial_run()
    #cv2.imshow('image', skinCrCbHist)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()