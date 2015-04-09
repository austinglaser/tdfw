#!/usr/bin/env python

import socket
import threading
import sys
import errno
import time

def main():
	global s
	global listen_should_exit

	tcp_ip = '192.168.2.1'
	tcp_port = 5005
	buffer_size = 1024
	
	message = "poop on ur face"
	
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

	listen_should_exit = False
	listen_thread = threading.Thread(target=listen)

	connected = False
	while not connected:
		try:
			s.connect((tcp_ip, tcp_port))
			connected = True
		except socket.error as serr:
			if serr.errno == errno.ECONNREFUSED:
				print "connection refused"
				time.sleep(1)
			else:
				raise serr
		

	listen_thread.start()

	while True:
		try:
			line = sys.stdin.readline()
			s.send(line)
		except KeyboardInterrupt:
			print ""
			listen_should_exit = True
			listen_thread.join()
			s.close()
			sys.exit()
	
def listen():
	global s
	global listen_should_exit

	buffer_size = 1024

	while True:
		line = s.recv(buffer_size)
		if listen_should_exit or not line:
			break;

		print line,

if __name__ == "__main__":
	main()
