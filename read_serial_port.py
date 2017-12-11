import serial
import matplotlib.pyplot as plt
from drawnow import *
import time as t

try:
    arduino = serial.Serial('COM3', '9600', timeout = 10)
except:
    print('check port')

rawdata = []
count = 0
datapoints = 3600
interval = 1
xaxis = datapoints
time = []
temp = []
relay = []
error = []
PID = []
P = []
I = []
D = []
setpoint = []

def makeFig():
    plt.title("Live Temp + Relay")
    plt.grid(True)
    plt.xlim(0, datapoints)
    plt.xlabel('time [s]')
    plt.ylim(20, 40)
    plt.ylabel("oC")
    plt.plot(temp, 'r-', label='deg C')
    plt.plot(setpoint, 'k-', label='setpoint')
    plt.legend(loc='upper left')
    plt2 = plt.twinx()
    plt.ylim(-2000,11000)
    plt2.plot(relay, 'b-', label='relay')
    plt2.plot(P, 'r:', label=('P' + ' ' + str(y3)))
    plt2.plot(I, 'g:', label=('I' + ' ' + str(y4)))
    plt2.plot(D, 'b:', label=('D' + ' ' + str(y5)))
    plt2.plot(PID, 'y-', label=('PID'+ ' ' + str(y6)))
    plt2.set_ylabel('state')
    plt2.legend(loc='upper right')
    

t.sleep(5)

for i in range(datapoints):
    data = str(arduino.readline())[2:].split('\\r')[0]
    print(data)
    x = float(data.split(";")[0])  # time
    y1 = float(data.split(";")[1])  # temp
    y2 = float(data.split(";")[2])  # relay state
    y3 = float(data.split(";")[3])  # P
    y4 = float(data.split(";")[4])  # I
    y5 = float(data.split(";")[5])  # D
    y6 = float(data.split(";")[6])  # PID
    y7 = float(data.split(";")[7])  # setpoint
    if y2 == 0:
        y2 = 1000
    elif y2 == 1:
        y2 = 0  
    time.append(x)
    temp.append(y1)
    relay.append(y2)
    P.append(y3)
    I.append(y4)
    D.append(y5)
    PID.append(y6)
    setpoint.append(y7)
    drawnow(makeFig)
    plt.pause(0.05)
    t.sleep(1)

arduino.close()
