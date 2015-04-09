import serial
import time

#send start calibration info
ser = serial.Serial('/dev/tty.usbmodemfd121', 115200, timeout=1)
ser.write("c")

#wait for simulated calibration
time.sleep(5)

#finish calibration
s = ser.read(100) #required to flush the data (?)
ser.flush() #clear the buffer before requesting the results

ser.write("s")
s = ser.read(100)
print "results of encoder calibration: "
print s

ser.close()