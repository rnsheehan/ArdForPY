import supervisor
import time
import board
import pwmio
#import busio # I thought these might be needed 
#import digitalio # but they're not required
from analogio import AnalogIn
from analogio import AnalogOut

# pwm = pwmio.PWMOut(board.D9, frequency=500)
# pwm = pwmio.PWMOut(board.D9, duty_cycle=2 ** 15, frequency=440, variable_frequency=True)
pwm5 = pwmio.PWMOut(board.D5, duty_cycle=2 ** 15)
pwm7 = pwmio.PWMOut(board.D7, duty_cycle=2 ** 15)
pwm9 = pwmio.PWMOut(board.D9, duty_cycle=2 ** 15)
pwm10 = pwmio.PWMOut(board.D10, duty_cycle=2 ** 15)
pwm11 = pwmio.PWMOut(board.D11, duty_cycle=2 ** 15)
pwm12 = pwmio.PWMOut(board.D12, duty_cycle=2 ** 15)
pwm13 = pwmio.PWMOut(board.D13, duty_cycle=2 ** 15)

# For listening for serial inputs
####
# Define names for In and Out pins
Vout0 = AnalogOut(board.A0)
Vout1 = AnalogOut(board.A1)
Vin2 = AnalogIn(board.A2)
Vin3 = AnalogIn(board.A3)
Vin4 = AnalogIn(board.A4)
Vin5 = AnalogIn(board.A5)
Vin6 = AnalogIn(board.D2) # according to pinout doc pin labelled as 2 is another AI
NRdChLim = 5 # Upper limit for no of channels that can read, previously NRdChLim = 4

# Define the constants
# For notes on the following see 
# https://learn.adafruit.com/circuitpython-basics-analog-inputs-and-outputs/analog-to-digital-converter-inputs
# ADC values in circuit python are all put in the range of 16-bit unsigned values so 0 - 65535 (-1+2**16)
Vmax = Vin2.reference_voltage # max AO/AI value
bit_scale = (64*1024) # 64 bits

# Functions to convert from 12-bit to Volt.
def dac_value(volts):
    return int(volts / Vmax * bit_scale)
def get_voltage(pin):
    return (pin.value * Vmax) / bit_scale
def get_PWM(percentage):
    return (int(percentage/100.0*0xffff+0.5))
while True:
    if supervisor.runtime.serial_bytes_available:   # Listens for a serial command
        command = input()
        if command.startswith("*IDN"):
            print('ISBY-UCC-RevA.1')
        if command.startswith("PWM"):
            try:
                Tokens = command[3:].split(":")
                ThePin = int(Tokens[0])
                SetPWM = float(Tokens[1])
                if ThePin == 5:
                    pwm5.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 7:
                    pwm7.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 9:
                    pwm9.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 10:
                    pwm10.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 11:
                    pwm11.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 12:
                    pwm12.duty_cycle = get_PWM(SetPWM)                
                elif ThePin == 13:
                    pwm13.duty_cycle = get_PWM(SetPWM)                
                else:
                    print('Pin out of range: 5,7,9,10-13')
            except ValueError as ex:
                print('Vset must be an integer')
                print(ex)
            else:
                print("PWMset", ThePin, "=", str(SetPWM), end=' ')
                print()
        if command.startswith("Write"):
            try:
                Tokens = command[5:].split(":")
                Chan = int(Tokens[0])
                SetVoltage = float(Tokens[1])
                if SetVoltage >= 0 and SetVoltage < 3.31:
                    # Sets limits on the Output voltage to board specs
                    if Chan == 0:
                        Vout0.value = dac_value(SetVoltage)  # Set the voltage
                    elif Chan == 1:
                        Vout1.value = dac_value(SetVoltage)  # Set the voltage
                    else:
                        print('Channel out of range: 0 - 1')
                else:
                    print('Vset out of range: 0 - 3.3V')
            except ValueError as ex:
                print('Vset must be a float')
                print(ex)
            except:
                print('Unknown problem')
            else:
                print("Vset", Chan, "=", str(SetVoltage), end=' ')
                print()
        if command.startswith("Read"):
            try:
                Tokens = command[4:].split(":")
                Chan = int(Tokens[0])
                N = int(Tokens[1])
                if Chan == 0:
                    Pin = Vin2
                elif Chan == 1:
                    Pin = Vin3
                elif Chan == 2:
                    Pin = Vin4
                elif Chan == 3:
                    Pin = Vin5
                elif Chan == 4:
                    Pin = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                if N < 1:
                    print('Must read at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if Chan in range(0, NRdChLim) and N > 0:
                    Values = [0] * N
                    for i in range(N):
                        Values[i] = get_voltage(Pin)
                    print("Output", Chan, "=", str(Values[0]), end=' ')
                    for i in range(1, N):
                        print(",", str(Values[i]), end=' ')
                    print()
        if command.startswith("Average"):
            try:
                Tokens = command[7:].split(":")
                Chan = int(Tokens[0])
                N = int(Tokens[1])
                if Chan == 0:
                    Pin = Vin2
                elif Chan == 1:
                    Pin = Vin3
                elif Chan == 2:
                    Pin = Vin4
                elif Chan == 3:
                    Pin = Vin5
                elif Chan == 4:
                    Pin = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                if N < 1:
                    print('Must average at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if Chan in range(0, NRdChLim) and N > 0:
                    Value = 0.0
                    for i in range(N):
                        Value += get_voltage(Pin)
                    Value /= N
                    print("Average", Chan, "=", str(Value))
        if command.startswith("BRead"):
            try:
                Tokens = command[5:].split(":")
                Chan = int(Tokens[0])
                N = int(Tokens[1])
                if Chan == 0:
                    Pin = Vin2
                elif Chan == 1:
                    Pin = Vin3
                elif Chan == 2:
                    Pin = Vin4
                elif Chan == 3:
                    Pin = Vin5
                elif Chan == 4:
                    Pin = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                if N < 1:
                    print('Must read at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if Chan in range(0, NRdChLim) and N > 0:
                    Values = [0] * N
                    for i in range(N):
                        Values[i] = Pin.value
                    print("Output", Chan, "=", str(Values[0]), end=' ')
                    for i in range(1, N):
                        print(",", str(Values[i]), end=' ')
                    print()
        if command.startswith("Diff_Read"):
            try:
                Tokens = command[9:].split(":")
                ChanPlus = int(Tokens[0])
                if ChanPlus == 0:
                    Pplus = Vin2
                elif ChanPlus == 1:
                    Pplus = Vin3
                elif ChanPlus == 2:
                    Pplus = Vin4
                elif ChanPlus == 3:
                    Pplus = Vin5
                elif ChanPlus == 4:
                    Pplus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                ChanMinus = int(Tokens[1])
                if ChanMinus == 0:
                    Pminus = Vin2
                elif ChanMinus == 1:
                    Pminus = Vin3
                elif ChanMinus == 2:
                    Pminus = Vin4
                elif ChanMinus == 3:
                    Pminus = Vin5
                elif ChanMinus == 4:
                    Pminus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                N = int(Tokens[2])
                if N < 1:
                    print('Must read at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if ChanPlus in range(0, NRdChLim) and ChanMinus in range(0, NRdChLim) and N > 0:
                    Values = [0] * N
                    for i in range(N):
                        Values[i] = get_voltage(Pplus)-get_voltage(Pminus)
                    print("Output =", str(Values[0]), end=' ')
                    for i in range(1, N):
                        print(",", str(Values[i]), end=' ')
                    print()
        if command.startswith("Diff_Average"):
            try:
                Tokens = command[12:].split(":")
                ChanPlus = int(Tokens[0])
                if ChanPlus == 0:
                    Pplus = Vin2
                elif ChanPlus == 1:
                    Pplus = Vin3
                elif ChanPlus == 2:
                    Pplus = Vin4
                elif ChanPlus == 3:
                    Pplus = Vin5
                elif ChanPlus == 4:
                    Pplus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                ChanMinus = int(Tokens[1])
                if ChanMinus == 0:
                    Pminus = Vin2
                elif ChanMinus == 1:
                    Pminus = Vin3
                elif ChanMinus == 2:
                    Pminus = Vin4
                elif ChanMinus == 3:
                    Pminus = Vin5
                elif ChanMinus == 4:
                    Pminus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                N = int(Tokens[2])
                if N < 1:
                    print('Must read at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if ChanPlus in range(0, NRdChLim) and ChanMinus in range(0, NRdChLim) and N > 0:
                    Value = 0.0
                    for i in range(N):
                        Value += (get_voltage(Pplus) - get_voltage(Pminus))
                    Value /= N
                    print("Average =", str(Value))
        if command.startswith("Diff_BRead"):
            try:
                Tokens = command[10:].split(":")
                ChanPlus = int(Tokens[0])
                if ChanPlus == 0:
                    Pplus = Vin2
                elif ChanPlus == 1:
                    Pplus = Vin3
                elif ChanPlus == 2:
                    Pplus = Vin4
                elif ChanPlus == 3:
                    Pplus = Vin5
                elif ChanPlus == 4:
                    Pplus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                ChanMinus = int(Tokens[1])
                if ChanMinus == 0:
                    Pminus = Vin2
                elif ChanMinus == 1:
                    Pminus = Vin3
                elif ChanMinus == 2:
                    Pminus = Vin4
                elif ChanMinus == 3:
                    Pminus = Vin5
                elif ChanMinus == 4:
                    Pminus = Vin6
                else:
                    print('Channel out of range: 0 - 4')
                N = int(Tokens[2])
                if N < 1:
                    print('Must read at least one value')
            except ValueError as ex:
                print('Channel must be an integer')
                print(ex)
            except:
                print('Unknown problem')
            else:
                if ChanPlus in range(0, NRdChLim) and ChanMinus in range(0, NRdChLim) and N > 0:
                    Values = [0] * N
                    for i in range(N):
                        Values[i] = Pplus.value - Pminus.value
                    print("Output =", str(Values[0]), end=' ')
                    for i in range(1, N):
                        print(",", str(Values[i]), end=' ')
                    print()