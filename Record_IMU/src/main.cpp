#include <Arduino.h>

/*
  [] ========== SPI Connection to Teensy 4 board ========== []

  FSync     --> GND
  NCS == CS --> 10
  ADO       --> 12
  SDA       --> 11
  SCL       --> 13

  [] ========== MPU-9250 Library ========== []

  https://github.com/bolderflight/MPU9250

  [] ========== Changing Serial Baud Rate in Visual Code ========== []

  Go to 'Explorer' --> platformio.ini

  Enter the following 

  monitor_speed = 115200
  board_build.f_cpu = 60000000L

  ** Additional documentation about changing Teensy clock speed
  https://community.platformio.org/t/teensy-3-1-3-2-cpu-change-clock-speed/253
*/

#include "MPU9250.h"
#define OUTPUT_READABLE_ACCELGYRO
#define NUM_SAMPLES 119
#define NUM_AXES 7
#define ACCEL_THRESHOLD 2.5
#define SAMPLE_LIMIT 20
/*
    CLOCK FOR TEENSY BOARD HAS TO BE AT 150 MHZ
    Reference Link : https://github.com/bolderflight/MPU9250/issues/63
*/
// Designate which digital pin will trigger CS to low for MPU9250 to send data back to Teensy
MPU9250 IMU(SPI, 10);

int status;
int num_loops = 0;
const int number_Samples = 119;
int samples_Read = number_Samples;
// NUM_AXES has to be +1 than the number of DOF present/being used due to null character in C-string
double baseline[NUM_AXES];
float ax, ay, az, gx, gy, gz = 0.0;

void read_IMU(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
  // Signal the MPU9250 to be read
  IMU.readSensor();

  // Subtract values saved in baseline[] arrays from 'incoming' values --> Zeros the MPU relative to where it was calibrated
  *ax = (IMU.getAccelX_mss() - baseline[0]);
  *ay = (IMU.getAccelY_mss() - baseline[1]);
  *az = (IMU.getAccelZ_mss() - baseline[2]);
  *gx = IMU.getGyroX_rads() - baseline[3];
  *gy = IMU.getGyroY_rads() - baseline[4];
  *gz = (IMU.getGyroZ_rads() * 100) - baseline[5];
}
void display_Data()
{
  // Display data via serial monitor
  Serial.print(ax);
  Serial.print(',');
  Serial.print(ay);
  Serial.print(',');
  Serial.print(az);
  Serial.print(',');
  Serial.print(gx);
  Serial.print(',');
  Serial.print(gy);
  Serial.print(',');
  Serial.println(gz);
}

//This will 'zero' out the IMU sensor in relation to whatever surface it's resting on
void calibrate()
{
  // Read 10 gyroscopic/accelerometer values
  for (int i = 0; i < 10; i++)
  {
    ax = IMU.getAccelX_mss();
    ay = IMU.getAccelY_mss();
    az = IMU.getAccelZ_mss();
    gx = IMU.getGyroX_rads();
    gy = IMU.getGyroY_rads();
    gz = IMU.getGyroZ_rads();

    delay(25);
  }

  // Store the values read in an array
  baseline[0] = ax;
  baseline[1] = ay;
  baseline[2] = az;
  baseline[3] = gx;
  baseline[4] = gy;
  baseline[5] = gz;
}

bool motionDetected(float ax, float ay, float az)
{
  // Read the MPU data
  read_IMU(&ax, &ay, &az, &gx, &gy, &gz);

  // If X/Y/Z acceleromotor values exceed ACCEL_THRESHOLD --> Spit out MPU data to serial montior
  if (abs(ax) + abs(ay) + abs(az) > ACCEL_THRESHOLD)
  {
    return true;
  }

  // If accelerometer data doesn't exceed threshold, just delay 150 ms
  else
  {
    //Serial.print("Current Acceleration : "); Serial.println(abs(ax + ay + az));
    delay(150);
    return false;
  }
}

// Used for troubleshooting purposes when intial code write up
void display_Number_Loops()
{
  Serial.print("LOOP : ");
  Serial.println(num_loops);
  delay(500);
}

void setup()
{
  // Serial to display data
  Serial.begin(115200);

  // Waits for serial connection | Not really sure if this is needed
  while (!Serial)
  {
  }

  // Start communication with IMU, Status should return 1 if everything is okay
  status = IMU.begin();

  if (status < 0)
  {
    Serial.println("ERROR, Check wiring and ensure Clock speed = 150 Mhz");
    Serial.print("Status: ");
    Serial.println(status);
    while (1)
    {
    }
  }
  // "Prints" out the header needed for the CSV file
  Serial.println("aX,aY,aZ,gX,gY,gZ");
  calibrate();
}

void loop()
{
  // Waits for movement from IMU sensor --> Reset counter --> Runs remaining portion of program
  while (samples_Read == number_Samples)
  {
    if (motionDetected(ax, ay, az))
    {
      // Resets the counter
      samples_Read = 0;
    }

    // Keeps track of counter and displays IMU data. Once max readings hits limit, counter resets
    while ((samples_Read < number_Samples) && (num_loops < SAMPLE_LIMIT))
    {
      // Read data from IMU sensor
      read_IMU(&ax, &ay, &az, &gx, &gy, &gz);

      // If no motion is detected, loop here until then
      if (motionDetected(ax, ay, az))
      {
        // Display the data through serial monitor or Putty Terminal
        display_Data();

        // Counter used to keep track on number of samples
        samples_Read++;
        delay(10);

        // Once the sample_Read counter hits 119 --> Counter resets --> Blank line inserted in log/csv file
        if (samples_Read == number_Samples)
        {
          samples_Read = 0;
          num_loops++;
          //display_Number_Loops();
          Serial.println();
        }

      } // End of 'if (motionDetected(ax, ay, az))'

    } // End of ' while (samples_Read < number_Samples)'

  } // End of 'while (samples_Read == number_Samples)'

} // End of void loop()
