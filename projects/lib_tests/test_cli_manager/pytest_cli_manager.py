import logging


def test_cli_manager(dut):
    # expect from what esptool.py printed to sys.stdout
    dut.expect('listening on socket:')
    dut.expect('27015')