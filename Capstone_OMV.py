# Capstone_OMV - By: shawn - Wed Jan 5 2022
#import libraries
import pyb, sensor, image, time, math
#global variables
x_1 = 0 #x coordinate location 1
y_1 = 0 #y coordinate location 1
x_2 = 0 #x coordinate location 2
y_2 = 0 #y coordinate location 2
x_3 = 0 #x coordinate location 3
y_3 = 0 #y coordinate location 3
#set up the camera
sensor.reset()#initializes camera sensor
sensor.set_pixformat(sensor.GRAYSCALE)#pixel format is grayscale 8bits/pixel
sensor.set_framesize(sensor.QQVGA)#sets framesize to 160x120
sensor.skip_frames(time = 2000)#let camera stabilize after changing settings
#start clock for FPS
c_fps = time.clock()#Create a clock object to track the FPS.
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
    img1 = sensor.snapshot().lens_corr(1.8)
    #for loop to find the puck in the image **alter r_min and r_max based on puck**
    for a in img1.find_circles(threshold = 2000, x_margin = 10, y_margin = 10, r_margin = 10,
            r_min = 2, r_max = 100, r_step = 2):
        img1.draw_circle(a.x(), a.y(), a.r(), color = (255, 0, 0)) #draws a circle on the image
        #get coordinates from the first position of the puck
        x_1 = a.x()
        y_1 = a.y()
        print(a)
    #print the first set of coordinates
    print("Coordinates 1:", x_1, y_1)
    #start timer to count time between coordinates
    t_0 = pyb.millis()
    #take a new snapshot(with lens corrected)for second position of puck
    img2 = sensor.snapshot().lens_corr(1.8)
    #for loop to find the puck in the image **alter r_min and r_max based on puck**
    for b in img2.find_circles(threshold = 2000, x_margin = 10, y_margin = 10, r_margin = 10,
            r_min = 2, r_max = 100, r_step = 2):
        img2.draw_circle(b.x(), b.y(), b.r(), color = (255, 0, 0)) #draws a circle on the image
        #get coordinates from the second position of the puck
        x_2 = b.x()
        y_2 = b.y()
        print(b)
    #print the second set of coordinates
    print("Coordinates 2:", x_2, y_2)
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
