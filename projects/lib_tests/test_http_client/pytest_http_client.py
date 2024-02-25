import logging
import pytest

@pytest.mark.esp32
@pytest.mark.qemu
def test_http_client(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('WIFI_C_CONNECTED_BIT is set!')
    dut.expect('HTTP GET Status = 200')
    dut.expect('[{"gpio":22,"state":1,"gpio_alias":"Relay"},{"gpio":2,"state":1,"gpio_alias":"Onboard LED"}]')