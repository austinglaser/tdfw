from sys import stdin
import time
import threading

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

	thread = threading.Thread(target=BG_Thread)
	thread.start()

	while True:
		try:
			print "User Score: ", userScore
			print "AHA Score: ", AHAScore

		except KeyboardInterrupt:
			ExitFlag = True
			thread.join()
			break

main()