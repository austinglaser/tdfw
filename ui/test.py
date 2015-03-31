#!/usr/bin/python
import Tkinter
from Tkinter import *
import tkMessageBox
import PIL
from PIL import ImageTk
from PIL import Image
import threading
import time
from sys import stdin

top = Tkinter.Tk()              # I dunno what this does... but it makes everything work
T 	= Text(top, height=2, width=30)

path1  = "CalibrateStep1.jpg"
path2  = "CalibrateStep2.jpg"
path3  = "CalibrateStep3.jpg"
path4  = "CalibrateStep4.jpg"
path5  = "StartUpScreen.jpg"
path6  = "0.jpg"
path7  = "1.jpg"
path8  = "2.jpg"
path9  = "3.jpg"
path10 = "Winner.jpg"
path11 = "Loser.jpg"
path12 = "Score.jpg"
path13  = "0.jpg"
path14  = "1.jpg"
path15  = "2.jpg"
path16  = "3.jpg"

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
Calibrate = 0					# Calibrate value to write to controller for calibration start
mode 	  = 0					# Mode value to write to controller for difficulty setting set up
AHAscore  = 0
Yourscore = 0

def BG_Thread():
	global userinput
	global userScore
	userScore = 0
	global AHAScore
	AHAScore = 0

	while(~ExitFlag):
		userinput = stdin.readline()

		if "u" in userinput:
			userScore = userScore + 1
		if "a" in userinput:
			AHAScore = AHAScore + 1

		time.sleep(0.05)

		if(ExitFlag):
			break

def main():
	global ExitFlag
	ExitFlag = False

	top = Tkinter.Tk()              # I dunno what this does... but it makes everything work
	T 	= Text(top, height=2, width=30)

	path1  = "CalibrateStep1.jpg"
	path2  = "CalibrateStep2.jpg"
	path3  = "CalibrateStep3.jpg"
	path4  = "CalibrateStep4.jpg"
	path5  = "StartUpScreen.jpg"
	path6  = "0.jpg"
	path7  = "1.jpg"
	path8  = "2.jpg"
	path9  = "3.jpg"
	path10 = "Winner.jpg"
	path11 = "Loser.jpg"
	path12 = "Score.jpg"
	path13  = "0.jpg"
	path14  = "1.jpg"
	path15  = "2.jpg"
	path16  = "3.jpg"

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
	Calibrate = 0					# Calibrate value to write to controller for calibration start
	mode 	  = 0					# Mode value to write to controller for difficulty setting set up


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
	top.geometry("400x272")
	top.configure(background='white')

	Start.pack()					# Whole program begins here where the Start button appears alone

	top.mainloop()

	while True:
		try:


		except KeyboardInterrupt:
			ExitFlag = True
			thread.join()
			break

def Welcome():					# Window that appears after initialization
	Start.pack_forget()			# Wipe all buttons from previous window
	Easy.pack_forget()
	Med.pack_forget()
	Hard.pack_forget()
	Back1.pack_forget()
	panel4.pack_forget()
	Finish.pack_forget()
	panel10.pack_forget()
	panel11.pack_forget()

	panel5.pack()
	Cal.pack()					# Set buttons for Calibrate, Turn Off, and Choose Difficulty options
	OFF.pack()
	Dif.pack()
	top.title("Welcome")

def Calibrate():				# When Calibrate button is triggered, come here 
    Calibrate = 1

    Cal.pack_forget()			# Wipe all buttons from previous window
    OFF.pack_forget()
    Dif.pack_forget()
    panel5.pack_forget()

    Next1.pack()
    top.title("Calibration")

    T.pack()
    T.insert(END, "Perform the illustrated step\nand press Next\n")
    mainloop()


def Next1():
	Next1.pack_forget()
	T.delete(1.0, END)
	T.pack_forget()

	Next2.pack()

	top.title("Step 1")
	panel1.pack()

def Next2():
	Next2.pack_forget()
	panel1.pack_forget()

	Next3.pack()

	top.title("Step 2")
	panel2.pack()


def Next3():
	Next3.pack_forget()
	panel2.pack_forget()

	Next4.pack()

	top.title("Step 3")
	panel3.pack()

def Next4():
	Next4.pack_forget()
	panel3.pack_forget()

	Finish.pack()

	top.title("Step 4")
	panel4.pack()

def turnOff():					# When Turn Off button is chosen, come here

	tkMessageBox.showinfo( "Fuck You", "Turning Off...")
 
 	PowerDown = 1				# Set Power Down flag to 1
								# Write Power Down flag to controller

def DifficultySetting():		# When Choose Difficulty button is triggered, come here
	Cal.pack_forget()			# Wipe all buttons from previous window
	OFF.pack_forget()
	Dif.pack_forget()
	Back2.pack_forget()
	StartGame.pack_forget()
	panel5.pack_forget()

	top.title("Choose a Level")
	Easy.pack()
	Med.pack()
	Hard.pack()
	Back1.pack()				# Back option returns to Welcome Menu

def stage1():					# When Easy button is triggered, come here
	Easy.pack_forget()			# Wipe all buttons from previous window
	Med.pack_forget()
	Hard.pack_forget()
	Back1.pack_forget()

	mode = 1					# Set Mode flag to 1
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.pack()			# Set buttons for start game or go back
	Back2.pack()

def stage2():					# When Medium button is triggered, come here
	Easy.pack_forget()			# Wipe all buttons from previous window
	Med.pack_forget()
	Hard.pack_forget()
	Back1.pack_forget()

	mode = 2					# Set Mode flag to 2
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.pack()			# Set buttons for start game or go back
	Back2.pack()

def stage3():					# When Hard button is triggered, come here
	Easy.pack_forget()			# Wipe all buttons from previous window 
	Med.pack_forget()
	Hard.pack_forget()
	Back1.pack_forget()

	mode = 3					# Set Mode flag to 3
								# Write Mode flag to controller

	top.title("Ready to Play?")
	StartGame.pack()			# Set buttons for start game or go back
	Back2.pack()				

def Playtime():					# When Start Game button is triggered, come here
	Back2.pack_forget()			# Wipe all buttons from previous window
	StartGame.pack_forget()
	top.title("Scores")

	panel12.pack()
	panel6.pack()
	panel13.pack()
	panel13.place(x = 250, y = 125)
	panel6.place(x = 40, y = 125)
	Scores()

def Scores(): #change

	#start the scorekeeping thread
	thread = threading.Thread(target=BG_Thread)
	thread.start()

	if AHAScore == 3:
		ExitFlag = True
		thread.join()
		Loser()

	if userScore == 3:
		ExitFlag = True
		thread.join()
		Winner()

def Winner():
		panel6.destroy()
		panel7.destroy()
		panel8.destroy()
		panel9.destroy()
		panel12.destroy()
		panel13.destroy()
		panel14.destroy()
		panel15.destroy()
		panel16.destroy()

		panel10.pack()

		time.sleep(10)

		Welcome()

def Loser():
		panel6.destroy()
		panel7.destroy()
		panel8.destroy()
		panel9.destroy()
		panel12.destroy()
		panel13.destroy()
		panel14.destroy()
		panel15.destroy()
		panel16.destroy()

		panel11.pack()

		time.sleep(10)

		Welcome()