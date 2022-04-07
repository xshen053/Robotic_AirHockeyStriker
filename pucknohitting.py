# Untitled - By: MikeShen - 周五 2月 11 2022
#import libraries
import pyb, sensor, image, time, math
from pyb import UART
uart = UART(3,9600)
#global variables
x_1 = 0 #x coordinate location 1
y_1 = 0 #y coordinate location 1
x_2 = 0 #x coordinate location 2
y_2 = 0 #y coordinate location 2
x_3 = 0 #x coordinate location 3
y_3 = 0 #y coordinate location 3
flag = 0
ym = 0
xs = 15
#set up the camera
sensor.reset()#initializes camera sensor
sensor.set_pixformat(sensor.GRAYSCALE)#pixel format is grayscale 8bits/pixel
sensor.set_framesize(sensor.QQVGA)#sets framesize to 160x120
sensor.skip_frames(time = 2000)#let camera stabilize after changing settings
#start clock for FPS
c_fps = time.clock()#Create a clock object to track the FPS.
uart.init(9600, bits=8, parity=None, stop=1)#串口初始化
#FUNCTIONS-----------------------------------------------------------------------------------
#This function returns the distance between two points. inputs: ints output: double
def distance(x0, y0, x1, y1):
    d = ((x0 - x1)**2 + (y0 - y1)**2)**0.5
    return d
#MAIN CODE-----------------------------------------------------------------------------------
while(True):
    #Update the FPS clock.
    c_fps.tick()
    #take snapshot with lens corrected
    img1 = sensor.snapshot().lens_corr(1.3)
    #for loop to find the puck in the image **alter r_min and r_max based on puck**
    #while(x_1 == x_1_prev and y_1 == y_1_prev):
        #flag1 = flag1 + 1
    for a in img1.find_circles(threshold = 3600, x_margin = 10, y_margin = 10, r_margin = 10, r_min = 5, r_max = 12, r_step = 2):
            img1.draw_circle(a.x(), a.y(), a.r(), color = (255, 0, 0)) #draws a circle on the image
        #get coordinates from the first position of the puck
            x_1 = a.x()
            y_1 = a.y()
            #t = a.threshold()
            print("coordinate1 m:")
            print(a.magnitude())
    #print the first set of coordinates
    if (x_1 == 0 and y_1 == 0):
        continue
    print("Coordinates 1 round:", flag)
    print("Coordinates 1:", x_1, y_1)

    ##one condition that guarantee that we will get two pairs of coordinates every time
    #x_1_prev = x_1
    #y_1_prev = y_1
    #start timer to count time between coordinates

    t_0 = pyb.millis()
    pyb.delay(200)
    #take a new snapshot(with lens corrected)for second position of puck
    img2 = sensor.snapshot().lens_corr(1.3)

    #while(x_2 == x_2_prev and y_2 == y_2_prev):
        #flag2 = flag2 + 1
    #while(1):
    #for loop to find the puck in the image **alter r_min and r_max based on puck**
    #while (not(x_1 == 0 and y_1 == 0) and (x_2 == 0 and y_2 ==0)):
    for b in img2.find_circles(threshold = 3600, x_margin = 10, y_margin = 10, r_margin = 10, r_min = 5, r_max = 12, r_step = 2):
            img2.draw_circle(b.x(), b.y(), b.r(), color = (255, 0, 0)) #draws a circle on the image
        #get coordinates from the second position of the puck
            x_2 = b.x()
            y_2 = b.y()
            #print("hello")
            print("coordinate2 m:")
            print(b.magnitude())
            #print(b)
    print("Coordinates 2 round:", flag)
    flag = flag + 1
    print("Coordinates 2:", x_2, y_2)
    ##jump this loop if x_2 == 0 && y_2 == 0
    if (x_2 == 0 and y_2 == 0):
        continue
    #print the second set of coordinates
    print("Coordinates 2:", x_2, y_2)
    ##one condition that guarantee that we will get two pairs of coordinates every time
    #x_2_prev = x_2
    #y_2_prev = y_2
    #calculate the amount of time(in seconds) between the two coordinates
    t_b = pyb.elapsed_millis(t_0)
    #get the distance(in pixels) between the two coordinates
    dis = distance(x_1, y_1, x_2, y_2)
    print("Distance between the coordinates ", dis)
    #get the rate at which the puck is traveling(pixels/sec)
    rate_p = dis / t_b
    #print the time between the two coordinates
    print("Time between coordinates: ", t_b)
    #print the rate of the puck
    print("Rate of puck(pixels/s): ", rate_p)
    #print the FPS
    print("FPS %f" % c_fps.fps())#print FPS+

    if (x_2 >= x_1):
        continue

        #data1 = bytearray([0xA5, x_3, , 0x5B])
        #uart.write(data1)
    #calculate the slope and trajectory
    #we already know xs, ym,
    #(xc, yc) is the first pair coordinate of puck
    #assign(xc, yc) from what we got from camera before
    xc = x_1
    yc = y_1
    #(xp, yp) is the second pair coordinate of puck
    #assign(xp, yp) from what we got from camera before
    xp = x_2
    yp = y_2

    #know whether the puck will hit the edge
    #using(xs,ym) and (xc, yc) to calculate the k1(slope)
    k1 = (yc - ym) / (xc - xs + 0.000001)
    #using(xs,0) and (xc, yc) to calculate the k2(slope)
    k2 = (yc - 120) / (xc - xs + 0.000001)



    #k(slope) of trajectory of puck
    #using(xp, yp) and (xc, yc) to calculate the k(slope)
    k = (yp - yc) / (xp - xc + 0.000001)

    print("k:", k)
    print("k1: k2:", k1, k2)
    #if k2 < k < k1
    #1:the puck will not hit the edge
    if (k2 < k and k < k1):
    #based on equation to get the coordinate of ys, because we have already gotten (xc, yc) and xs
        if(rate_p <= 0.2):
            xs = 40

        ys = yc - k * (xc - xs + 0.000001)
        print("No hitting")

        x_3 = int(xs)
        y_3 = int(ys)
        if(y_3 > 120):
            continue
        if(y_3 >= 85):
            y_3 = 85
        if(y_3 <= 36):
            y_3 = 36

        print("x_3:", x_3)
        print("y_3:", y_3)
        data = bytearray([0xA5, x_3, y_3, 0x5B])
        uart.write(data)
        pyb.delay(2000)


    #time.sleep(5)
    x_1 = 0 #x coordinate location 1
    y_1 = 0 #y coordinate location 1
    x_2 = 0 #x coordinate location 2
    y_2 = 0 #y coordinate location 2
    xs = 15


