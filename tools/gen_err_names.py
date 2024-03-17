#! /usr/bin/python3


import os

os.chdir('~/Projects/esp_helper/projects/lib_tests/')

#first get list of files containing errors
for lib in os.listdir("lib"):
    os.chdir("include/")
    for header in os.listdir():
        print(header)