import logging
import pytest


def test_esp_helper_utils(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('TESTING ESP HELPER UTILS')
    dut.expect('Starting OTA')