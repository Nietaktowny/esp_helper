import logging
import pytest

@pytest.mark.esp32
def test_wifi_controller(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('wifi successfully initialized')
    dut.expect('WiFi successfully configured as STA')
    dut.expect('WIFI_C_CONNECTED_BIT is set!')
    dut.expect('SSID: TEST_AP')
    dut.expect('Password: TEST_AP_PASSWORD')