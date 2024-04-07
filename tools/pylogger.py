#! /usr/bin/python3

import os
import subprocess
import sys
import socket

levels = []
buffer = []





def connect_debug(host):
     try:
          print("Connecting with host: " + host)
          s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
          s.connect((host, 27015))
          print("Connected to {:s}".format(repr((host, 27015))))
          while True:
               message = s.recv(128)
               print(message)
     except KeyboardInterrupt:
          sys.exit()


def connect_and_filter(host):
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
                             if log.endswith('\n') or log.startswith('\n'):
                                 print(log, end='')
                             else:
                                 print(log)
     except KeyboardInterrupt:
          sys.exit()


def connect(host):
     try:
          print("Connecting with host: " + host)
          s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
          s.connect((host, 27015))
          print("Connected to {:s}".format(repr((host, 27015))))
          while True:
               message = s.recv(256)
               logs = message.decode('utf-8', 'strict')
               print(logs, end='')
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


def pylogger_print_help():
    help_text = """
    This python program is used to capture logs from ESP Helper devices on local network.
    You need first to know your ESP Helper device local IPv4 address, it can be found on
    https://wmytych.usermd.net/public/configuration.php page after selecting your device in "Device information"

    Usage:
    pylogger.py ipv4 -options

    Options:
    -x                  -           Print all captured data without decoding it, used mostly for debugging.
    -v                  -           Log verbose logs
    -d                  -           Log debug logs
    -i                  -           Log info logs
    -w                  -           Log warn logs
    -e                  -           Log error logs
    -f                  -           Log fatal logs

    If no log levels will be specified, no filtering on log level will be done.
    """
    print(help_text)


if(len(sys.argv) == 2 and sys.argv[1] != '-h'):
     connect(sys.argv[1])
elif(len(sys.argv) == 2 and sys.argv[1] == '-h'):
    pylogger_print_help()
elif(len(sys.argv) == 3 and sys.argv[2] == '-x'):
    connect_debug(sys.argv[1])
elif(len(sys.argv) >= 3 and sys.argv[2] != '-x'):
     setup_log_level()
     connect_and_filter(sys.argv[1])      
else:
     print("Provide arguments to pylogger")
