import threading
import time
import serial

def Serial1Thread():
	global threadTime
	global port1Data
	global data1Ready
	data1Ready = False

	ser = serial.Serial('/dev/tty.usbmodemfd121', 115200, timeout=1)

	while(~ExitFlag):

		if not data1Ready:
			port1Data = ser.read(1)  #reads only 1 byte!

		if not port1Data:
			data1Ready = False
		else:
			data1Ready = True

		if(ExitFlag):
			#close the serial port
			ser.close()
			break

def main():
	global ExitFlag
	global data1Ready
	data1Ready = False
	ExitFlag = False

	thread = threading.Thread(target=Serial1Thread)
	thread.start()

	while(1):
		try:
			if(data1Ready):
				print port1Data
				data1Ready = False
			#wait 5 seconds between checking	
			time.sleep(5)
		except KeyboardInterrupt:
			ExitFlag = True
			thread.join()
			break
#main function
main()