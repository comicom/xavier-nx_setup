# uart_test06 tlfong01 2019apr08hkt1603 ***

# Computer = Rpi3B+
# Linux    = $ hostnamectl = raspberrypi Raspbian GNU/Linux 9 (stretch) Linux 4.14.34-v7+ arm 
# Python   = >>> sys.version = 3.5.3 Jan 19 2017

# Test 1   - repeatWriteBytes() - UART port repeatedly send out bytes.  
# Function - Repeat many times sending bytes, pause after each bytes.

# Test 2   - loopBackTest() - UART port send and receive bytes.
# Function - Send one bytes to TX, wait some time (Note 1), then read bytes back from RX. 
# Setup    - Connet Tx pin to Rx pin to form a loop.

# Note 1
# Bolutek BlueTooth BC04 needs at least 10mS to respond

from   time import sleep
import serial

serialPort0 = serial.Serial(port = '/dev/ttyTHS0',
        baudrate = 115200,
        parity = serial.PARITY_NONE,
        stopbits = serial.STOPBITS_ONE,
        bytesize = serial.EIGHTBITS,
        timeout= 1)

def setSerialPortBaudRate(serialPort, baudrate):
    serialPort.baudrate = baudrate
    return

def serialPortWriteBytes(serialPort, writeBytes):
    serialPort.write(writeBytes)
    return

def serialPortReadBytes(serialPort, maxBytesLength):
    readBytes = serialPort.read(maxBytesLength)
    return readBytes

def serialPortWriteWaitReadBytes(serialPort, writeBytes, maxBytesLength, waitTime):
    serialPort.flushInput()
    serialPort.flushOutput()
    serialPort.write(writeBytes)
    sleep(waitTime) 
    readBytes = serialPortReadBytes(serialPort, maxBytesLength)
    print('        bytes written = ', writeBytes) 
    print('        bytes read    = ', readBytes)
    return readBytes

def repeatWriteBytes(serialPort, writeBytes, pauseTimeBetweenBytes, repeatCount):
    print('       Begin repeatWriteOneByte(), ...')   
    for i in range(repeatCount):
        serialPortWriteBytes(serialPort, writeBytes)                
        sleep(pauseTimeBetweenBytes)
    print('       End   repeatWriteOneByte().')
    return

def serialPortLoopBack(serialPort, writeBytes, maxBytesLength, waitTime): 
    print('        Begin serialPortLoopBack() [Remember to connect Tx to Rx!] , ...')
    serialPortWriteWaitReadBytes(serialPort, writeBytes, maxBytesLength, waitTime)     
    print('        End   serialPortLoopBack(), ...')
    return

setSerialPortBaudRate(serialPort0, 115200)
try:
    while True:
#        file = open("/home/ines-xavier-nx/ADAS_project/frame_info.txt","r")
        #repeatWriteBytes(serialPort0, b'AT\r\n', 0.01, 200000000) 
#        frame_info = file.read()
#        data = frame_info + "F"
        serialPortLoopBack(serialPort0, b'xavier test\n', 32, 0.030)

except KeyboardInterrupt:
    serialPort0.close()

''' Sample output  tlfong01 2019apr0801
>>> 
=== RESTART: /home/pi/Python_Programs/test1193/uart_test02_2019apr0801.py ===
        Begin serialPortLoopBack() [Remember to connect Tx to Rx!] , ...
        bytes written =  b'AT\r\n'
        bytes read    =  b'AT\r\n'
        End   serialPortLoopBack(), ...
>>>
'''

# End
