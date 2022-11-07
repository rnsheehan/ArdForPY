# The aim of this project is to develop code necessary to run the IBM4
# The code should enable all measurements
# All commands should be well documented
# All limits should be strictly enforced
# Ideally the IBM4 should never be allowed to be in an unknown state
#
# The code is based on an existing set of commands implemented by JC, RNS and FHP
# All commands must be compatible with the LabVIEW library IBM4
#
# Ultimately the code on the IBM4 will consist of two files
# Code.py - will run the serial loop function that listens for input commands
# Measurement.py - a module which will contain the function definitions that are called from inside Code.py
#
# R. Sheehan 7 - 11 - 2022
