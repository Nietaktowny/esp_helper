import logging


def test_sys_utils(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('TESTING SYSTEM UTILS')
    dut.expect('MAC address')