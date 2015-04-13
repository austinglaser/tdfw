from __future__ import division
import cv2
import time
from datetime import datetime

cap = cv2.VideoCapture(1)
cap.set(3,320) #set width
cap.set(4,240) #set height
cap.set(5,125) #set FPS
cap.set(10,-0.5) #set brightness
cap.set(12,0.8) #set brightness

frameCount = 0
startTime = datetime.now()

while True:
	try:
		frameCount = frameCount + 1
		ret, image = cap.read()
		cv2.imshow("image", image)
		cv2.waitKey(1)
	except KeyboardInterrupt:
		finalTime = datetime.now()
		c = finalTime - startTime
		elapsed = (c.days * 24 * 60 * 60 + c.seconds) * 1000 + c.microseconds / 1000.0
		print '\nelapsed time us=', elapsed, 'frames =', frameCount, 'framerate =', 1000*frameCount/(elapsed)
		break