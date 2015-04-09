import time
import serial
#comms functions
def setMDSParams(SerialStream, channel, Kp, Ki, Kd, Sat): #accepts strings
	IsError = False
	ErrorString = ''
	sendStr = 'GR' + channel + ':' + Kp + ',' + Ki +',' + Kd +',' + Sat + '\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print 'returned string:',returnString

	if 'GE' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	return (IsError, ErrorString)

def setMDSLocation(SerialStream, x, y): #accepts float
	IsError = False
	ErrorString = ''
	sendStr = 'GL:'+ str(x) + ',' + str(y) + '\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print 'returned string:',returnString
	if 'GE' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	return (IsError, ErrorString)

def sendCommand(SerialStream, command): #accepts string
	sendStr = ''
	IsError = False
	ErrorString = ''
	if command == 'start':
		sendStr = 'GS\n'
	if command == 'stop':
		sendStr = 'GP\n'
	if command == 'calibrate':
		sendStr = 'GC\n'
	if command == 'calibration done':
		sendStr = 'GD\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print 'returned string:',returnString
	if 'GE' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	elif command == 'calibration done':
			if 'GI' in returnString:
				splitList = returnString.split(':', 1)
				params = splitList[1].split(',', 3)
				vals = [float(param) for param in params]
				print vals
	
	return (IsError, ErrorString)

#main 
print 'connecting to serial'
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

print 'caibrating'
sendCommand(ser, 'calibrate')
time.sleep(20)

print 'done calibrating'
sendCommand(ser, 'calibration done')
time.sleep(1)

print 'setting params'
setMDSParams(ser, 'X', '0.15', '0.000005', '5.0', '10.0')
time.sleep(1)

print 'starting'
sendCommand(ser, 'start')
time.sleep(1)

print 'moving'
#should go
setMDSLocation(ser, 100.0, 100.0)
time.sleep(5)

print 'out of bounds'
#should throw error
setMDSLocation(ser, 700.0, 500.0)
time.sleep(5)

print 'stopping'
sendCommand(ser, 'stop')
time.sleep(1)

print 'disabled'
#should also throw error
setMDSLocation(ser, 100.0, 100.0)
time.sleep(5)

print 'closing serial'
ser.close()