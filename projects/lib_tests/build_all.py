import os
import subprocess
from colorama import init
from termcolor import colored

for file in os.listdir():
    if file.startswith("test_"):
        print(colored('Building test directory: ' + file, 'green', 'on_black'))
        os.chdir(file)
        subprocess.run(["idf.py", "build"])
        os.chdir("..")