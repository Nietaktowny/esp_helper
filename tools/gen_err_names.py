import os

#first get list of files containing errors
for lib in os.listdir("lib"):
    os.chdir("include/")
    for header in os.listdir():
        print(header)