import time
from subprocess import run
import sys
from threading import Thread

#  python3 measure_energy.py ./matmul size
command = sys.argv[1:]

values  = []
running = True

def measure_thread():
    tty = '/dev/ttyACM0'
    run(f'stty -F {tty} 9600', shell=True, check=True)
    with open(tty) as f:
        for line in f:
            if line != '\n':
                values.append( float(line.split()[0]) )
            if not running:
                break

thread = Thread(target=measure_thread)
thread.start()

start_time = time.time()
run(command)
end_time = time.time()

total_time = end_time - start_time

running = False
thread.join()

constant_voltage =  5.23 #This value was tested using Multimeter device.
avg = sum(values) / len(values)
energy = avg * total_time * constant_voltage
energy_base = energy * 0.60 # 0.6 amp is the current amount when there is not background/frontgound process
power= avg * constant_voltage

print('Power: ' + str(power))
print('Energy: ' + str(energy))
#print('Energy_baseline: ' + str(energy_base))
print('Time: ' + str(total_time))
print('samples',len(values))

print('Power ' + 'Energy '  + 'Time' )
print(str(power) + ' ' +str(energy)  + ' '  + str(total_time))

