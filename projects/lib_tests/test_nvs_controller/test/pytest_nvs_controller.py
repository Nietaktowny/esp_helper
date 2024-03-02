import logging
import pytest

@pytest.mark.qemu
@pytest.mark.esp32
def test_nvs_controller(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect("Tests 0 Failures 0 Ignored ", timeout=300)
    dut.expect("OK", timeout=300)