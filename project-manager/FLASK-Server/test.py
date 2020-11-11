import serial as Serial
import time
import threading

def gotoxy(x,y):
    print ("%c[%d;%df" % (0x1B, y, x), end='')

stop_threads = False
send_msg = ""
ser = Serial.Serial("COM5")
def serial_run():
    if not ser.isOpen():
        ser.open()
    try:
        if ser.isOpen():
            serialString = ""                           # Used to hold data coming over UART
            global stop_threads 
            global send_msg
            while(1):
                if send_msg != "":
                    ser.write(str.encode('{0}'.format(send_msg)))
                    send_msg = ""
                    time.sleep(1)
                if ser.in_waiting > 0:
                    serialString = ser.readline()
                    if serialString[0] != '\r\n':
                        print(serialString.decode('Ascii'), end='')
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

if __name__ == "__main__":
    x = threading.Thread(target=serial_run)
    x.start()
    try:
        while True:
            send_msg = input("Enter string : ")
    except KeyboardInterrupt:
        stop_threads = True
        ser.close()
        if not ser.isOpen():
            print("User,Serial comm is closed")