import logging

def test_wifi_manager(dut):
    dut.expect('cpu_start: Project name:     test_wifi_manager')
    dut.expect('Cannot find any stored AP in NVS')
    dut.expect("wifi_controller deinitialized")
    dut.expect('Started AP')