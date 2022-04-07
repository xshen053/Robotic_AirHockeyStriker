# Untitled - By: MikeShen - 周五 2月 11 2022

import sensor, image, time
from pyb import UART
uart = UART(3,9600)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()
uart.init(9600, bits=8, parity=None, stop=1)#串口初始化
while(True):
    clock.tick()
    img = sensor.snapshot()
    print(clock.fps())
    data = bytearray([0xA5,20,30,0x5B])
    uart.write(data)
