#include <Arduino.h>

/*
  [] ========== SPI Connection ========== []

  FSync     --> GND
  NCS == CS --> 10
  ADO       --> 12
  SDA       --> 11
  SCL       --> 13

  [] ========== Github Reference ========== []

  https://github.com/bolderflight/MPU9250

  [] ========== Changing Serial Baud Rate ========== []

  Go to platformio.ini

  Enter : monitor_speed = baudRate

  [] ========== Changing Clock Speed ========== []

  Reference Link : https://community.platformio.org/t/teensy-3-1-3-2-cpu-change-clock-speed/253

  [] ========== Stuff on Arduino Side ========== []

  Make sure in Tools --> USB Type --> Raw HID is selected
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
//Teensy pins
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
  // read the sensor
  IMU.readSensor();

  *ax = (IMU.getAccelX_mss() - baseline[0]);
  *ay = (IMU.getAccelY_mss() - baseline[1]);
  *az = (IMU.getAccelZ_mss() - baseline[2]);
  *gx = IMU.getGyroX_rads() - baseline[3];
  *gy = IMU.getGyroY_rads() - baseline[4];
  *gz = (IMU.getGyroZ_rads() * 100) - baseline[5];
}
void display_Data()
{
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

  // This will 'zero' out everything
  baseline[0] = ax;
  baseline[1] = ay;
  baseline[2] = az;
  baseline[3] = gx;
  baseline[4] = gy;
  baseline[5] = gz;
}

bool motionDetected(float ax, float ay, float az)
{
  read_IMU(&ax, &ay, &az, &gx, &gy, &gz);
  if (abs(ax) + abs(ay) + abs(az) > ACCEL_THRESHOLD)
  {
    return true;
  }

  else
  {
    //Serial.print("Current Acceleration : "); Serial.println(abs(ax + ay + az));
    delay(150);
    return false;
  }
}

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

  // Waits for serial connection
  while (!Serial)
  {
  }

  // Start communication with IMU
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

        // If the sample_Read counter hits 119 --> Reset counter --> Insert blank line for CSV file
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