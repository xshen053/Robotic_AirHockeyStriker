# UART Test - By: shawn - Mon Feb 7 2022

import pyb, sensor, image, time, math, ustruct

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()


uart = pyb.UART(3, 9600, timeout_char=1000)                         # init with given baudrate
uart.init(9600, bits=8, parity=None, stop=1, timeout_char=1000) # init with given parameters
Tx_data = bytearray([0x2C, 7, 1, 2, 3, 4, 0x5B])
while(True):
    clock.tick()
    #uart.writechar(5)
    uart.write(Tx_data)
    time.sleep(1)
    #print(clock.fps())
