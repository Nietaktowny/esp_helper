import os
import subprocess

print('\x1b[1;32m' + 'Building all pytest projects...' + '\x1b[0m')

for file in os.listdir():
    if file.startswith("test_"):
        print('\x1b[0;34m' + 'Building test project: ' + file + '\x1b[0m')
        os.chdir(file)
        subprocess.run(["idf.py", "build"])
        os.chdir("..")
        
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