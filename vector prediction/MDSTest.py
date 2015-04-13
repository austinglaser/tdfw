import time
import serial

#comms functions
def setMDSParams(SerialStream, channel, Kp, Ki, Kd, Sat): #accepts strings
	IsError = False
	ErrorString = ''
	sendStr = 'MR' + channel + ':' + Kp + ',' + Ki +',' + Kd +',' + Sat + '\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print returnString
	if 'ME' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	return (IsError, ErrorString)

def setMDSLocation(SerialStream, x, y): #accepts float
	IsError = False
	ErrorString = ''
	sendStr = 'ML:'+ ('%.2f' % x) + ',' + ('%.2f' % y) + '\n'
	print sendStr
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print returnString
	if 'ME' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	return (IsError, ErrorString)

def sendCommand(SerialStream, command): #accepts string
	global x_calibration_min, x_calibration_max
	x_calibration_min, x_calibration_max = 0, 0
	sendStr = ''
	IsError = False
	ErrorString = ''
	if command == 'start':
		sendStr = 'MS\n'
	if command == 'stop':
		sendStr = 'MP\n'
	if command == 'calibrate':
		sendStr = 'MC\n'
	if command == 'calibration done':
		sendStr = 'MD\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
	print returnString
	if 'ME' in returnString:
		#there is an error
		errorList = returnString.split(':', 1)
		ErrorString = errorList[1]
		IsError = True

	elif command == 'calibration done':
		if 'MI' in returnString:
			splitList = returnString.split(':', 1)
			params = splitList[1].split(',', 3)
			vals = [float(param) for param in params]
			x_calibration_min, x_calibration_max = vals[0], vals[1]

	return (IsError, ErrorString)


#main 
print 'connecting to serial'
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

# print 'caibrating'
# sendCommand(ser, 'calibrate')
# time.sleep(20)

# print 'done calibrating'
# sendCommand(ser, 'calibration done')
# time.sleep(1)

print 'setting params'
setMDSParams(ser, 'X', '0.22', '0.000010', '10.0', '20.0')
setMDSParams(ser, 'Y', '0.35', '0.000020', '15.0', '20.0')
time.sleep(1)

print 'starting'
sendCommand(ser, 'start')
time.sleep(1)

print 'moving'
#should go
setMDSLocation(ser, 300, 25.0)
time.sleep(5)
setMDSLocation(ser, 300, 150.0)
time.sleep(2)
setMDSLocation(ser, 300, 25.0)
time.sleep(2)

print 'stopping'
sendCommand(ser, 'stop')
time.sleep(1)

print 'closing serial'
ser.close()