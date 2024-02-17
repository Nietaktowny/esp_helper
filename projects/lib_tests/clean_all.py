import os
import subprocess
from colorama import init
from termcolor import colored

for file in os.listdir():
    if file.startswith("test_"):
        print(colored('Cleaning test directory: ' + file, 'green', 'on_black'))
        os.chdir(file)
        subprocess.run(["idf.py", "fullclean"])
        os.chdir("..")