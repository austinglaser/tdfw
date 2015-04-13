from __future__ import division
import numpy as np
import cv2
import math
import os
import time
import serial
import socket
import sys
import threading
import errno
import select
from datetime import datetime

def doCountdown(time_sleep):
	for i in range(time_sleep, 0, -1):
		print i, '...'
		time.sleep(1)
	return

def doAverage(FIFO, newSample):
	fifo_sum = 0

	#fill the FIFO
	for x in range(len(FIFO)-1, 0, -1):
		FIFO[x] = FIFO[x-1]
		fifo_sum = fifo_sum + FIFO[x]

	FIFO[0] = newSample
	fifo_sum = fifo_sum + newSample

	return (FIFO, (fifo_sum/len(FIFO)))

def locatePuck(image, boundaries, X_goal, Y_max, Y_min):

	#unsharp_mask = cv2.blur(image, (2, 2))
 	#image = cv2.addWeighted(image, 1.5, unsharp_mask, -0.5, 0.0)

	default_return = (int(X_goal), int((Y_max+Y_min)/2))

	# loop over the boundaries which actually doesn't matter, it only runs once
	for (lower, upper) in boundaries:
		# create NumPy arrays from the boundaries
		lower = np.array(lower, dtype = "uint8")
		upper = np.array(upper, dtype = "uint8")

		# find the colors within the specified boundaries and apply the mask
		mask = cv2.inRange(image, lower, upper)
		kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
		mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
		kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(2,2))
		mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
		output = cv2.bitwise_and(image, image, mask = mask)

		Omoments = cv2.moments(mask)
		dM01 = Omoments['m01']
		dM10 = Omoments['m10']
		dArea = Omoments['m00']

		if dArea > 10000: #the puck is on the screen
			posX = int(dM10/dArea)
			posY = int(dM01/dArea)

			cv2.circle(output, (int(posX), int(posY)), 2, (0, 0, 255), -1)
			cv2.imshow("images", np.hstack([image, output]))
			cv2.waitKey(1)

			return (posX, posY)

		else:
			return default_return

def predictPuck (X_goal, Y_max, Y_min, delX, delY):
    global image
    global X_prev
    global Y_prev

    #x/y last are for propagation
    #there is a bug where there cannot be an initial reutrn value if ther has been no movement?
    #also no preductiin if there isnt a puc on screen

    X_last = X_prev
    X_next = int(X_goal) #0
    Y_last = Y_prev
    Y_next = int((Y_max+Y_min)/2) #0
    bounces = 0
    lastBouncePt = (0, 0)
    default_return = (int(X_goal), int((Y_max+Y_min)/2))

    if (delX >= 0): #the puck is moving away from the robot, or completely vertically
        #print 'invalid movement'
        return (int(X_goal), int(Y_prev + delY))
        #return default_return

    if ((math.sqrt(math.pow(delX, 2) + math.pow(delY, 2))) < 2): #the puck isn't moving or is moving very slowly
        #print 'puck isnt moving'
        return (int(X_goal), int(Y_prev + delY))

    #normalize the step vaues to 1 px
    stepX = -2 #-1
    stepY = -(delY/delX)

    i = 0

    while(1):
        i = i+1
        if (i>500):
            #print 'max iterations reached. aborting'
            return (int(X_goal), int(Y_prev + delY))
            #return default_return

        Y_next = Y_last + stepY #forward prediction
        X_next = X_last + stepX

        #in each case, draw a pixel to create a line. (DEBUG)
        cv2.circle(image, (int(X_next), int(Y_next)), 1, (0, 0, 255), -1)

        if (X_next <= X_goal): #the puck will reach the goal line
            return (int(X_next), int(Y_next)) #change this to a dict maybe?

        if ((Y_next >= Y_min) or (Y_next <= Y_max)): #there is a bounce

            #"bounce" the puck
            stepY = -stepY

            if (bounces > 1):
                #print 'too many bounces. aborting'
                return (int(X_goal), int(Y_prev + delY))
                #return default_return

            bounces = bounces + 1

        lastBouncePt = (int(X_next), int(Y_next))
        Y_last = Y_next 
        X_last = X_next

    return default_return

def calcPlayingField(x, y):
	if len(x) < 4 or len(y) < 4:
		#error
		return (0, 0, 0)
	else:
		Y_max = (y[0] + y[3])/2 - 5
		Y_min = (y[1] + y[2])/2 + 5
		X_goal = (x[0] + x[1])/2 + 15
		return X_goal, Y_max, Y_min,

def drawArena (X_goal_in, Y_max_in, Y_min_in, img):
	X_goal = int(X_goal_in)
	Y_max = int(Y_max_in)
	Y_min = int(Y_min_in)

	cv2.line(img, (X_goal, Y_max), (X_goal, Y_min), (30, 30, 255))
	cv2.line(img, (X_goal, Y_max), (500, Y_max), (30, 30, 255))
	cv2.line(img, (X_goal, Y_min), (500, Y_min), (30, 30, 255))
	return

def createCalFactor(x_min_px, x_max_px, x_min_mm, x_max_mm):
	return (x_max_mm - x_min_mm)/(x_max_px - x_min_px)

def pxToMM(px, calFactor, yOffset):
	return (px - yOffset)*calFactor

def checkCalibration(x_arr, y_arr, low, high):
	if(len(y_arr) != 4 or len(x_arr) != 4):
		#incorrect parameters
		return False
	for y in y_arr:
		#check to make sure that there is no (0,0) point
		if y == 0:
			return False
	for x in x_arr:
		#check to make sure that there is no (0,0) point
		if x == 0:
			return False
	if(low == [0, 0, 0] or high == [0, 0, 0]):
		#the color detection failed
		return False
	return True

def doMDScalibration(debug):
	retVal = (0, 0)
	cap = cv2.VideoCapture(1) 
	cap.set(10,-0.5) #set brightness
	cap.set(12,0.8) #set brightness
	ret, frame = cap.read()

	#unsharp mask
 	unsharp_mask = cv2.blur(frame, (2, 2))
 	frame = cv2.addWeighted(frame, 1.5, unsharp_mask, -0.5, 0.0)

 	#contrast enchancement
 	array_alpha = np.array([1.2])
	array_beta = np.array([-30.0])
    # add a beta value to every pixel 
	cv2.add(frame, array_beta, frame)                    
    # multiply every pixel value by alpha
	cv2.multiply(frame, array_alpha, frame)  

	boundaries = [([0, 150, 180], [10, 205, 230])]	#very rough color estimation, no absolute color detection
	# loop over the boundaries which actually doesn't matter right now, it only runs once
	for (lower, upper) in boundaries:
		# create NumPy arrays from the boundaries
		lower = np.array(lower, dtype = "uint8")
		upper = np.array(upper, dtype = "uint8")
	 
		# find the colors within the specified boundaries and apply
		# the mask
		mask = cv2.inRange(frame, lower, upper)
		kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(6,8))
		mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
		kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
		mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
		output = cv2.bitwise_and(frame, frame, mask = mask)

		Omoments = cv2.moments(mask)
		dM01 = Omoments['m01']
		dM10 = Omoments['m10']
		dArea = Omoments['m00']

		if dArea > 8000: #the dot is on the screen
 				posX = int(dM10/dArea)
 				posY = int(dM01/dArea)
 				retVal = (posX, posY)
 				#print a circle if the indicator was detected
 				cv2.circle(frame, (posX, posY), 8, (0, 255, 255), -1)

 	if(debug):
 		#debug for showing the image to ensure the puck is detected
		cv2.imshow("images", np.hstack([frame, output]))
		cv2.waitKey(0)

	cap.release()
	cv2.destroyAllWindows()
	print retVal
	return retVal

def doPuckCalibration(debug): 
	#first detect center puck

	thresh_low = [0, 0, 0]
	thresh_high = [0, 0, 0]

	cap = cv2.VideoCapture(1)

	cap.set(3,320)
	cap.set(4,240)
	cap.set(5,125)
	cap.set(10,-0.5) #set brightness
	cap.set(12,0.8) #set brightness

	frames = 0

	while frames < 100:
		ret, frame = cap.read()
		frames = frames + 1
		print frames

	#probably needs some contrast adjustment for this part....NOT ON FRAME_CROP!
	#changing the contrast isn't good if it's the final color detection frame.
	#divide the image into thirds. The inner circle should be in the middle third.
	crop_y = len(frame)
	crop_x = len(frame[0])
	frame_crop =  frame[0:crop_y, crop_x/3:2*crop_x/3]
	#do the contrast change here

	array_alpha = np.array([1.5])
	array_beta = np.array([-50.0])
    # add a beta value to every pixel 
	cv2.add(frame_crop, array_beta, frame_crop)                    
    # multiply every pixel value by alpha
	cv2.multiply(frame_crop, array_alpha, frame_crop)  
	
	#detect the innermost circle to find the puck inside it.
	temp = frame_crop.copy()
	temp = cv2.cvtColor(frame_crop, cv2.COLOR_BGR2GRAY)

	circles = cv2.HoughCircles(temp, cv2.HOUGH_GRADIENT, 1.2, 50)
	cropWidth = 0
	crop_center = (0, 0)

	# ensure at least some circles were found
	if circles is not None:
		# convert the (x, y) coordinates and radius of the circles to integers
		circles = np.round(circles[0, :]).astype("int")
 
		# loop over the (x, y) coordinates and radius of the circles
		for (x, y, r) in circles:
			if r>30:
				cropWidth = int(math.sqrt((math.pow(r, 2)/2))) #width of rect
				#the image crop is centered at x,y and has a length of a*2
				crop_center = (x, y)
 		
 	if ((cropWidth != 0) and crop_center != (0, 0)) or (crop_y is 240):
 		#crop to the inner circle. BAM!

 		if crop_y == 240:
 			#recreate the image with no contrast adjustments 
			frame_crop =  frame[0:crop_y, crop_x/3:2*crop_x/3]
 			frame_crop =  frame_crop[108-14:108+14, 55-14:55+14]

 		else:
	 		#recreate the image with no contrast adjustments 
	 		frame_crop =  frame[0:crop_y, crop_x/3:2*crop_x/3]
	 		frame_crop =  frame_crop[y-cropWidth:y+cropWidth, x-cropWidth:x+cropWidth]

 		#unsharp mask
 		# unsharp_mask = cv2.blur(frame_crop, (2, 2))
 		# frame_crop = cv2.addWeighted(frame_crop, 1.5, unsharp_mask, -0.5, 0.0)

		#another option
		boundaries = [([0, 0, 0], [255, 255, 255])] #very rough color estimation, no absolute color detection
		# loop over the boundaries which actually doesn't matter right now, it only runs once
		for (lower, upper) in boundaries:
			# create NumPy arrays from the boundaries
			lower = np.array(lower, dtype = "uint8")
			upper = np.array(upper, dtype = "uint8")
		 
			# the mask
			mask = cv2.inRange(frame_crop, lower, upper)
			kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(10,10))
			mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
			output = cv2.bitwise_and(frame_crop, frame_crop, mask = mask)

			if(debug):
				#debug
				#cv2.imshow("output", output)
				cv2.imshow("images", np.hstack([frame_crop, output]))
				cv2.waitKey(0)

			Omoments = cv2.moments(mask)
			dM01 = Omoments['m01']
			dM10 = Omoments['m10']
			dArea = Omoments['m00']

			if dArea > 10000: #the puck is on the screen
	 				posX = int(dM10/dArea)
	 				posY = int(dM01/dArea)
	 				colors = [10, 10, 10, 10]

	 				increment = 50;
	 				colors = frame_crop[posX, posY]
	 				#make sure we won't overrun anything
	 				for x in range(0,3):
		 				if colors[x] + increment > 255:
		 					thresh_high[x] = 255;
		 				else:
		 					thresh_high[x] = colors[x] + increment

		 				if colors[x] - increment < 0:
		 					thresh_low[x] = 0;
		 				else:
		 					thresh_low[x] = colors[x] - increment

	 				#now do a second pass for better color detection
	 				boundaries = [(thresh_low, thresh_high)] #absolute color detection
					for (lower, upper) in boundaries:
						# create NumPy arrays from the boundaries
						lower = np.array(lower, dtype = "uint8")
						upper = np.array(upper, dtype = "uint8")
					 
						# find the colors within the specified boundaries and apply
						frame_crop = cv2.blur(frame_crop, (1, 1))
						mask = cv2.inRange(frame_crop, lower, upper)
						kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(15,15))
						mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
						output = cv2.bitwise_and(frame_crop, frame_crop, mask = mask)

						avg = (cv2.mean(output, mask = mask))

						increment = 40;
	 					#make sure we won't overrun anything
		 				for x in range(0,2):
			 				if avg[x] + increment > 255:
			 					thresh_high[x] = 255;
			 				else:
			 					thresh_high[x] = int(avg[x] + increment)

			 				if avg[x] - increment < 0:
			 					thresh_low[x] = 0;
			 				else:
			 					thresh_low[x] = int(avg[x] - increment)

						#thresh_low = (int(avg[0]) - 10, int(avg[1]) - 10, int(avg[2]) - 20)
	 					#thresh_high = (int(avg[0]) + 10, int(avg[1]) + 10, int(avg[2]) + 20)

			else: 
 				print 'no puck detected. calibration failed'
 				return ([0, 0, 0], [0, 0, 0])
 		if(debug):
			# show the output image
			#cv2.imshow("output", output)
			cv2.imshow("images", np.hstack([frame_crop, output]))
			cv2.waitKey(0)

	else:print 'error detecting center of arena, dawg. you messed up.'

	cap.release()
	cv2.destroyAllWindows()
	return (thresh_low, thresh_high)

def drawArena (X_goal_in, Y_max_in, Y_min_in, img):
	X_goal = int(X_goal_in)
	Y_max = int(Y_max_in)
	Y_min = int(Y_min_in)

	cv2.line(img, (X_goal, Y_max), (X_goal, Y_min), (30, 30, 255))
	cv2.line(img, (X_goal, Y_max), (500, Y_max), (30, 30, 255))
	cv2.line(img, (X_goal, Y_min), (500, Y_min), (30, 30, 255))
	return

#comms functions
def setMDSParams(SerialStream, channel, Kp, Ki, Kd, Sat): #accepts strings
	IsError = False
	ErrorString = ''
	sendStr = 'MR' + channel + ':' + Kp + ',' + Ki +',' + Kd +',' + Sat + '\n'
	SerialStream.write(sendStr)

	returnString = SerialStream.readline()
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
	print sendStr
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

def connect():
	global s
	tcp_ip = '192.168.2.1'
	tcp_port = 5005

	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

	connected = False
	while not connected:
		try:
			s.connect((tcp_ip, tcp_port))
			return s
			connected = True
		except socket.error as serr:
			if serr.errno == errno.ECONNREFUSED:
				print "connection refused"
				time.sleep(1)
 			else:
 				print serr

def listen():
	global s
	global listen_should_exit
	global dataFromUI

	buffer_size = 1024

	while True:
		ready = select.select([s], [], [], 1)[0]
		if (ready):
			try:
				dataFromUI = s.recv(buffer_size)
				print dataFromUI,
				if dataFromUI == "UF\n":
					s.close()
					s = connect()
			except socket.error as serr:
				if serr.errno == errno.ECONNRESET:
					print "connection reset. Retrying connection"
					s = connect()
				else:
					raise serr
		if listen_should_exit:
			break;

#main loop
global image
global X_prev
global Y_prev
global ddataFromUI

#local variables
serialEnable = False
doCalibrate = False
x_avg_5 = 0;
y_avg_5 = 0;
x_send = 0;
y_send = 0;
y_array =  [0] * 1;
x_array =  [0] * 1;
calFactor = 0;
strikeFlag = False
y_meow = 25.0
state = "startup"
difficulty = 'easy'

#x and y calibration array
y = [0] * 4;
x = [0] * 4;

#comms vars
dataFromUI = ''

if (doCalibrate == False):
	goal, ymax, ymin, low, high = 100, 70.0, 350.0, [0, 210, 0], [200, 255, 20] #[0, 210, 0] [200, 255, 20]

while True:
	#states: startup, idle, calibrate, play, shutdown. ctl-c also enters the shutdown state. 
	if state == "startup":
		#invoke startup code
		print 'startup'

		#open TCP
		global s
		global listen_should_exit

		listen_should_exit = False
		listen_thread = threading.Thread(target=listen)
		listen_thread.setDaemon(True)

		s = connect()

		listen_thread.start()

		#open serial to MDS 
		ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

		state = "idle"

	elif state == "calibrate":
		#calibrate code. 
		print 'calibrate'
		#TODO: add puck caibration into the system 
		sendCommand(ser, 'calibrate')

		while dataFromUI != 'UC1\n':
			time.sleep(0.1)

		while dataFromUI != 'UC2\n':
			time.sleep(0.1)

		x[1], y[1] = doMDScalibration(False)

		while dataFromUI != 'UC3\n':
			time.sleep(0.1)

		x[2], y[2] = doMDScalibration(False)

		while dataFromUI != 'UC4\n':
			time.sleep(0.1)

		x[3], y[3] = doMDScalibration(False)

		while dataFromUI != 'UCD\n':
			time.sleep(0.1)

		x[0], y[0] = doMDScalibration(False)

		sendCommand(ser, 'calibration done')

		#check that the calibration worked properly
		goal, ymax, ymin = calcPlayingField(x,y)
		print goal/2, ymax/2, ymin/2

		#TODO: add error comms info if the calibration did not work properly.

		success = checkCalibration(x, y, low, high )
		if success == True:
			#create a calfactor using the values from the MDS
			calFactor = createCalFactor(y[0]/2, y[1]/2, x_calibration_min, x_calibration_max)
			print calFactor #should be ~4
			print 'calibration complete'

		else:
			calFactor = 0
			print 'calibraton failed'

		cap = cv2.VideoCapture(1)
		cap.set(3,320) #set height
		cap.set(4,240) #set width
		cap.set(5,125) #set framerate
		cap.set(10,-0.5) #set brightness
		cap.set(12,0.8) #set set saturation
		ret, image = cap.read()

		drawArena(goal/2, ymax/2, ymin/2, image)
		cv2.imshow("image", image)

		cap.release()

		cv2.waitKey(0)
		cv2.destroyAllWindows()

		state = 'idle'

	elif state == "play":
		#play code
		print 'play'
		#values which keep track of where the puck was last frame.
		X_prev = 0
		Y_prev = 0

		#TODO: change the parameters based on what the difficulty level is.
		print 'setting params'
		print difficulty

		if difficulty == 'easy':
			#TODO: ADD BOUNDARY CLOSURE
			setMDSParams(ser, 'X', '0.1', '0.0000100', '17.0', '20.0')
			setMDSParams(ser, 'Y', '0.35', '0.000020', '15.0', '20.0')

		elif difficulty == 'medium':
			#TODO: ADD BOUNDARY CLOSURE
			setMDSParams(ser, 'X', '0.15', '0.000010', '15.0', '20.0')
			setMDSParams(ser, 'Y', '0.35', '0.000020', '15.0', '20.0')

		elif difficulty == 'hard': 
			#TODO: ADD BOUNDARY CLOSURE
			setMDSParams(ser, 'X', '0.22', '0.000010', '10.0', '20.0')
			setMDSParams(ser, 'Y', '0.35', '0.000020', '15.0', '20.0')

		else:
			#default, easy
			setMDSParams(ser, 'X', '0.15', '0.000010', '15.0', '20.0')
			setMDSParams(ser, 'Y', '0.35', '0.000020', '15.0', '20.0')

		while dataFromUI != 'US\n':
			time.sleep(0.1)

		print 'starting'
		sendCommand(ser, 'start')
		time.sleep(1)

		#open the video stream
		cap = cv2.VideoCapture(1)

		cap.set(3,320) #set height
		cap.set(4,240) #set width
		cap.set(5,125) #set framerate
		cap.set(10,-0.5) #set brightness
		cap.set(12,0.8) #set set saturation

		while(dataFromUI != 'UP\n'):
			try:
				ret, image = cap.read()

				boundaries = [(low, high)]

				locX, locY = locatePuck(image, boundaries, goal/2, ymax/2, ymin/2)

				if locX < 55:
					#don't predict the puck location, because it's behind the MDS
					xp,yp = (int(goal/2), int(((ymax/2)+(ymin/2))/2))
					y_meow = 30.0

				else:
					#done in main every frame
					deltaX = locX - X_prev
					deltaY = locY - Y_prev

					if (deltaX < 0 and locX in range(80, 120) or ((deltaX >= 0) and locX in range(80, 100))):
						#set the mds y to 100
						if deltaX < 0:
							y_meow = 125

						# elif ((math.sqrt(math.pow(deltaX, 2) + math.pow(deltaY, 2))) < 2):
						# 	#the puck isn't moving
						# 	y_meow = 2.5*locX

						# else: #go to the puck's location
						# 	y_meow = 125
						#record time the strike happened
						if not strikeFlag:
							strikeTime = datetime.now()
						#set the strike flag
						strikeFlag = True

					else:
						#normal y placement
						y_meow = 30.0
						strikeFlag = False

					if strikeFlag:
						#has 0.X seconds passed?
						finalTime = datetime.now()
						c = finalTime - strikeTime
						elapsed = (c.days * 24 * 60 * 60 + c.seconds) * 1000 + c.microseconds / 1000.0 #in ms
						print elapsed
						if elapsed > 700:
							#reset the striking
							y_meow = 30.0
							strikeFlag = False

					xp,yp = predictPuck(goal/2, ymax/2, ymin/2, deltaX, deltaY)
					#print 'predicton: ', xp, yp

				#filter the predicted output. Do we actually want to use this?
				x_array, x_avg_5 = doAverage(x_array, xp)
				y_array, y_avg_5 = doAverage(y_array, yp)
				x_send = x_avg_5
				y_send = y_avg_5

				X_prev = locX
				Y_prev = locY

				#the final point to send is in x,y format. 
				#since we are always hovering at x = 50mm, we don't even need to calfactor anything.
				#just send it 50mm unless the MDS is striking.
				finalpt = pxToMM(y_send, calFactor, ymax/2)

				error, errorStr = setMDSLocation(ser, finalpt, y_meow)

				#show the images, bring these back to watch the fun in realtime!

				cv2.circle(image, (int(x_send), int(y_send)), 4, (255, 255, 0), -1)
				drawArena(goal/2, ymax/2, ymin/2, image)
				cv2.imshow("image", image)
				cv2.waitKey(1)

			except KeyboardInterrupt:
				#send stop command
				#close serial port
				cap.release()
				print 'stopping mds'
				sendCommand(ser, 'stop')
				print 'closing serial'
				ser.close()
				listen_should_exit = True
				break

		#the stop game command came in, so stop.
		#release the capture
		cap.release()
		sendCommand(ser, 'stop')

		cv2.destroyAllWindows()

		print 'game finished'

		state = 'idle'

		#set the parameters based on the difficulty selected

	elif state == "shutdown":
		#shutdown code
		print 'shutdown'
		#close the serial
		print 'closing serial'
		listen_should_exit = True
		ser.close()
		state = 'idle'

	else:
		print 'idle'
		#idle code, default case
		while True:
			if dataFromUI == 'UCS\n':
				state = "calibrate"
				break
			if dataFromUI == 'UD1\n':
				#TODO:set the tuning parameters before starting the game
				state = "play"
				difficulty = 'easy'
				break
			if dataFromUI == 'UD2\n':
				#TODO:set the tuning parameters before starting the game
				state = "play"
				difficulty = 'medium'
				break
			if dataFromUI == 'UD3\n':
				#TODO:set the tuning parameters before starting the game
				state = "play"
				difficulty = 'hard'
				break
			if dataFromUI == 'UF\n':
				state = "shutdown"
				break

