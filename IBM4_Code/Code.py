# This script is for testing the operation of the current source controlled by the Itsy-Bitsy M4
# R. Sheehan 23 - 10 - 2020

# The IBM4 board is very sensistive to Python errors so when defining methods always use the pass keyword
# until you don't need it, having a method with undefined outputreally messes with the board
# R. Sheehan 30 - 10 - 2020
# def methodName():
#   pass

# import the necessary modules
import board
import busio
import time
import digitalio
from analogio import AnalogOut
from analogio import AnalogIn
import supervisor # for listening to serial ports

# Methods for making the measurements are defined in the file Measurement.py
import Measurement

#Measurement.First_Script()

Measurement.Cuffe_Iface() # This is for the Current Source Measurement

#Measurement.AC_Read() # This is trying to read a sine wave

#Measurement.AC_Max() # Find the max value being input to some channel

#Measurement.Voltage_Divider_Test()

#Measurement.Current_Source_Measurement()

#Measurement.IO_Simple()

#Measurement.hardware_SPI_test(board.SCK, board.MOSI) # Yes

#Measurement.hardware_SPI_test(board.SCK, board.MISO) # Yes

#Measurement.hardware_SPI_test(board.A2, board.A3) # No

#Measurement.T_Meas_Bridge()

#Measurement.T_Meas_Divider()
