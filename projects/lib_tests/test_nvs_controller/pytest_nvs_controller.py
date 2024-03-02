import logging
import pytest

@pytest.mark.qemu
@pytest.mark.esp32
def test_nvs_controller(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('TESTING NVS CONTROLLER')
    dut.expect('default nvs partition was init.')
    dut.expect("opened namespace 'nvs' for read and write operations")
    dut.expect("written 12 bytes data into namespace 'nvs'")
    dut.expect("read 13 bytes from namespace: 'nvs'")
    dut.expect("nvs namespace: 'nvs' was closed")