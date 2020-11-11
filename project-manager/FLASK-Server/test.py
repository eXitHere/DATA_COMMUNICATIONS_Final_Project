import serial as Serial
import time
import threading

def gotoxy(x,y):
    print ("%c[%d;%df" % (0x1B, y, x), end='')

stop_threads = False
send_msg = ""
ser = Serial.Serial("COM1", 1000000)
temp = []
def serial_run():
    print("Started")
    if not ser.isOpen():
        ser.open()
    try:
        if ser.isOpen():
            serialString = ""                           # Used to hold data coming over UART
            started = False
            global stop_threads 
            global send_msg
            countPix = 0
            tmpFlag = ""
            tmp = ""
            while(1):
                #if send_msg != "":
                #    ser.write(str.encode('{0}'.format(send_msg)))
                #    send_msg = ""
                #    time.sleep(1)
                if ser.in_waiting > 0:
                    serialString = ser.read()
                    #temp.append(str(serialString)[4:-1])
                    t = str(serialString)
                    if not started:
                        try:
                            tmpFlag += serialString.decode('Ascii')
                            if tmpFlag == "*SHIT*":
                                started = True
                                print('Found flag')
                        except:
                            pass
                    if started:
                        countPix += 1
                        #tmp += str(serialString[3:-1])
                        tmp += (serialString)
                        if countPix > 30*30:
                            print(tmp)
                            with open('image.png', 'wb') as file:
                                file.write(bytearray.fromhex(tmp))
                            break
                    #print(str(serialString)[2:-1])
                    #if serialString[0] != '\r\n':
                        #print(serialString.decode('Ascii'), end='')
    except KeyboardInterrupt:
        #print(len(temp))
        #print(temp)
        #print("Fucking")
        ser.close()
        if not ser.isOpen():
            print("User,Serial comm is closed")
    finally:
        #print(temp)
        ser.close()
        if not ser.isOpen():
            print("Program,Serial comm is closed")

if __name__ == "__main__":
    serial_run()
    #x = threading.Thread(target=serial_run)
    #x.start()
    #try:
    #    pass
        #while True:
    #        #send_msg = input("Enter string : ")
    #except KeyboardInterrupt:
    #    stop_threads = True
    #    ser.close()
    #    if not ser.isOpen():
    #        print("User,Serial comm is closed")