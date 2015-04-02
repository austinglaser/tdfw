#!/usr/bin/env python
#THIS IS FOR REAL
import Tkinter
from Tkinter import *
import tkMessageBox
import PIL
from PIL import ImageTk
from PIL import Image
import threading
import time
from sys import stdin
import os

ExitFlag = False
userinput = ""
userScore = 0
AHAScore = 0

top = Tkinter.Tk()              # I dunno what this does... but it makes everything work
top.overrideredirect(1)
T 	= Text(top, height=2, width=30)

ui_path = os.path.dirname(os.path.realpath(__file__))

path1   = ui_path + "/CalibrateStep1.jpg"
path2   = ui_path + "/CalibrateStep2.jpg"
path3   = ui_path + "/CalibrateStep3.jpg"
path4   = ui_path + "/CalibrateStep4.jpg"
path5   = ui_path + "/StartUpScreen.jpg"
path6   = ui_path + "/0.jpg"
path7   = ui_path + "/1.jpg"
path8   = ui_path + "/2.jpg"
path9   = ui_path + "/3.jpg"
path10  = ui_path + "/Winner.jpg"
path11  = ui_path + "/Loser.jpg"
path12  = ui_path + "/Score.jpg"
path13  = ui_path + "/0.jpg"
path14  = ui_path + "/1.jpg"
path15  = ui_path + "/2.jpg"
path16  = ui_path + "/3.jpg"
path17  = ui_path + "/StartButton.jpg"
path18  = ui_path + "/easy.png"
path19  = ui_path + "/med.png"
path20  = ui_path + "/hard.png"
path21  = ui_path + "/back.png"
path22  = ui_path + "/next.png"
path23  = ui_path + "/calibrate.png"
path24  = ui_path + "/off.png"
path25  = ui_path + "/play.png"
path26  = ui_path + "/done.png"
path27  = ui_path + "/nextcalibrate.png"
path28  = ui_path + "/start2.png"

img1 	= ImageTk.PhotoImage(Image.open(path1))
img2 	= ImageTk.PhotoImage(Image.open(path2))
img3 	= ImageTk.PhotoImage(Image.open(path3))
img4 	= ImageTk.PhotoImage(Image.open(path4))
img5 	= ImageTk.PhotoImage(Image.open(path5))
img6 	= ImageTk.PhotoImage(Image.open(path6))
img7 	= ImageTk.PhotoImage(Image.open(path7))
img8 	= ImageTk.PhotoImage(Image.open(path8))
img9 	= ImageTk.PhotoImage(Image.open(path9))
img10 	= ImageTk.PhotoImage(Image.open(path10))
img11 	= ImageTk.PhotoImage(Image.open(path11))
img12 	= ImageTk.PhotoImage(Image.open(path12))
img13 	= ImageTk.PhotoImage(Image.open(path13))
img14 	= ImageTk.PhotoImage(Image.open(path14))
img15 	= ImageTk.PhotoImage(Image.open(path15))
img16 	= ImageTk.PhotoImage(Image.open(path16))
StartButton = ImageTk.PhotoImage(Image.open(path17))
EasyMode	= ImageTk.PhotoImage(Image.open(path18))
MediumMode	= ImageTk.PhotoImage(Image.open(path19))
HardMode	= ImageTk.PhotoImage(Image.open(path20))
BackPanel	= ImageTk.PhotoImage(Image.open(path21))
NextPanel    = ImageTk.PhotoImage(Image.open(path22))
calibrateimage   = ImageTk.PhotoImage(Image.open(path23))
Off    		= ImageTk.PhotoImage(Image.open(path24))
Difficulty  = ImageTk.PhotoImage(Image.open(path25))
Done		= ImageTk.PhotoImage(Image.open(path26))
NextCalibrate		= ImageTk.PhotoImage(Image.open(path27))
ReturntoStart		= ImageTk.PhotoImage(Image.open(path28))

panel1 	= Tkinter.Label(top, image = img1)
panel2 	= Tkinter.Label(top, image = img2)
panel3 	= Tkinter.Label(top, image = img3)
panel4 	= Tkinter.Label(top, image = img4)
panel5 	= Tkinter.Label(top, image = img5)
panel6 	= Tkinter.Label(top, image = img6)
panel7 	= Tkinter.Label(top, image = img7)
panel8 	= Tkinter.Label(top, image = img8)
panel9 	= Tkinter.Label(top, image = img9)
panel10 = Tkinter.Label(top, image = img10)
panel11 = Tkinter.Label(top, image = img11)
panel12 = Tkinter.Label(top, image = img12)
panel13 = Tkinter.Label(top, image = img13)
panel14 = Tkinter.Label(top, image = img14)
panel15 = Tkinter.Label(top, image = img15)
panel16 = Tkinter.Label(top, image = img16)

PowerDown = 0                   # Power down value to write to controller for system shut off
Calibrate1 = 0					# Calibrate value to write to controller for calibration start
mode 	  = 0					# Mode value to write to controller for difficulty setting set up


def BG_Thread():
	print "thread spawned"
	global userinput
	global userScore
	userScore = 0
	global AHAScore
	AHAScore = 0

	while True:
		#print "meow"
		userinput = stdin.readline()
		print userinput

		if "u" in userinput:
			userScore = userScore + 1
		if "a" in userinput:
			AHAScore = AHAScore + 1

		time.sleep(0.05)

		if(userScore == 3 or AHAScore == 3):
			print "exiting thread"
			break

def Welcome():					# Window that appears after initialization
	Start.place_forget()			# Wipe all buttons from previous window
	Easy.place_forget()
	Med.place_forget()
	Hard.place_forget()
	Back1.place_forget()
	panel4.pack_forget()
	Finish.place_forget()
	panel10.pack_forget()
	panel11.pack_forget()

	Cal.place(x = 0, y = 0)					# Set buttons for Calibrate, Turn Off, and Choose Difficulty options
	Cal.config(image = calibrateimage)
	OFF.place(x = 240, y = 0)
	OFF.config(image = Off)
	Dif.place(x = 120, y = 136)
	Dif.config(image = Difficulty)
	top.title("Welcome")

def Calibrate():				# When Calibrate button is triggered, come here 

    Cal.place_forget()			# Wipe all buttons from previous window
    OFF.place_forget()
    Dif.place_forget()

    Next1.place(x = 120, y = 100)
    Next1.config(image = NextPanel)
    top.title("Calibration")

    T.pack()
    T.insert(END, "Perform the illustrated step\nand press Next\n")
    mainloop()


def Next1():
	Next1.place_forget()
	T.delete(1.0, END)
	T.pack_forget()

	Next2.place(x = 173, y = 200)
	Next2.config(image = NextCalibrate)

	top.title("Step 1")
	panel1.pack()

def Next2():
	Next2.place_forget()
	panel1.pack_forget()

	Next3.place(x = 173, y = 200)
	Next3.config(image = NextCalibrate)

	top.title("Step 2")
	panel2.pack()


def Next3():
	Next3.place_forget()
	panel2.pack_forget()

	Next4.place(x = 173, y = 200)
	Next4.config(image = NextCalibrate)

	top.title("Step 3")
	panel3.pack()

def Next4():
	Next4.place_forget()
	panel3.pack_forget()

	Finish.place(x = 173, y = 200)
	Finish.config(image = Done)

	top.title("Step 4")
	panel4.pack()

def turnOff():					# When Turn Off button is chosen, come here
	os.system(ui_path + "/off.sh")

	top.destroy()
	
 	PowerDown = 1				# Set Power Down flag to 1
								# Write Power Down flag to controller

def DifficultySetting():		# When Choose Difficulty button is triggered, come here
	Cal.place_forget()			# Wipe all buttons from previous window
	OFF.place_forget()
	Dif.place_forget()
	Back2.place_forget()
	StartGame.place_forget()

	top.title("Choose a Level")
	Easy.place(x = 0, y = 0)
	Easy.config(image = EasyMode)
	Med.place(x = 240, y = 0)
	Med.config(image = MediumMode)
	Hard.place(x = 0, y = 136)
	Hard.config(image = HardMode)
	Back1.place(x = 240, y = 136)
	Back1.config(image = BackPanel)				# Back option returns to Welcome Menu

def stage1():					# When Easy button is triggered, come here
	Easy.place_forget()			# Wipe all buttons from previous window
	Med.place_forget()
	Hard.place_forget()
	Back1.place_forget()

	mode = 1					# Set Mode flag to 1
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.place(x = 0, y = 0)			# Set buttons for start game or go back
	StartGame.config(image = Difficulty)
	Back2.place(x = 240, y = 0)
	Back2.config(image = BackPanel)


def stage2():					# When Medium button is triggered, come here
	Easy.place_forget()			# Wipe all buttons from previous window
	Med.place_forget()
	Hard.place_forget()
	Back1.place_forget()

	mode = 2					# Set Mode flag to 2
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.place(x = 0, y = 0)			# Set buttons for start game or go back
	StartGame.config(image = Difficulty)
	Back2.place(x = 240, y = 0)
	Back2.config(image = BackPanel)

def stage3():					# When Hard button is triggered, come here
	Easy.place_forget()			# Wipe all buttons from previous window 
	Med.place_forget()
	Hard.place_forget()
	Back1.place_forget()

	mode = 3					# Set Mode flag to 3
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.place(x = 0, y = 0)			# Set buttons for start game or go back
	StartGame.config(image = Difficulty)
	Back2.place(x = 240, y = 0)
	Back2.config(image = BackPanel)				

def Playtime():					# When Start Game button is triggered, come here
	Back2.place_forget()			# Wipe all buttons from previous window
	StartGame.place_forget()
	top.title("Scores")

	panel12.pack()

	panel6.pack()
	panel13.pack()

	panel13.place(x = 250, y = 125)
	panel6.place(x = 40, y = 125)

	time.sleep(1)

	Scores()

def Scores(): #change

	#start the scorekeeping thread
	global thread
	thread = threading.Thread(target=BG_Thread)
	thread.start()

	loopy = top.after(1000,checkScores)


def checkScores():
	global thread
	global ExitFlag

	print "aha score: ", AHAScore, "userScore", userScore
	loopy = top.after(2000,checkScores)

	if AHAScore == 0:
		#display 0
		panel13.configure(image = img6)

	if AHAScore == 1:
		#display 0
		panel13.configure(image = img7)

	if AHAScore == 2:
		panel13.configure(image = img8)
		#display 0

	if userScore == 0:
		#display 0
		panel6.configure(image = img6)

	if userScore == 1:
		#display 0
		panel6.configure(image = img7)

	if userScore == 2:
		panel6.configure(image = img8)
		#display 0

	if AHAScore >= 3:
		panel13.configure(image = img9)
		#display 0
		if ExitFlag:
			top.after_cancel(loopy)
			thread.join()
			Loser()
		ExitFlag = True

	if userScore >= 3:
		panel6.configure(image = img9)
		#display 0
		if ExitFlag:
			top.after_cancel(loopy)
			thread.join()
			Winner()
		ExitFlag = True


def Winner():
		panel13.place(x = 1000, y = 1000) #user score
		panel6.place(x = 1000, y = 1000)	#aha score
		
		panel12.pack_forget()

		panel10.pack()

		Start.place(x = 200, y = 220)
		Start.config(image = ReturntoStart)

def Loser():
		panel13.place(x = 1000, y = 1000) #user score
		panel6.place(x = 1000, y = 1000)	#aha score
		
		panel12.pack_forget()

		panel11.pack()

		Start.place(x = 200, y = 220)
		Start.config(image = ReturntoStart)


Cal       = Tkinter.Button(top, text = "Calibrate",         command = Calibrate)			# Button set up
OFF       = Tkinter.Button(top, text = "Turn Off",          command = turnOff)
Dif       = Tkinter.Button(top, text = "Choose Difficulty", command = DifficultySetting)
Easy      = Tkinter.Button(top, text = "Easy Mode",         command = stage1)
Med       = Tkinter.Button(top, text = "Medium Mode",       command = stage2)
Hard      = Tkinter.Button(top, text = "Hard Mode",         command = stage3)
Back1     = Tkinter.Button(top, text = "Back",              command = Welcome)
Back2     = Tkinter.Button(top, text = "Back",              command = DifficultySetting)
Start     = Tkinter.Button(top, text = "Start",             command = Welcome)
StartGame = Tkinter.Button(top, text = "Play",              command = Playtime)
Next1     = Tkinter.Button(top, text = "Next",              command = Next1)
Next2     = Tkinter.Button(top, text = "Next",              command = Next2)
Next3     = Tkinter.Button(top, text = "Next",              command = Next3)
Next4     = Tkinter.Button(top, text = "Next",              command = Next4)
Finish    = Tkinter.Button(top, text = "Finish",            command = Welcome)

top.title("AHA! Welcome")
top.geometry("480x272")
top.configure(background='white')

top.focus_set()

Start.place(x = 10, y = 0)					# Whole program begins here where the Start button appears alone
Start.config(image = StartButton)

top.mainloop()
