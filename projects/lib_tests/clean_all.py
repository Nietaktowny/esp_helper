import os
import subprocess

for file in os.listdir():
    if file.startswith("test_"):
        print('Cleaning test directory: ' + file)
        os.chdir(file)
        subprocess.run(["idf.py", "fullclean"])
        os.chdir("..")