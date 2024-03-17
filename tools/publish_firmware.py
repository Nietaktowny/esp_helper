#! /usr/bin/python3

import subprocess
import os
import requests
import sys



if(len(sys.argv) == 1):
    os.chdir('..')
else:
    os.chdir(sys.argv[1])


def describe_project():
    args = ['describe', '--tags']
    my_cmd = ['git'] + args
    with open('version.txt', 'w+') as outfile:
        subprocess.run(my_cmd, stdout=outfile)

def translate_board(env):
    if(env == "esp32-c3"):
        return str('ESP32-C3-DevKitC-02')
    elif(env == 'esp-wrover-kit'):
        return str('ESP-WROVER-KIT')
    elif(env == 'esp-dev'):
        return str('ESP32 Dev Module')
    else:
        return str("not known")
    

def get_project_version():
    version_file = open('version.txt', 'r+')
    firmware_ver = version_file.readline()
    print("Current project version is: " + firmware_ver)
    return firmware_ver


def publish_env(project_ver, board_ver, esp_idf_ver, firmware_ver):
    data = {
        'firmware_submit': (None, 'Upload'),
        'project_ver': (None, project_ver),
        'board_ver': (None, board_ver),
        'esp_idf_ver': (None, esp_idf_ver),
        'firmware_ver': (None, firmware_ver),
        'firmware_bin': open('firmware.bin', 'rb'),
    }

    print("\n\x1b[1;34mPrepared data:")
    print(data)
    print("\x1b[0m")   #print newline
    response = requests.post('https://wmytych.usermd.net/modules/setters/upload_firmware.php', files=data)
    print("\x1b[1;32m" + "Server response: \n")
    print(response.text)
    print("\x1b[0m")

for project in os.listdir("projects"):
    if(project == 'lib_tests' or project == 'plantsitter'):
        continue
    os.chdir("projects/" + project)
    print("\x1b[1;32mPublishing firmware of project: " + project + '\x1b[0m')
    describe_project()
    firmware_ver = get_project_version()
    project_ver = project
    esp_idf_ver = '6.5.0'
    os.chdir('.pio')
    for board in os.listdir('build'):
        if(board == 'project.checksum'):
            continue
        board_ver = translate_board(board)
        os.chdir("build/" + board)
        publish_env(project_ver, board_ver, esp_idf_ver, firmware_ver)
        os.chdir('../..')
    os.chdir('../../..')
        