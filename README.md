## === [ ntroduction ] === 

### All of this was done in VS Code with the Platform IO extension

This can still be used for the Arduino IDE, to do so :
 * go to src --> Main.cpp and comment out the #include <Arduino.h> everything will still work

### === [ Overview ] ===
* Intro about Arduino Nano 33
* Wiring up MPU9250 with Teensy 4
* How to lower clock speed in Arduino IDE | Additional setting parameters to be set in Visual Code
* Breif introduction to Python Script used

 -----------
 
On October 15, 2019 Arduino had posted a [blog/tutorial](https://blog.arduino.cc/2019/10/15/get-started-with-machine-learning-on-arduino/) on how to use their new Arduino Nano 33 IoT board
 
![image](https://user-images.githubusercontent.com/39348633/93692108-9dfecc00-fab4-11ea-9ec8-d777d6c8ebf8.png)
 
The Nano 33 comes smack dab with numerous SMD components already soldered on in a nice, neat, little package.
 
However if you're like me and have an ARM based board like the Teensy 4, an MPU-9250, and are itching to get into Machine Learning, you're at the right spot.
 
The overarching goal for this repository will esentially "mimick" all the steps stated in the [Arduino blog post](https://blog.arduino.cc/2019/10/15/get-started-with-machine-learning-on-arduino/)
and to get the Teensy to distinguish a punching motion from a flexing motion by sampling the incoming data from the MPU-9250
 
## === [ Wiring Everything Up ] === 
 
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
NCS/CS | Pin 10
 
For those wanting to know what FSYNC/ADO/SDA and SCL mean, the following screenshot was taken from [SparkFun's tutorial](https://learn.sparkfun.com/tutorials/mpu-9250-hookup-guide/all) on how to hook up their own version of the MPU-9250.
 
![image](https://user-images.githubusercontent.com/39348633/93692709-07cea400-fabc-11ea-8405-e53f344fe765.png)
 
-----------
 
 ## ==== [ LOOK HERE LIKE RIGHT MEWO ] ====
 
 ### If using the Arduino IDE
 
 You'll have to lower the clock speed to get the data off the MPU9250.
 
 In order to do so to go :
 
  * Tools --> CPU Speed --> 150 Mhz
  
 ### If using VS Code with the Platform IO extension
 
 * Click on your "platformio.ini
 * Make sure the following lines are there, if not, add them :
   * monitor_speed = 115200
   * board_build.f_cpu = 60000000L
 
## === [ Everything Is Wired Up Now What? ] ===

### --- [ Step 1 : SAMPLE IMU DATA ] ---

![image](https://user-images.githubusercontent.com/39348633/93728765-fc947a80-fb86-11ea-8c2c-7d4faab644ac.png)

In the Google Colab Python script that will be discussed later takes 119 individual IMU readings and consideres that ONE sample.

The way how the Teeny/Arduino code is currently set up once 20 sample are taken then sampling is halted.

### --- [ Step 1a : CONVERT DATA TO CSV FILE] ---

So I used a terminal emulator called [PuTTy](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) to "rip" the IMU data.

To do so, you'll have to know which COM port your Teensy board is doing, which can be found in you device manager --> Ports (COM & LPT)

After you downloaded/installed Putty and determined which COM port is being used, open Putty and change the following : 

 * Speed --> 115200
 * Connection Type --> Serial
 * Go to Session --> Logging --> Session Logging --> Printable output
   * Desingate where you want the IMU data to be saved, which I just used my desktop cause I'm lazy
   * Ensure the following boxes are checked 
     * Ask the user every time 
     * Flush Log file frequently 
     
 The screenshots below can be used for reference
 
 ![image](https://user-images.githubusercontent.com/39348633/93729467-2ef3a700-fb8a-11ea-9159-7315c5d26f16.png)
 ![image](https://user-images.githubusercontent.com/39348633/93729553-a1fd1d80-fb8a-11ea-91c1-7723b6e8a11e.png)


