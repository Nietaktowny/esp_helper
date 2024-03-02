import logging
import pytest

@pytest.mark.esp32
def test_bmp_project(dut):
    dut.expect("logger initialized successfully")
    dut.expect('default nvs partition was init.')
    dut.expect("STA connect handler function of wifi controller changed!")
    dut.expect("Wifi initialized.")