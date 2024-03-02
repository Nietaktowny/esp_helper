import os
import subprocess

for file in os.listdir():
    if file.startswith("test_"):
        print('Building test project directory: ' + file)
        os.chdir(file)
        subprocess.run(["idf.py", "build"])
        if(os.path.isdir("test")):
            os.chdir("test")
            print(" Building test directory of project: " + os.getcwd())
            os.chdir("..")
        os.chdir("..")