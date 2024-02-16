import logging


def test_wifi_controller(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('wifi successfully initialized')
    dut.expect('WiFi successfully configured as STA')
    dut.expect('Started AP')