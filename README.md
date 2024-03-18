![example workflow](https://github.com/Nietaktowny/esp_helper/actions/workflows/lib_tests_workflow.yml/badge.svg)
![example workflow](https://github.com/Nietaktowny/esp_helper/actions/workflows/build_projects.yml/badge.svg)

# ESP Helper

My own IOT framework build on top of the ESP-IDF framework.

It's mainly made of four parts:
 - IOT projects build on ESP32 chips, written in C using ESP-IDF - they are stored in [projects/](/projects/) directory.
 - Libraries used by projects as reusable pieces of software - stored in [libraries/](libraries/) directory.
 - CLI utility python program - used to mainly to make maintaining C projects easier - stored in [tools/](/tools/) directory.
 - Server written in PHP for end users to make interacting with IOT Devices easy - server address is: https://wmytych.usermd.net/.

