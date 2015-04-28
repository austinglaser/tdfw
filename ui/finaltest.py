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
import datetime
from sys import stdin
import os
import socket
import select
import struct

def main():
	global userScore
	global AHAScore
	userScore = 0
	AHAScore = 0

	global top
	global T
	top = Tkinter.Tk()              # I dunno what this does... but it makes everything work
	top.overrideredirect(1)
	T 	= Text(top, height=2, width=30)
	
	global ui_path
	ui_path = os.path.dirname(os.path.realpath(__file__))
	
	calibratestep1_img_path			= ui_path + "/CalibrateStep1.jpg"
	calibratestep2_img_path   		= ui_path + "/CalibrateStep2.jpg"
	calibratestep3_img_path   		= ui_path + "/CalibrateStep3.jpg"
	calibratestep4_img_path   		= ui_path + "/CalibrateStep4.jpg"
	startupscreen_img_path			= ui_path + "/StartUpScreen.jpg"
	number0_img_path				= ui_path + "/0.jpg"
	number1_img_path   				= ui_path + "/1.jpg"
	number2_img_path   				= ui_path + "/2.jpg"
	number3_img_path   				= ui_path + "/3.jpg"
	winner_img_path					= ui_path + "/Winner.jpg"
	loser_img_path					= ui_path + "/Loser.jpg"
	score_img_path  				= ui_path + "/Score.jpg"
	start_button_img_path			= ui_path + "/StartButton.jpg"
	easy_button_img_path			= ui_path + "/easy.png"
	med_button_img_path				= ui_path + "/med.png"
	hard_button_img_path			= ui_path + "/hard.png"
	back_button_img_path  			= ui_path + "/back.png"
	next_button_img_path  			= ui_path + "/next.png"
	calibrate_button_img_path		= ui_path + "/calibrate.png"
	off_button_img_path				= ui_path + "/off.png"
	difficulty_button_img_path		= ui_path + "/play.png"
	done_button_img_path			= ui_path + "/done.png"
	nextcalibrate_button_img_path	= ui_path + "/nextcalibrate.png"
	returntostart_button_img_path	= ui_path + "/start2.png"

	global calibratestep1_img
	global calibratestep2_img
	global calibratestep3_img
	global calibratestep4_img
	global startupscreen_img
	global number0_img
	global number1_img
	global number2_img
	global number3_img
	global winner_img
	global loser_img
	global score_img
	global img13
	global img14
	global img15
	global img16
	global start_button_img
	global easy_button_img
	global med_button_img
	global hard_button_img
	global back_button_img
	global next_button_img
	global calibrate_button_img
	global off_button_img
	global difficulty_button_img
	global done_button_img
	global nextcalibrate_button_img
	global returntostart_button_img
	calibratestep1_img			= ImageTk.PhotoImage(Image.open(calibratestep1_img_path))
	calibratestep2_img 			= ImageTk.PhotoImage(Image.open(calibratestep2_img_path))
	calibratestep3_img 			= ImageTk.PhotoImage(Image.open(calibratestep3_img_path))
	calibratestep4_img 			= ImageTk.PhotoImage(Image.open(calibratestep4_img_path))
	startupscreen_img 			= ImageTk.PhotoImage(Image.open(startupscreen_img_path))
	number0_img					= ImageTk.PhotoImage(Image.open(number0_img_path))
	number1_img 				= ImageTk.PhotoImage(Image.open(number1_img_path))
	number2_img 				= ImageTk.PhotoImage(Image.open(number2_img_path))
	number3_img 				= ImageTk.PhotoImage(Image.open(number3_img_path))
	winner_img					= ImageTk.PhotoImage(Image.open(winner_img_path))
	loser_img  					= ImageTk.PhotoImage(Image.open(loser_img_path))
	score_img 					= ImageTk.PhotoImage(Image.open(score_img_path))
	start_button_img			= ImageTk.PhotoImage(Image.open(start_button_img_path))
	easy_button_img				= ImageTk.PhotoImage(Image.open(easy_button_img_path))
	med_button_img				= ImageTk.PhotoImage(Image.open(med_button_img_path))
	hard_button_img				= ImageTk.PhotoImage(Image.open(hard_button_img_path))
	back_button_img				= ImageTk.PhotoImage(Image.open(back_button_img_path))
	next_button_img				= ImageTk.PhotoImage(Image.open(next_button_img_path))
	calibrate_button_img		= ImageTk.PhotoImage(Image.open(calibrate_button_img_path))
	off_button_img				= ImageTk.PhotoImage(Image.open(off_button_img_path))
	difficulty_button_img		= ImageTk.PhotoImage(Image.open(difficulty_button_img_path))
	done_button_img				= ImageTk.PhotoImage(Image.open(done_button_img_path))
	nextcalibrate_button_img	= ImageTk.PhotoImage(Image.open(nextcalibrate_button_img_path))
	returntostart_button_img	= ImageTk.PhotoImage(Image.open(returntostart_button_img_path))

	global calibratestep1_panel
	global calibratestep2_panel
	global calibratestep3_panel
	global calibratestep4_panel
	global startup_panel
	global aha_number_panel
	global winner_panel
	global loser_panel
	global score_panel
	global user_number_panel
	calibratestep1_panel	= Tkinter.Label(top, image = calibratestep1_img)
	calibratestep2_panel 	= Tkinter.Label(top, image = calibratestep2_img)
	calibratestep3_panel 	= Tkinter.Label(top, image = calibratestep3_img)
	calibratestep4_panel 	= Tkinter.Label(top, image = calibratestep4_img)
	startup_panel			= Tkinter.Label(top, image = startupscreen_img)
	aha_number_panel		= Tkinter.Label(top, image = number0_img)
	user_number_panel		= Tkinter.Label(top, image = number0_img)
	winner_panel			= Tkinter.Label(top, image = winner_img)
	loser_panel				= Tkinter.Label(top, image = loser_img)
	score_panel				= Tkinter.Label(top, image = score_img)

	global calibrate_button
	global off_button
	global difficulty_button
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	global backtodifficulty_button
	global start_button
	global startgame_button
	global next1_button
	global next2_button
	global next3_button
	global next4_button
	global finish_button
	calibrate_button		= Tkinter.Button(top, text = "Calibrate",         command = calibrate_start_screen)			# Button set up
	off_button				= Tkinter.Button(top, text = "Turn Off",          command = turn_off)
	difficulty_button   	= Tkinter.Button(top, text = "Choose Difficulty", command = difficulty_set_screen)
	easy_button				= Tkinter.Button(top, text = "Easy Mode",         command = easy_ready_screen)
	med_button				= Tkinter.Button(top, text = "Medium Mode",       command = med_ready_screen)
	hard_button				= Tkinter.Button(top, text = "Hard Mode",         command = hard_ready_screen)
	backtowelcome_button    = Tkinter.Button(top, text = "Back",              command = welcome_screen)
	backtodifficulty_button = Tkinter.Button(top, text = "Back",              command = difficulty_set_screen)
	start_button			= Tkinter.Button(top, text = "Start",             command = welcome_screen)
	startgame_button 		= Tkinter.Button(top, text = "Play",              command = score_screen)
	next1_button     		= Tkinter.Button(top, text = "Next",              command = calibrate_step1_screen)
	next2_button     		= Tkinter.Button(top, text = "Next",              command = calibrate_step2_screen)
	next3_button     		= Tkinter.Button(top, text = "Next",              command = calibrate_step3_screen)
	next4_button     		= Tkinter.Button(top, text = "Next",              command = calibrate_step4_screen)
	finish_button    		= Tkinter.Button(top, text = "Finish",            command = calibration_done)
	
	top.title("AHA! Welcome")
	top.geometry("480x272")
	top.configure(background='white')
	
	top.focus_set()
	
	start_button.place(x = 10, y = 0)					# Whole program begins here where the Start button appears alone
	start_button.config(image = start_button_img)

	global vector_listen
	global vector_listen_should_exit
	global conn
	conn = None
	vector_listen_should_exit = False
	vector_listen = threading.Thread(target = vector_listen_f)
	vector_listen.start()

	vector_send("UO")

	try:
		top.mainloop()
	except KeyboardInterrupt:
		vector_listen_should_exit = True
		vector_listen.join()
		sys.exit()

def vector_listen_f():
	global vector_listen_should_exit
	global conn

	tcp_port = 5005
	buffer_size = 1024

	socket.setdefaulttimeout(1)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind(('', tcp_port))
	sock.listen(1)

	while True:
		try:
			conn, addr = sock.accept()
		except socket.timeout:
			print "timeout accepting"
			if vector_listen_should_exit:
				l_onoff = 1
				l_linger = 0
				sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
								struct.pack('ii', l_onoff, l_linger))
				sock.close()
				return
			continue
		break
	
	while True:
		ready = select.select([conn], [], [], 1)[0]
		if ready:
			line = conn.recv(buffer_size)
			print line,
		if vector_listen_should_exit:
			l_onoff = 1
			l_linger = 0
			sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
							struct.pack('ii', l_onoff, l_linger))
			sock.close()
			break

def vector_send(string):
	global conn

	if not conn is None:
		conn.send(string + '\n')

def score_read():
	print "thread spawned"
	global userScore
	global AHAScore
	global turnoff

	userScore = 0
	AHAScore = 0

	while len(select.select([sys.stdin.fileno()], [], [], 0.0)[0])>0:
		os.read(sys.stdin.fileno(), 4096)

	while True:
		#print "meow"
		if len(select.select([sys.stdin.fileno()], [], [], 0.0)[0])>0:
			userinput = stdin.readline()
			print userinput

			if "u" in userinput:
				userScore = userScore + 1
			if "a" in userinput:
				AHAScore = AHAScore + 1

		time.sleep(0.05)
		print turnoff

		if(userScore == 3 or AHAScore == 3 or turnoff == True):
			print "exiting thread do score read"
			break

def welcome_screen():					# Window that appears after initialization
	global start_button
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	global calibratestep4_panel
	global finish_button
	global winner_panel
	global loser_panel
	start_button.place_forget()			# Wipe all buttons from previous window
	easy_button.place_forget()
	med_button.place_forget()
	hard_button.place_forget()
	backtowelcome_button.place_forget()
	calibratestep4_panel.pack_forget()
	finish_button.place_forget()
	winner_panel.pack_forget()
	loser_panel.pack_forget()

	global calibrate_button
	global off_button
	global difficulty_button
	global top
	calibrate_button.place(x = 0, y = 0)					# Set buttons for Calibrate, Turn off_button_img, and Choose difficulty_button_img options
	calibrate_button.config(image = calibrate_button_img)
	off_button.place(x = 240, y = 0)
	off_button.config(image = off_button_img)
	difficulty_button.place(x = 120, y = 136)
	difficulty_button.config(image = difficulty_button_img)
	top.title("Welcome")

def calibrate_start_screen():				# When Calibrate button is triggered, come here 
	global calibrate_button
	global off_button
	global difficulty_button
	calibrate_button.place_forget()			# Wipe all buttons from previous window
	off_button.place_forget()
	difficulty_button.place_forget()

	vector_send("UCS")

	global next1_button
	global top
	global T
	next1_button.place(x = 120, y = 100)
	next1_button.config(image = next_button_img)
	top.title("Calibration")

	T.pack()
	T.insert(END, "Perform the illustrated step\nand press Next\n")
	mainloop()


def calibrate_step1_screen():
	global next1_button
	global T
	next1_button.place_forget()
	T.delete(1.0, END)
	T.pack_forget()

	vector_send("UC1")

	global next2_button
	next2_button.place(x = 173, y = 200)
	next2_button.config(image = nextcalibrate_button_img)

	global top
	global calibratestep1_panel
	top.title("Step 1")
	calibratestep1_panel.pack()

def calibrate_step2_screen():
	global next2_button
	global calibratestep1_panel
	next2_button.place_forget()
	calibratestep1_panel.pack_forget()

	vector_send("UC2")

	global next3_button
	next3_button.place(x = 173, y = 200)
	next3_button.config(image = nextcalibrate_button_img)

	global top
	global calibratestep2_panel
	top.title("Step 2")
	calibratestep2_panel.pack()

def calibrate_step3_screen():
	global next3_button
	global calibratestep2_panel
	next3_button.place_forget()
	calibratestep2_panel.pack_forget()

	vector_send("UC3")

	global next4_button
	next4_button.place(x = 173, y = 200)
	next4_button.config(image = nextcalibrate_button_img)

	global top
	global calibratestep3_panel
	top.title("Step 3")
	calibratestep3_panel.pack()

def calibrate_step4_screen():
	global next4_button
	global calibratestep3_panel
	next4_button.place_forget()
	calibratestep3_panel.pack_forget()

	vector_send("UC4")

	global finish_button
	finish_button.place(x = 173, y = 200)
	finish_button.config(image = done_button_img)

	global top
	global calibratestep4_panel
	top.title("Step 4")
	calibratestep4_panel.pack()

def calibration_done():
	vector_send("UCD")

	welcome_screen()

def turn_off():					# When Turn Off button is chosen, come here
	global vector_listen_should_exit
	global vector_listen

	vector_send("UF")

	vector_listen_should_exit = True
	vector_listen.join()

	global ui_path
	os.system(ui_path + "/off.sh")

	global top
	top.destroy()

def difficulty_set_screen():		# When Choose Difficulty button is triggered, come here
	global calibrate_button
	global off_button
	global difficulty_button
	global backtodifficulty_button
	global startgame_button
	calibrate_button.place_forget()			# Wipe all buttons from previous window
	off_button.place_forget()
	difficulty_button.place_forget()
	backtodifficulty_button.place_forget()
	startgame_button.place_forget()

	global top
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	top.title("Choose a Level")
	easy_button.place(x = 0, y = 0)
	easy_button.config(image = easy_button_img)
	med_button.place(x = 240, y = 0)
	med_button.config(image = med_button_img)
	hard_button.place(x = 0, y = 136)
	hard_button.config(image = hard_button_img)
	backtowelcome_button.place(x = 240, y = 136)
	backtowelcome_button.config(image = back_button_img)				# Back option returns to Welcome Menu

def easy_ready_screen():					# When Easy button is triggered, come here
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	easy_button.place_forget()			# Wipe all buttons from previous window
	med_button.place_forget()
	hard_button.place_forget()
	backtowelcome_button.place_forget()

	vector_send("UD1")

	global top
	global startgame_button
	global backtodifficulty_button
	top.title("Ready to Play?")
	startgame_button.place(x = 0, y = 0)			# Set buttons for start game or go back
	startgame_button.config(image = difficulty_button_img)
	backtodifficulty_button.place(x = 240, y = 0)
	backtodifficulty_button.config(image = back_button_img)


def med_ready_screen():					# When Medium button is triggered, come here
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	easy_button.place_forget()			# Wipe all buttons from previous window
	med_button.place_forget()
	hard_button.place_forget()
	backtowelcome_button.place_forget()

	vector_send("UD2")

	global top
	global startgame_button
	global backtodifficulty_button
	top.title("Ready to Play?")
	startgame_button.place(x = 0, y = 0)			# Set buttons for start game or go back
	startgame_button.config(image = difficulty_button_img)
	backtodifficulty_button.place(x = 240, y = 0)
	backtodifficulty_button.config(image = back_button_img)

def hard_ready_screen():					# When Hard button is triggered, come here
	global easy_button
	global med_button
	global hard_button
	global backtowelcome_button
	easy_button.place_forget()			# Wipe all buttons from previous window 
	med_button.place_forget()
	hard_button.place_forget()
	backtowelcome_button.place_forget()

	vector_send("UD3")

	global top
	global startgame_button
	global backtodifficulty_button
	top.title("Ready to Play?")
	startgame_button.place(x = 0, y = 0)			# Set buttons for start game or go back
	startgame_button.config(image = difficulty_button_img)
	backtodifficulty_button.place(x = 240, y = 0)
	backtodifficulty_button.config(image = back_button_img)				

def score_screen():					# When Start Game button is triggered, come here
	global backtodifficulty_button
	global startgame_button
	global top
	backtodifficulty_button.place_forget()			# Wipe all buttons from previous window
	startgame_button.place_forget()
	top.title("Scores")

	vector_send("US")

	global score_panel
	score_panel.pack()

	global aha_number_panel
	global user_number_panel
	aha_number_panel.pack()
	aha_number_panel.place(x = 40, y = 125)
	user_number_panel.pack()
	user_number_panel.place(x = 250, y = 125)

	time.sleep(1)

	start_scorekeeping()

def start_scorekeeping(): #change

	global start_time
	global scorekeeping_done
	scorekeeping_done = False
	start_time = datetime.datetime.now()

	#start the scorekeeping thread
	global score_read_thread
	global turnoff
	turnoff = False
	score_read_thread = threading.Thread(target=score_read)
	score_read_thread.start()

	loopy = top.after(1000,update_scores)


def update_scores():
	global score_read_thread
	global scorekeeping_done
	global top

	print "aha score: ", AHAScore, "userScore", userScore
	loopy = top.after(2000,update_scores)

	global user_number_panel
	global aha_number_panel
	if AHAScore == 0:
		#display 0
		user_number_panel.configure(image = number0_img)

	if AHAScore == 1:
		#display 0
		user_number_panel.configure(image = number1_img)

	if AHAScore == 2:
		user_number_panel.configure(image = number2_img)
		#display 0

	if userScore == 0:
		#display 0
		aha_number_panel.configure(image = number0_img)

	if userScore == 1:
		#display 0
		aha_number_panel.configure(image = number1_img)

	if userScore == 2:
		aha_number_panel.configure(image = number2_img)
		#display 0

	if AHAScore >= 3:
		user_number_panel.configure(image = number3_img)
		#display 0
		if scorekeeping_done:
			top.after_cancel(loopy)
			score_read_thread.join()
			loser_screen()
		scorekeeping_done = True

	if userScore >= 3:
		aha_number_panel.configure(image = number3_img)
		#display 0
		if scorekeeping_done:
			top.after_cancel(loopy)
			score_read_thread.join()
			winner_screen()
		scorekeeping_done = True

	global start_time
	global turnoff
	now = datetime.datetime.now() - start_time
	if now.total_seconds() >= 60:
		print "timeout ui background"
		turnoff = True
		top.after_cancel(loopy)
		score_read_thread.join()
		print "exit ui background loop"
		loser_screen()


def winner_screen():
	global user_number_panel
	global aha_number_panel
	user_number_panel.place(x = 1000, y = 1000) #user score
	aha_number_panel.place(x = 1000, y = 1000)	#aha score

	vector_send("UP")
	
	global score_panel
	score_panel.pack_forget()

	global winner_panel
	winner_panel.pack()

	global start_button
	start_button.place(x = 200, y = 220)
	start_button.config(image = returntostart_button_img)

def loser_screen():
	global user_number_panel
	global aha_number_panel
	user_number_panel.place(x = 1000, y = 1000) #user score
	aha_number_panel.place(x = 1000, y = 1000)	#aha score

	vector_send("UP")
	
	global score_panel
	score_panel.pack_forget()

	global loser_panel
	loser_panel.pack()

	global start_button
	start_button.place(x = 200, y = 220)
	start_button.config(image = returntostart_button_img)

if __name__ == "__main__":
	main()
