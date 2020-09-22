## === [ Introduction ] === 

### All of this was done in VS Code with the [Platform IO extension](https://platformio.org/platformio-ide)

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

### --- [ Step 1 : SAMPLE IMU DATA WITH RECORD_IMU CODE] ---

![image](https://user-images.githubusercontent.com/39348633/93822988-ad058b80-fc26-11ea-8383-e3c332253ab8.png)

In the Arduino ML blog highlights the Nano 33's ML capability of recongizing if the user is either flexing with the board in hand, or throwing a punch, which is also achievable in this repository. 

In the Google Colab Python script that will be discussed later, takes 119 individual IMU readings and consideres that ONE sample.

With how the Record_IMU code is set up, it'll stop sampling the MPU-9250 data after 20 samples. 

After uploading the Record_IMU code to the Teensy make sure throughout the sampling process TO KEEP GOING THROUGH THE MOTIONS. 

The program will automatically stop after taking 20 samples.

Any "second-guessing" can/will affect the Teensy's "ability" to recognize the gesture properly.

For those who are wanting to sample more/less can do so by changing the "SAMPLE_LIMIT" numerical value as shown in the screenshot above.

When switching from sampling one gesture to another, the only thing you have to do is just :
 * Exit out of Putty
 * Press the onboard "Reset" button on the Teesny circled in red in the screenshot below
 * Continue sampling IMU data with whatever motion/gesture you have left.
 
 ![image](https://user-images.githubusercontent.com/39348633/93826271-80547280-fc2c-11ea-9561-bce5e5fea960.png)


### --- [ Step 1a : CONVERT DATA TO CSV FILE] ---

So I used a terminal emulator called [PuTTy](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) to "rip" the IMU data.

To do so, you'll have to know which COM port your Teensy board is doing before hand, which can be found in you device manager --> Ports (COM & LPT)

After you downloaded/installed Putty and determined which COM port is being used, open Putty and change the following : 

 * Speed --> 115200
 * Connection Type --> Serial
 * Go to Session --> Logging --> Session Logging --> Printable output
   * Desingate where you want the IMU data to be saved, which I just used my desktop cause I'm lazy
   * Ensure the following boxes are checked 
     * Ask the user every time 
     * Flush Log file frequently 
     
 The screenshots below can be used for reference
 
![image](https://user-images.githubusercontent.com/39348633/93729467-2ef3a700-fb8a-11ea-9159-7315c5d26f16.png)![image](https://user-images.githubusercontent.com/39348633/93821435-2bacf980-fc24-11ea-86dd-8e407940872f.png)

Once you exit out of Putty, a log file will be generated.

### --- [ Step 1b : Log file --> CSV file ] ---

So now we'll have to change the log file that has all the IMU data to a TXT file, then a CSV file in order to 'feed' the data to the Google Colab Python script I mentioned earlier.

In order to do so, you'll have to enable 'Show file extensions"

To do this in Windows 10 is as followed :
 * Go to your search bar and type "File Explorer"
 * Click on "VIEW" in the File explorer 
 * Check the box next to "File name extension" 
 * Close the "File explorer"

A better write up on the matter can be found [here](https://support.winzip.com/hc/en-us/articles/115011457948-How-to-configure-Windows-to-show-file-extensions-and-hidden-files)

NEXT STEP --> Right click and rename the "IMU_Data.log" file to --> "IMU_Data.csv" 

A warning will pop up, but it's not a big deal from my experience.

So now with the CSV files ready to go, we can then upload them to the Google Colab to generate the "weights" which will help distinguish if the user is either flexing or punching.

### --- [ Step 2 : Using Google Colab To Generate "Weights" ] ---

The original Google Colab that's being used for the Arduino ML tutorial can be found [here](https://colab.research.google.com/github/arduino/ArduinoTensorFlowLiteTutorials/blob/master/GestureToEmoji/arduino_tinyml_workshop.ipynb) 

I also uploaded my own [Google Colab](https://github.com/Digital1O1/Tensorflow_Teensy_MPU9250/blob/master/Teensy_ML_Script.ipynb) that has notes that I've written in order to get a better understanding of what's going on.

Anyways... To upload the CSV files, you just simply drag and drop them into the area circled in red

![image](https://user-images.githubusercontent.com/39348633/93828362-c3b0e000-fc30-11ea-8596-bc94dc002d5c.png)

After running each cell, the Google Colab will generate a Model.h file that contains all the "weights" to be used in the IMU_Classifier.ino/cpp file

### --- [ Step 3 : Upload Modified IMU_Classifier to Teensy ] ---

In both the Arduino IDE and VS Code, make sure to include the model.h header file in the same folder as the main.cpp/sketch is located
 * Just in case if that sounded vauge for the VS Code users
   * Open VS Code --> File --> Open Folder --> Pick IMU_Classifier --> Go to File Explorer --> Src --> Right Click main.cpp --> Click "Reveal in File Explorer --> Drag and Drop      model.h in same folder
   * make sure to use : #include "model.h"
   
After uploading the code to the Teensy board, open your serial monitor and when you flex, or throw a punch the serial monitor should return values that look something like this

![image](https://user-images.githubusercontent.com/39348633/93834049-22ca2100-fc40-11ea-8ce9-1a182ddc0444.png)

### PLEASE KEEP IN MIND

The Teeny's ability to accurately predict the gestures ISN'T GOING TO BE PERFECT

There's still a lot that I have to teach myself regarding machine learning, A.I, and all the nitty gritty details in between.

So to put it short as of September 21st, 2020, I will continue to refine/revise this repo as I learn more down the road.

So in the meantime, this is all I have for now.

And I really hope this helps someone out there on the interwebs.
