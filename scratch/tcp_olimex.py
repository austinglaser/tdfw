#!/usr/bin/env python

import socket
import threading
import sys

def main():
	global conn
	global listen_should_exit

	tcp_port = 5005
	
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bind(('', tcp_port))
	s.listen(1)
	
	listen_should_exit = False
	listen_thread = threading.Thread(target=listen)

	conn, addr = s.accept()
	print 'Connection address:', addr
	listen_thread.start()

	while True:
		try:
			line = sys.stdin.readline()
			conn.send(line)
		except KeyboardInterrupt:
			print ""
			listen_should_exit = True
			listen_thread.join()
			conn.close()
			sys.exit()

def listen():
	global conn
	global listen_should_exit

	buffer_size = 1024

	print "listening"

	while True:
		line = conn.recv(buffer_size)
		if listen_should_exit or not line:
			break

		print line,

if __name__ == "__main__":
	main()
