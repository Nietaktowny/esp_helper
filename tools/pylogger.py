import os
import subprocess
import sys
import socket

levels = []

def connect(host):
     try:
          print("Connecting with host: " + host)
          s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
          s.connect((host, 27015))
          print("Connected to {:s}".format(repr((host, 27015))))
          while True:
               message = s.recv(1024)
               for level in levels:
                    logs = message.decode()
                    logs = logs.split('\x1b[0m')
                    for log in logs:
                         if level in log:
                              print(log)
     except KeyboardInterrupt:
          sys.exit()
          

def setup_log_level():
     for args in sys.argv:
          args = args.split('-')
          for arg in args:
               if(arg == 'v'):
                    levels.append('| VERBOSE |')
               elif(arg == 'd'):
                    levels.append('| DEBUG |')
               elif(arg == 'i'):
                    levels.append('| INFO  |')
               elif(arg == 'w'):
                    levels.append('| WARN  |')
               elif(arg == 'e'):
                    levels.append('| ERROR |')
               elif(arg == 'f'):
                    levels.append('| FATAL |')        
          
if(len(sys.argv) == 2):
     levels = ['| VERBOSE |', '| DEBUG |' ,'| INFO  |',  '| WARN  |', '| ERROR |', '| FATAL |']
     connect(sys.argv[1])
elif(len(sys.argv) >= 3):
     setup_log_level()
     connect(sys.argv[1])      
else:
     print("Provide arguments to pylogger")
