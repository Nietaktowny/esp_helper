#! /usr/bin/python3

import subprocess
import os
import requests

os.chdir('..')

def describe_project():
    subprocess.run('git describe --tags > version.txt')

def translate_board(env):
    if(env == "esp32-c3"):
        return 'ESP32-C3-DevKitC-02'
    elif(env == 'esp-wrover-kit'):
        return 'ESP-WROVER-KIT'
    elif(env == 'esp32-dev'):
        return 'ESP32 Dev Module'
    

def get_project_version():
    version_file = open('version.txt', 'r+')
    firmware_ver = version_file.readline()
    print("Current project version is: " + firmware_ver)
    return firmware_ver


def publish_env(project_ver, board_ver, esp_idf_ver, firmware_ver):
    files = {
        'firmware_submit': (None, 'Upload'),
        'project_ver': (None, project_ver),
        'board_ver': (None, board_ver),
        'esp_idf_ver': (None, esp_idf_ver),
        'firmware_ver': (None, firmware_ver),
        'firmware_bin': open('firmware.bin', 'rb'),
    }

    response = requests.post('https://wmytych.usermd.net/modules/setters/upload_firmware.php', files=files)
    print(response.text)

for project in os.listdir("projects"):
    if(project == 'lib_tests' or project == 'plantsitter'):
        continue
    os.chdir(project)
    print("Publishing firmware of project: " + project)
    describe_project()
    firmware_ver = get_project_version()
    project_ver = project
    esp_idf_ver = '6.5.0'
    os.chdir('.pio')
    for board in os.listdir('build'):
        board_ver = translate_board(board)
        os.chdir(board)
        publish_env(project_ver, board_ver, esp_idf_ver, firmware_ver)
        os.chdir('..')
    os.chdir('..')
        