import logging
import pytest
from pytest_embedded import Dut

@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.qemu
def test_logger(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect("Verbose log")
    dut.expect("Debug log")
    dut.expect("Info log")
    dut.expect("Warn log")
    dut.expect("Error log")
    dut.expect("Fatal log")