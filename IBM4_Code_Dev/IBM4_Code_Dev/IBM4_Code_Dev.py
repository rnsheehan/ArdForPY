# The aim of this project is to develop code necessary to run the IBM4
# The code should enable all measurements
# All commands should be well documented
# All limits should be strictly enforced
# Ideally the IBM4 should never be allowed to be in an unknown state
#
# The code is based on an existing set of commands implemented by JC, RNS and FHP starting on 23 - 10 - 2020
# All commands must be compatible with the LabVIEW library IBM4
#
# Ultimately the code on the IBM4 will consist of two files
# Code.py - will run the serial loop function that listens for input commands
# Measurement.py - a module which will contain the function definitions that are called from inside Code.py
#
# R. Sheehan 7 - 11 - 2022

# The IBM4 board is very sensistive to Python errors so when defining methods always use the pass keyword
# until you don't need it, having a method with undefined outputreally messes with the board
# R. Sheehan 30 - 10 - 2020
# def methodName():
#   pass

# Name the module to aid with exception handling
MOD_NAME_STR = "IBM4_Code_Dev"

# import the necessary modules
import board
import busio
import time
import digitalio
import pwmio # required for PWM output
from analogio import AnalogOut
from analogio import AnalogIn
import supervisor # for listening to serial ports

# Define the names of the pins being written to and listened to
# pin definitions
# https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4/pinouts
Vout0 = AnalogOut(board.A0) # use the analog outputs
Vout1 = AnalogOut(board.A1) # use the analog outputs
#Vin1 = AnalogIn(board.A1) # Remember it is possible for A0 and A1 to be defined as AI pins but in general we won't do this
Vin2 = AnalogIn(board.A2)
Vin3 = AnalogIn(board.A3)
Vin4 = AnalogIn(board.A4)
Vin5 = AnalogIn(board.A5)
Vin6 = AnalogIn(board.D2) # according to pinout doc pin labelled as 2 is another AI
PWMpin = pwmio.PWMOut(board.D5, duty_cycle=0, frequency=440, variable_frequency=True) # setup PWM output

# The relays that enable HI and LO impedance measurements require a separate relay to switch their states
# D9 is being reserved for this purpose
Relpin = pwmio.PWMOut(board.D9, duty_cycle=0, frequency=1000, variable_frequency=False) # setup PWM voltage for Relay pin

# Define the constants
# For notes on the following see 
# https://learn.adafruit.com/circuitpython-basics-analog-inputs-and-outputs/analog-to-digital-converter-inputs
# ADC values in circuit python are all put in the range of 16-bit unsigned values so 0 - 65535 (-1+2**16)
Vmax = Vin2.reference_voltage # max AO/AI value
bit_scale = (64*1024) # 64 bits

# Define the resistors in the BP-UP converter
R1 = 2.65 # kOhm
R2 = 1.47 # kOhm
R3 = 1.0 # kOhm
K_BPUP = (R1*R2) + (R2*R3) + (R3*R1) # BP-UP constant
M_BPUP = (R3*R2)/K_BPUP # slope in BP-UP conversion 
M_BPUP_inv = 1.0 / M_BPUP # inverse of slope in BP-UP conversion
Rp = (R1*R3)/(R1+R3) # Parallel resistance needed to identify reference level
Sf = (R2+Rp) / Rp # scale factor in voltage divider used to identify reference level

# Default Read Method is going to be high-impedance 
# this means relays are set to normally closed, which means that Vin = Vout
# to get low-impedance read method switch relays by turning on Relpin and set HI_IMPED = False
# R. Sheehan 2 - 6 - 2022
HI_IMPED = True # This means that DC readings are the default to get AC readings set HI_IMPED  = False

board_name = 'ISBY-UCC-RevA.2'

# Need the following functions to convert voltages to 12-bit readings
# which can be understood by the board

def dac_value(volts):
    # convert a voltage to 10-bit value

    FUNC_NAME = ".dac_value()" # use this in exception handling messages
    ERR_STATEMENT = "Error: " + MOD_NAME_STR + FUNC_NAME

    try:
        if Vmax > 0.0 and bit_scale > 0:
            return int((volts / Vmax)*bit_scale)
        else:
            ERR_STATEMENT = ERR_STATEMENT + "\nvolt, bit scale factors not defined"
            raise Exception
    except Exception as e:
        print(ERR_STATEMENT)
        print(e)

def get_voltage(pin):
    # convert pin reading to voltage value
    # will need to correct reading by subtracting GND reading
    
    FUNC_NAME = ".get_voltage()" # use this in exception handling messages
    ERR_STATEMENT = "Error: " + MOD_NAME_STR + FUNC_NAME

    try:
        if Vmax > 0.0 and bit_scale > 0:
            ret_val = ((pin.value*Vmax)/bit_scale)
            return ret_val
        else:
            ERR_STATEMENT = ERR_STATEMENT + "\nvolt, bit scale factors not defined"
            raise Exception
    except Exception as e:
        print(ERR_STATEMENT)
        print(e)

def Interface():

    # The interface between the IBM4 and the PC
    # This is the only method that will be explicitly called in Code.py
    # The PC will see this function and treat it like a Serial port that can be written to and read from
    # R. Sheehan 18 - 11 - 2021

    # Python, strictly speaking, does not have a switch-case structure. 
    # You can use dictionary to implement switch-case but I don't think this is satisfactory, if-elif-else will do the job
    #
    # In Python 3.10, introduced in 2021, the match-case structure was introduced, this would do the job but I'm not sure if CircuitPython is compatible 
    # with Python 3.10 and I don't want to spend a lot of time figuring out whether or not it is when a solution exists
    # 
    # For more on match-case and switch-case implementations in Python, see
    # https://stackoverflow.com/questions/60208/replacements-for-switch-statement-in-python
    # R. Sheehan 14 - 11 - 2022

    # I'm going to include some legacy commands in the Interface that will only be accessible when interacting with IBM4 
    # using the console, these are being included because RNS finds it convenient to input simple commands when debugging / testing a board
    # The primary goal is to make the IBM4 compatible with the IBM4 LabVIEW library, so the end-user will only ever use the LabVIEW library
    # R. Sheehan 14 - 11 - 2022

    FUNC_NAME = ".Interface()" # use this in exception handling messages
    ERR_STATEMENT = "Error: " + MOD_NAME_STR + FUNC_NAME

    try:
        HI_IMPED = True # DC read is the default
        while True:
            if supervisor.runtime.serial_bytes_available:   # Listens for a serial command
                command = input() # read the messages coming from the console
                # Define what the IBM4 should do for each command you require
                if command.startswith("*IDN"):
                    print(board_name)
                elif command.startswith("a"):
                    Simple_Vout_A0(command, ERR_STATEMENT)
                elif command.startswith("b"):
                    Simple_Vout_A1(command, ERR_STATEMENT)
                elif command.startswith("f"):
                    HI_IMPED = False
                    Relpin.duty_cycle = 65535 # 100% Fully On
                elif command.startswith("g"):
                    HI_IMPED = True
                    Relpin.duty_cycle = 0 # 100% Fully Off
                elif command.startswith("l"):
                    Reading(HI_IMPED)
                else:
                    ERR_STATEMENT = ERR_STATEMENT + '\nERROR: Unknown command entered\n'
                    raise Exception
            else:
                print('If you can read this something has gone very wrong. ')
    except Exception as e:
        print(ERR_STATEMENT)
        print(e)

# Methods associated with each command
def Simple_Vout_A0(command, error_msg):
    # If the user inputs command a<value> this will be interpreted as setting A0 to the volt level <value>
    try:
        SetVoltage = float(command[1:])
        if SetVoltage >= 0.0 and SetVoltage < Vmax: # Sets limits on the Output voltage to board specs
            Vout0.value = dac_value(SetVoltage) # Set the voltage
        else:
            Vout0.value = dac_value(0.0) # Set the voltage to zero in the event of SetVoltage range error
    except Exception as e:
        print(error_msg)
        print(e)

def Simple_Vout_A1(command, error_msg):
    # If the user inputs command b<value> this will be interpreted as setting A1 to the volt level <value>
    try:
        SetVoltage = float(command[1:])
        if SetVoltage >= 0.0 and SetVoltage < Vmax: # Sets limits on the Output voltage to board specs
            Vout1.value = dac_value(SetVoltage) # Set the voltage
        else:
            Vout1.value = dac_value(0.0) # Set the voltage to zero in the event of SetVoltage range error
    except Exception as e:
        print(error_msg)
        print(e)

def Relay_Power_Switch_On(error_msg):
    # Switch the relay power from Off = GND to On = +5V
    try:
        HI_IMPED = False
        Relpin.duty_cycle = 65535 # 100% Fully On
    except Exception as e:
        print(error_msg)
        print(e)

def Relay_Power_Switch_Off(error_msg):
    # Switch the relay power from On = +5V to Off = GND
    try:
        HI_IMPED = True
        Relpin.duty_cycle = 0 # 100% Fully Off
    except Exception as e:
        print(error_msg)
        print(e)

def Reading(IMPED = True):

    # read the voltage levels at each of the 4 AI
    # Assumes that DC-offset is connected to pin D2
    # reads voltages through buffer in range [0, 3.3V]
    # performs conversion to get real input to BP-UP circuits
    # R. Sheehan 9 - 5 - 2022
    
    FUNC_NAME = ".Reading()" # use this in exception handling messages
    ERR_STATEMENT = "Error: " + MOD_NAME_STR + FUNC_NAME
    
    try:
        if IMPED: 
            # High-impedance voltage reading Vout = Vin
            V2real = get_voltage(Vin2) # no BP-UP on A2
            V3real = get_voltage(Vin3) # no BP-UP on A3
            V4real = get_voltage(Vin4) # no BP-UP on A4
            V5real = get_voltage(Vin5) # no BP-UP on A5
            # format string to output to nearest 10 mV
            #output_str = '%(v2)0.2f, %(v3)0.2f, %(v4)0.2f, %(v5)0.2f'%{"v2":V2real, "v3":V3real, "v4":V4real, "v5":V5real}
            DC_offset = get_voltage(Vin6) # read the DC offset of the BP-UP circuit
            output_str = '%(v2)0.2f, %(v3)0.2f, %(v4)0.2f, %(v5)0.2f, %(v6)0.2f'%{"v2":V2real, "v3":V3real, "v4":V4real, "v5":V5real, "v6":DC_offset}
        else: 
            # Low-impedance voltage reading, i.e. reading through the BP-UP converter
            # Vout = M_BPUP_inv * ( Vin - DC_offset )
            # It is necessary to read the DC offset over time as Vref changes over time
            DC_offset = get_voltage(Vin6) # read the DC offset of the BP-UP circuit
            #Vref = Sf * DC_offset # compute the reference level
            V2real = M_BPUP_inv * ( get_voltage(Vin2) - DC_offset ) # BP-UP on A2
            V3real = M_BPUP_inv * ( get_voltage(Vin3) - DC_offset ) # BP-UP on A3
            V4real = M_BPUP_inv * ( get_voltage(Vin4) - DC_offset ) # BP-UP on A4
            V5real = M_BPUP_inv * ( get_voltage(Vin5) - DC_offset ) # BP-UP on A5
            # format string to output to nearest 10 mV
            #output_str = '%(v2)0.2f, %(v3)0.2f, %(v4)0.2f, %(v5)0.2f'%{"v2":V2real, "v3":V3real, "v4":V4real, "v5":V5real}
            output_str = '%(v2)0.2f, %(v3)0.2f, %(v4)0.2f, %(v5)0.2f, %(v6)0.2f'%{"v2":V2real, "v3":V3real, "v4":V4real, "v5":V5real, "v6":DC_offset}
        print(output_str) # Prints to serial to be read by LabVIEW
    except Exception as e:
        print(ERR_STATEMENT)
        print(e)
        