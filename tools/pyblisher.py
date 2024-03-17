#! /usr/bin/python3

import subprocess
import os
import requests
import sys
import json

ci = False   # if set to true, don't update with git describe, only read currently saved values from version.txt texts.
    
def get_devices():
    response = requests.post('https://wmytych.usermd.net/modules/getters/get_devices.php')
    devices = json.loads(response.text)
    return devices

def get_image_data(image_id):
    data = {
        'image_id': (None, image_id),
    }
    r = requests.post('https://wmytych.usermd.net/modules/getters/get_image_data.php', data=data)
    r.raise_for_status()
    image_data = json.loads(r.text)
    return image_data
            
def describe_project():
    if(ci == True):
        return
    args = ['describe', '--tags', '--always']
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
    response.raise_for_status()


def update_device_with_alias(alias, project, firmware):
    devices = get_devices()
    for device in devices:
        if(device[3] == alias):
            if(project == 'none' and firmware == 'none'):
                image_data = get_image_data(device[2])
                project = image_data[1]
                os.chdir("projects/" + project)
                firmware = get_project_version()
                os.chdir('../..')
            elif(project != 'none' and firmware == 'none'):
                os.chdir("projects/" + project)
                firmware = get_project_version()
                os.chdir('../..')
            elif(project == 'none' and firmware != 'none'):
                image_data = get_image_data(device[2])
                project = image_data[1]
                
            print("\x1b[1;34m" + "Setting data of device '" + alias + "' with id '"+ device[0] + "' to:\nProject: " + project + "\nVersion: " + firmware + "\x1b[0m")
            data = {
                'switch_firmware_submit': (None, 'Upload'),
                'switch_device_id': (None, device[0]),
                'switch_project': (None, project),
                'switch_version': (None, firmware),
            }
            response = requests.post('https://wmytych.usermd.net/modules/setters/switch_device_firmware.php', data=data)
            response.raise_for_status()


def update_all_devices():
    print("\x1b[1;32m" + "Updating all devices with currently available projects versions..." + "\x1b[0m")
    devices = get_devices()
    for device in devices:
        update_device_with_alias(device[3], 'none', 'none')

def publish_all_projects():
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


if(len(sys.argv) != 1 and sys.argv[1] == "--ci"):
    ci = True
    os.chdir('..')
    publish_all_projects()
elif(len(sys.argv) != 1 and sys.argv[1] == "--local"):
    os.chdir('/home/wmytych/Projects/esp_helper/')
    publish_all_projects()
elif(len(sys.argv) == 3 and sys.argv[1] == "--update" and sys.argv[2] == "all"):
    os.chdir('..')
    update_all_devices()
elif(len(sys.argv) == 3 and sys.argv[1] == "--update" and sys.argv[2] != "all"):
    os.chdir('..')
    update_device_with_alias(sys.argv[2], 'none', 'none')
elif(len(sys.argv) == 5 and sys.argv[1] == "--update" and sys.argv[3] == '--project'):
    os.chdir('..')
    update_device_with_alias(sys.argv[2], sys.argv[4], 'none')
elif(len(sys.argv) == 5 and sys.argv[1] == "--update" and sys.argv[3] == '--version'):
    os.chdir('..')
    update_device_with_alias(sys.argv[2], 'none', sys.argv[4])
else:
    os.chdir('..')
    publish_all_projects()
        