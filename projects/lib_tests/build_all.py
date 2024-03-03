import os
import subprocess
import sys

def build_all_projects():
    print('\x1b[1;32m' + 'Building all pytest projects...' + '\x1b[0m')
    for file in os.listdir():
        if file.startswith("test_"):
            print('\x1b[0;34m' + 'Building test project: ' + file + '\x1b[0m')
            os.chdir(file)
            subprocess.run(["idf.py", "build"])
            os.chdir("..")


def build_unity_tests():
    print('\x1b[1;32m' + 'Building all qemu Unity tests...' + '\x1b[0m')
    start_path = os.getcwd()
    for file in os.listdir():
        os.chdir(start_path)
        if file.startswith("test_"):
            os.chdir(file)
            if(os.path.isdir('test')):
                os.chdir("test")
                print('\x1b[0;34m' + 'Building qemu tests on path: ' + file + "/test" + '\x1b[0m')
                subprocess.run(["idf.py", "build"])

def build_one_project(project):
    print('\x1b[1;32m' + 'Building pytest project: ' + project + '\x1b[0m')
    if(os.path.isdir(project)):
        os.chdir(project)
        subprocess.run(["idf.py", "build"]) 
    else:
         print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + project + "\x1b[0m")
       

def build_one_unity_test(test):
    print('\x1b[1;32m' + 'Building qemu Unity test: ' + test + '\x1b[0m')
    if(os.path.isdir(test)):
        os.chdir(test)
        if(os.path.isdir('test')):
            os.chdir("test")
            print('\x1b[0;34m' + 'Building qemu tests on path: ' + test + "/test" + '\x1b[0m')
            subprocess.run(["idf.py", "build"])
        else:
            print("\x1b[1;31m" + "Specified project doesn't contain test folder" + "\x1b[0m")
    else:
        print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + test + "\x1b[0m")

if(len(sys.argv) == 1):
    build_all_projects()
    build_unity_tests()
if(len(sys.argv) == 2 and sys.argv[1] == "-u"):
    build_unity_tests()
elif(len(sys.argv) == 2 and sys.argv[1] == "-p"):
    build_all_projects()
elif(len(sys.argv) == 3 and sys.argv[1] == "-u"):
    build_one_unity_test(sys.argv[2])
elif(len(sys.argv) == 3 and sys.argv[1] == "-p"):
    build_one_project(sys.argv[2])
