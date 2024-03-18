#! /usr/bin/python3
import os
import subprocess


print('\x1b[1;32m' + 'Cleaning all pytest projects...' + '\x1b[0m')
for file in os.listdir():
    if file.startswith("test_"):
        print('Cleaning test directory: ' + file)
        os.chdir(file)
        subprocess.run(["idf.py", "fullclean"])
        os.chdir("..")
        
        
print('\x1b[1;32m' + 'Cleaning all qemu Unity tests...' + '\x1b[0m')
start_path = os.getcwd()
for file in os.listdir():
    os.chdir(start_path)
    if file.startswith("test_"):
        os.chdir(file)
        if(os.path.isdir('test')):
            os.chdir("test")
            print('\x1b[0;34m' + 'Cleaning qemu tests on path: ' + file + "/test" + '\x1b[0m')
            subprocess.run(["idf.py", "fullclean"])