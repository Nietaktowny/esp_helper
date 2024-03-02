import logging
import pytest

@pytest.mark.esp32
def test_wifi_manager(dut):
    dut.expect('cpu_start: Project name:     test_wifi_manager')
    dut.expect("wifi manager didn't find stored credentials or couldn't connect, starting AP and HTTP server...")
    dut.expect("wifi_controller deinitialized")
    dut.expect('Started AP')