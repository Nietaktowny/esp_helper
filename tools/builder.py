#! /usr/bin/python3
import os
import subprocess
import sys

def idf_build_all_projects():
    print('\x1b[1;32m' + 'Building all pytest projects...' + '\x1b[0m')
    for file in os.listdir():
        if file.startswith("test_"):
            print('\x1b[0;34m' + 'Building test project: ' + file + '\x1b[0m')
            os.chdir(file)
            subprocess.run(["idf.py", "build"])
            os.chdir("..")


def idf_build_unity_tests():
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

def idf_build_one_project(project):
    print('\x1b[1;32m' + 'Building pytest project: ' + project + '\x1b[0m')
    if(os.path.isdir(project)):
        os.chdir(project)
        subprocess.run(["idf.py", "build"]) 
    else:
         print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + project + "\x1b[0m")
  

def idf_build_one_unity_test(test):
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


def pio_build_one_project(project):
    print('\x1b[1;32m' + 'Building platformio project: ' + project + '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    if(os.path.isdir(project)):
        os.chdir(project)
        subprocess.run(["pio", "run"]) 
    else:
         print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + project + "\x1b[0m")


def pio_build_all_projects():
    print('\x1b[1;32m' + 'Building all platformio projects.' + '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    for project in os.listdir():
        os.chdir(project)
        subprocess.run(["pio", "run"])
        subprocess.run(["pio", "run", "-t", "compiledb"])
        os.chdir('..')

def pio_build_env_in_all_projects(env):
    print('\x1b[1;32m' + 'Building environment ' + env + ' in all platformio projects.' + '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    for project in os.listdir():
        os.chdir(project)
        subprocess.run("pio", "run", "-e", env)
        pio_args = ['run', '-t', 'compiledb', '-e', env]
        pio_cmd = ['pio'] + pio_args
        subprocess.run(pio_cmd)
        os.chdir('..')
    else:
         print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + project + "\x1b[0m")

def pio_build_env_in_one_project(project, env):
    print('\x1b[1;32m' + 'Building environment ' + env + ' in all platformio projects.' + '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    if(os.path.isdir(project)):
        os.chdir(project)
        subprocess.run('pio', 'run', '-e', env)
        pio_args = ['run', '-t', 'compiledb', '-e', env]
        pio_cmd = ['pio'] + pio_args
        subprocess.run(pio_cmd) 
    else:
         print("\x1b[1;31m" + "Cannot find project on given path: " + os.getcwd() + "/" + project + "\x1b[0m")

def pio_build_all_projects_ignore(ignore_list):
    print('\x1b[1;32m' + 'Building all platformio projects except: ' + ignore_list + '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    ignore_list = ignore_list.split(',')
    for project in os.listdir():
        if project in ignore_list:
            continue
        else:
            os.chdir(project)
            subprocess.run(["pio", "run"])
            subprocess.run(["pio", "run", "-t", "compiledb"]) 
            os.chdir('..')

def lib_update_libs_all_projects():
    print('\x1b[1;32m' + 'Updating libraries in all projects'+ '\x1b[0m')
    os.chdir('/home/wmytych/Projects/esp_helper/projects/')
    src_path = "/home/wmytych/Projects/esp_helper/projects/lib_tests/lib"
    for project in os.listdir():
        if(project == "lib_tests"):
            continue
        print('\x1b[0;34m' + 'Updating libraries in project: ' + project + '\x1b[0m')
        os.chdir(project)
        print("Purging old libraries versions in project: " + project)
        subprocess.run(["gio", "trash", "lib"])
        print("Updating new libraries versions...")
        subprocess.run(["cp", "-R", "-f", src_path, os.getcwd()])
        os.chdir('..')

def builder_print_help():
    help_text = """
    This python program is used to simplify building many ESP Helper projects at once.
    Options:
    --help               -       print help instructions
    --test               -       build all pytest test projects
            -u           -       build pytest unity test projects
            -p           -       build all pytest hardware test projects
            -u 'name'    -       build pytest unity test project with specified name
            -p 'name'    -       build pytest hardware test project with specified name
    --pio                        build all platformio projects
            -e           -       specify platformio enviornment to build
            -p           -       specify platformio project to build
            -i           -       ignore specified projects (separated by commas: np. project1,project2,project3)
    --update             -       update libraries from lib_tests in all other projects

    """
    print(help_text)


if(len(sys.argv) == 2 and sys.argv[1] == '--test'):
    print("Building all test projects")
    os.chdir('/home/wmytych/Projects/esp_helper/projects/lib_tests/')
    subprocess.run('. $HOME/Programs/esp/esp-idf/export.sh', shell=True)
    idf_build_all_projects()
    idf_build_unity_tests()
elif(len(sys.argv) == 2 and sys.argv[1] == '--help'):
    builder_print_help()
elif(len(sys.argv) == 2 and sys.argv[1] == '--update'):
    lib_update_libs_all_projects()
elif(len(sys.argv) == 3 and sys.argv[1] == '--test' and sys.argv[2] == "-u"):
    os.chdir('/home/wmytych/Projects/esp_helper/projects/lib_tests/')
    subprocess.run('. $HOME/Programs/esp/esp-idf/export.sh', shell=True)
    idf_build_unity_tests()
elif(len(sys.argv) == 3 and sys.argv[1] == '--test' and sys.argv[2] == "-p"):
    os.chdir('/home/wmytych/Projects/esp_helper/projects/lib_tests/')
    subprocess.run('. $HOME/Programs/esp/esp-idf/export.sh', shell=True)
    idf_build_all_projects()
elif(len(sys.argv) == 4 and sys.argv[1] == '--test' and sys.argv[2] == "-u"):
    os.chdir('/home/wmytych/Projects/esp_helper/projects/lib_tests/')
    subprocess.run('. $HOME/Programs/esp/esp-idf/export.sh', shell=True)
    idf_build_one_unity_test(sys.argv[3])
elif(len(sys.argv) == 4 and sys.argv[1] == '--test' and sys.argv[2] == "-p"):
    os.chdir('/home/wmytych/Projects/esp_helper/projects/lib_tests/')
    subprocess.run('. $HOME/Programs/esp/esp-idf/export.sh', shell=True)
    idf_build_one_project(sys.argv[3])
elif(len(sys.argv) == 2 and sys.argv[1] == '--pio'):
    pio_build_all_projects()
elif(len(sys.argv) == 4 and sys.argv[1] == '--pio' and sys.argv[2] == "-e"):
    pio_build_env_in_all_projects(sys.argv[3])
elif(len(sys.argv) == 4 and sys.argv[1] == '--pio' and sys.argv[2] == "-p"):
    pio_build_one_project(sys.argv[3])
elif(len(sys.argv) == 6 and sys.argv[1] == '--pio' and sys.argv[2] == "-p" and sys.argv[4] == "-e"):
    pio_build_env_in_one_project(sys.argv[3], sys.argv[5])
elif(len(sys.argv) == 4 and sys.argv[1] == '--pio' and sys.argv[2] == "-i"):
    pio_build_all_projects_ignore(sys.argv[3])
