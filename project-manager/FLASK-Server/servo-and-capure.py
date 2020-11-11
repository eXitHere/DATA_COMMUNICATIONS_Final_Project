import subprocess
import os
import time
import signal
import psutil
import serial as Serial
ser = Serial.Serial("COM5", 115200)

def kill(proc_pid):
    process = psutil.Process(proc_pid)
    for proc in process.children(recursive=True):
        proc.kill()
    process.kill()

if __name__ == "__main__":
    while True:
        command = input("Enter command : ")
        if command == '1':
            l = 1
            if ser.isOpen():
                ser.write(str.encode('R'))
            pro = subprocess.Popen('cd C:/Program Files (x86)/Java/jdk1.8.0_74/bin && java code.SimpleRead', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            if not ser.isOpen():
                ser.open()
            while l != 4:
                line = pro.stdout.readline()
                #print(line)
                if 'Saved image:' in line:
                    if l == 1 or l == 3:
                        if ser.isOpen():
                            ser.write(str.encode('M'))
                    elif l == 2:
                        if ser.isOpen():
                            ser.write(str.encode('L'))
                    #print(l)
                    l += 1
                #print(line, end='')
                if not line: break
            kill(pro.pid)
            try:
                #pro.wait(timeout=16) #8 10
                pass
            except:
                print("except")
                kill(pro.pid)
        elif command == '2':
            if not ser.isOpen():
                ser.open()
            if ser.isOpen():
                #print("Do")
                ser.write(str.encode('R'))
            pro = subprocess.Popen('cd C:/Program Files (x86)/Java/jdk1.8.0_74/bin && java code.SimpleRead', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            saved = False
            while not saved:
                line = pro.stdout.readline()
                if 'Saved image:' in line:
                    saved = True
            print("Saved 1")
            kill(pro.pid)
        elif command == '3':
            if not ser.isOpen():
                ser.open()
            if ser.isOpen():
                ser.write(str.encode('M'))
            pro = subprocess.Popen('cd C:/Program Files (x86)/Java/jdk1.8.0_74/bin && java code.SimpleRead', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            saved = False
            while not saved:
                line = pro.stdout.readline()
                if 'Saved image:' in line:
                    saved = True
            print("Saved 2")
            kill(pro.pid)
        elif command == '4':
            if not ser.isOpen():
                ser.open()
            if ser.isOpen():
                ser.write(str.encode('L'))
            pro = subprocess.Popen('cd C:/Program Files (x86)/Java/jdk1.8.0_74/bin && java code.SimpleRead', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            saved = False
            while not saved:
                line = pro.stdout.readline()
                if 'Saved image:' in line:
                    saved = True
            print("Saved 3")
            kill(pro.pid)
    if ser.isOpen():
        ser.close()