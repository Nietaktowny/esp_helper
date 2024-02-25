import logging
import pytest

@pytest.mark.esp32
def test_wifi_manager(dut):
    dut.expect("logger initialized successfully")
    dut.expect("nvs was initialized")
    dut.expect("STA connect handler function of wifi controller changed!")
    dut.expect("Wifi initialized.")
    dut.expect("Connect to device on IP")