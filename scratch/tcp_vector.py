#!/usr/bin/env python

import socket
import threading
import sys
import errno
import time
import select

def main():
	global s
	global listen_should_exit

	listen_should_exit = False
	listen_thread = threading.Thread(target=listen)
	listen_thread.setDaemon(True)

	s = connect()

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

def connect():
	tcp_ip = '192.168.2.30'
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

	buffer_size = 1024

	while True:
		ready = select.select([s], [], [], 1)[0]
		if (ready):
			try:
				line = s.recv(buffer_size)
				print line,
				if line == "UF\n":
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


if __name__ == "__main__":
	main()
