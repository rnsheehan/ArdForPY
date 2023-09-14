# SPDX-FileCopyrightText: 2017 Limor Fried for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""CircuitPython Essentials Storage logging boot.py file"""
import board
import digitalio 
import storage
from analogio import AnalogOut

Vout0 = AnalogOut(board.A0)
Vout1 = AnalogOut(board.A1)

Vout0.value = 0
Vout1.value = 0

# For Gemma M0, Trinket M0, Metro M0/M4 Express, ItsyBitsy M0/M4 Express
# switch = digitalio.DigitalInOut(board.D2)
switch = digitalio.DigitalInOut(board.RX)

# For Feather M0/M4 Express
# switch = digitalio.DigitalInOut(board.D5)

# For Circuit Playground Express, Circuit Playground Bluefruit
# switch = digitalio.DigitalInOut(board.D7)

switch.direction = digitalio.Direction.INPUT
switch.pull = digitalio.Pull.UP

# If the switch pin is not connected to ground CircuitPython can write to the drive
# storage.remount("/", not switch.value)

# If the swtich pin is not connected to ground, then the USB drive is hidden to the user (but not to CircuitPython) 
if switch.value:
    storage.disable_usb_drive()
else:
    storage.enable_usb_drive()