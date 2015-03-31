#!/usr/bin/python
import Tkinter
from Tkinter import *
import tkMessageBox
import PIL
from PIL import ImageTk
from PIL import Image

top = Tkinter.Tk()              # I dunno what this does... but it makes everything work
T 	= Text(top, height=2, width=30)

path1 = "CalibrateStep1.jpg"
path2 = "CalibrateStep2.jpg"
path3 = "CalibrateStep3.jpg"
path4 = "CalibrateStep4.jpg"
path5 = "StartUpScreen.jpg"

img1 	= ImageTk.PhotoImage(Image.open(path1))
img2 	= ImageTk.PhotoImage(Image.open(path2))
img3 	= ImageTk.PhotoImage(Image.open(path3))
img4 	= ImageTk.PhotoImage(Image.open(path4))
img5 	= ImageTk.PhotoImage(Image.open(path5))

panel1 	= Tkinter.Label(top, image = img1)
panel2 	= Tkinter.Label(top, image = img2)
panel3 	= Tkinter.Label(top, image = img3)
panel4 	= Tkinter.Label(top, image = img4)
panel5 	= Tkinter.Label(top, image = img5)

PowerDown = 0                   # Power down value to write to controller for system shut off
Calibrate = 0					# Calibrate value to write to controller for calibration start
mode 	  = 0					# Mode value to write to controller for difficulty setting set up

def Welcome():					# Window that appears after initialization
	Start.pack_forget()			# Wipe all buttons from previous window
	Easy.pack_forget()
	Med.pack_forget()
	Hard.pack_forget()
	Back1.pack_forget()
	panel4.pack_forget()
	Finish.pack_forget()

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
	print ("Made it!")


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
top.geometry("400x400")
top.configure(background='white')

Start.pack()					# Whole program begins here where the Start button appears alone

top.mainloop()