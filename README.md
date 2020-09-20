## --- Introduction --- ##
 
On October 15, 2019 Arduino had posted a [blog/tutorial](https://blog.arduino.cc/2019/10/15/get-started-with-machine-learning-on-arduino/) on how to use their new Arduino Nano 33 IoT board
 
![image](https://user-images.githubusercontent.com/39348633/93692108-9dfecc00-fab4-11ea-9ec8-d777d6c8ebf8.png)
 
The Nano 33 comes smack dab with numerous SMD components already soldered on in a nice, neat, little package.
 
However if you're like me and have an ARM based board like the Teensy 4, an MPU-9250, and are itching to get into Machine Leanring, you're at the right spot.
 
The overarching goal for this repository will esentially "mimick" all the steps stated in the [Arduino blog post](https://blog.arduino.cc/2019/10/15/get-started-with-machine-learning-on-arduino/)
and to get the Teensy to distinguish a punching motion from a flexing motion by sampling the incoming data from the MPU-9250
 
## --- Wiring Everything Up --- ##
 
As pictured below is the pinout for the Teensy 4
 
![image](https://user-images.githubusercontent.com/39348633/93692469-4f9ffc00-fab9-11ea-92a1-0242fd0c7c85.png)
 
The MPU-9250 is a affordable, internal measurement unit that has 9-degrees of freedom and will be interfaced with the Teensy 4 board via SPI. For those not familiar with SPI, here's a good [YouTube](https://www.youtube.com/watch?v=fvOAbDMzoks&ab_channel=GreatScott%21) video for reference.
 
-----------
So.... The wiring of the MPU-9250 and the Teensy board is as followed : 
 
MPU-9250 Pin | Teensy Pin 
------------ | ------------
VDD | 3 Volts
GND | GND on Teensy (Shocker, I know....)
SCL | Pin 13
SDA | Pin 11
ADO | Pin 12
INT and FSYNC | GND
NCS | Pin 10
 
For those wanting to know what FSYNC/ADO/SDA and SCL mean, the following screenshot was taken from [SparkFun's tutorial](https://learn.sparkfun.com/tutorials/mpu-9250-hookup-guide/all) on how to hook up their own version of the MPU-9250.
 
![image](https://user-images.githubusercontent.com/39348633/93692709-07cea400-fabc-11ea-8405-e53f344fe765.png)
 
-----------
 
 
