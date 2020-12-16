import serial
import RPi.GPIO as GPIO
import os, time

GPIO.setmode(GPIO.BOARD)
port = serial.Serial("/dev/ttyTHS0",
	parity = serial.PARITY_NONE,
	stopbits = serial.STOPBITS_ONE,
	baudrate=115200,
	timeout=1)

print(port.portstr)

val = 0x00

while True:
    port.write(b'hello arduino\r\n')
    #port.write(bytes(val))
    data = port.readline()
    val+=1
    #print(data)
    print(b'hello arduino\r\n')
    #print(val)
    if port.readable():
        rcv = port.readline()
        print("rcv: "+ str(rcv))
        pass

#        file = open("/home/ines-xavier-nx/ADAS_project/frame_info.txt","r")
#        frame_info = file.read()
#        print(frame_info)
#	port.write(frame_info.encode())

#        rcv = port.readline()
#        print(rcv.decode()[:len(rcv)-1])
