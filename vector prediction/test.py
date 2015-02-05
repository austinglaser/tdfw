from __future__ import division
import numpy as np
import cv2
import math

def drawArena (X_goal_in, Y_max_in, Y_min_in, img):
	X_goal = int(X_goal_in)
	Y_max = int(Y_max_in)
	Y_min = int(Y_min_in)

	cv2.line(img, (X_goal, Y_max), (X_goal, Y_min), (30, 30, 255))
	cv2.line(img, (X_goal, Y_max), (500, Y_max), (30, 30, 255))
	cv2.line(img, (X_goal, Y_min), (500, Y_min), (30, 30, 255))
	return

def predictPuck (X_goal, Y_max, Y_min, delX, delY):

	#x/y last are for propagation
	X_last = X_prev
	X_next = 0
	Y_last = Y_prev
	Y_next = 0
	bounces = 0
	lastBouncePt = (0, 0)
	default_return = (int(X_goal), int((Y_max+Y_min)/2))

	if (delX >= 0): #the puck is moving away from the robot, or completely vertically
		print 'invalid movement'
		return default_return

	if ((math.sqrt(math.pow(delX, 2) + math.pow(delY, 2))) < 10): #the puck isn't moving or is moving very slowly
		'puck isnt moving'
		return default_return

	#normalize the step vaues to 1 px
	stepX = -1
	stepY = -(delY/delX)

	i = 0

	while(1):
		i = i+1
		if (i>500):
			print 'max iterations reached. aborting'
			return default_return

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
				print 'too many bounces. aborting'
				return default_return

			bounces = bounces + 1

		lastBouncePt = (int(X_next), int(Y_next))
		Y_last = Y_next 
		X_last = X_next

	return default_return

image = cv2.imread('moved_2.jpg')
height , width , layers =  image.shape
fourcc = cv2.VideoWriter_fourcc(*'XVID')
video = cv2.VideoWriter('output.avi',fourcc, 60.0, (width,height))

i=0
#values which keep track of where the puck was last frame. THESE ARE MADE UP RIGHT NOW
#X_prev = 300
#Y_prev = 50
for y in range(60, 220, 5):
	for x in range (350, 180, -1): #do a test
		image = cv2.imread('moved_2.jpg')
		X_prev = x
		Y_prev = y
		print 'x position: ', x
		print 'y position: ', Y_prev

		boundaries = [
			([50, 40, 205], [70, 60, 235])
		]

		# loop over the boundaries which actually doesn't matter right now, it only runs once
		for (lower, upper) in boundaries:
			# create NumPy arrays from the boundaries
			lower = np.array(lower, dtype = "uint8")
			upper = np.array(upper, dtype = "uint8")
		 
			# find the colors within the specified boundaries and apply
			# the mask
			mask = cv2.inRange(image, lower, upper)
			kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
			mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
			output = cv2.bitwise_and(image, image, mask = mask)

			Omoments = cv2.moments(mask)
			dM01 = Omoments['m01']
			dM10 = Omoments['m10']
			dArea = Omoments['m00']

			if dArea > 10000: #the puck is on the screen
				posX = int(dM10/dArea)
				posY = int(dM01/dArea)
				print 'current position: ', posX, posY
				deltaX = posX - X_prev
				deltaY = posY - Y_prev
				xp,yp = predictPuck(30, 60, 220, deltaX, deltaY)
				print 'predicton: ', xp, yp

				#previous puck
				cv2.circle(image, (X_prev, Y_prev), 10, (0, 0, 255), -1)
				#current puck
				cv2.circle(output, (posX, posY), 2, (255, 255, 255), -1)
				#predicted location
				cv2.circle(image, (xp, yp), 4, (0, 255, 0), -1)

			# show the images
			drawArena(30, 60, 220, image)
			video.write(image)

cv2.destroyAllWindows()
#del video # this makes a working AVI
video.release()

	#cv2.imshow("images", np.hstack([image, output]))
	#cv2.waitKey(0)