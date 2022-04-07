# SPI_test - By: shawn - Mon Jan 24 2022

import sensor, image, time, pyb
#Wiring--------------------------
#openMV             |STM
#P0 = MOSI          |PA7  = SPI1_MOSI
#P1 = MISO          |PA6  = SPI1_MISO
#P2 = SCLK          |PA5  = SPI1_SCK
#P3 = SS            |PA4  = SPI1_SS **could also be PA15**
#--------------------------------
#set up SPI parameters
spi = SPI(2, SPI.MASTER, baudrate=115200, polarity=0, phase=1)
#slave select as Pin 3
ss = Pin("P3", Pin.OUT_OD)
clock = time.clock()

while(True):
    clock.tick()#for FPS tracking
    print("Starting Transmission")
    ss.low()#trigger transmission
    print("Sending Data")
    spi.send(b'5') #send data (hardcoded to 4 for testing)**try with three digit numbers**
    print("Ending Transmission")
    ss.high()#stop transmission
    print(clock.fps())#print fps
    delay(1000)#delay for 1s
