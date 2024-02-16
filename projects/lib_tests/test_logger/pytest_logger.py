import logging


def test_logger(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect("Debug log")
    dut.expect("Info log")
    dut.expect("Warn log")
    dut.expect("Error log")
    dut.expect("Fatal log")