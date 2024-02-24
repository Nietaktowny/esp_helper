import logging
import pytest

@pytest.mark.esp32
def test_ota_controller(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('TESTING OTA CONTROLLER')
    dut.expect('Starting OTA')
    dut.expect('image bytes read:')