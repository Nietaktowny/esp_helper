import os
import subprocess

for file in os.listdir():
    if file.startswith("test_"):
        print('Building test directory: ' + file)
        os.chdir(file)
        subprocess.run(["idf.py", "build"])
        os.chdir("..")