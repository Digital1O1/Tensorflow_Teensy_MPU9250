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

#include <Arduino.h>

//Tensorflow Libraries
#include <TensorFlowLite.h>
#include <tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/experimental/micro/micro_error_reporter.h>
#include <tensorflow/lite/experimental/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>
#include "model_new.h"

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::ops::micro::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];

// array to map gesture index to a name
const char* GESTURES[] = {
  "punch",
  "flex",
  //"fall"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

//Other Arduino Libraries
#include "MPU9250.h"

#define OUTPUT_READABLE_ACCELGYRO
#define NUM_SAMPLES 119
#define NUM_AXES 7
#define TRUNCATE 20
#define ACCEL_THRESHOLD 5

//Teensy pins
MPU9250 IMU(SPI, 10);
int status;
const int number_Samples = 119;
int samples_Read = number_Samples;
// NUM_AXES has to be +1 than the number of DOF present/being used due to null character in C-string
double baseline[NUM_AXES];
float ax, ay, az, gx, gy, gz = 0.0;


// ========================================== FUNCTIONS ========================================== //

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
      Serial.println("========================================== TEST 1 ==========================================");

  }
      Serial.println("========================================== TEST 2 ==========================================");

  // This will 'zero' out everything
  baseline[0] = ax;
  baseline[1] = ay;
  baseline[2] = az;
  baseline[3] = gx;
  baseline[4] = gy;
  baseline[5] = gz;
      Serial.println("========================================== TEST 3 ==========================================");

  Serial.println("================================== | Calibration done | ==================================");

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
    delay(25);
    return false;
  }

}

void read_IMU(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
  // read the sensor
  IMU.readSensor();

  *ax = (IMU.getAccelX_mss() - baseline[0]);
  *ay = (IMU.getAccelY_mss() - baseline[1]);
  *az = (IMU.getAccelZ_mss() - baseline[2]);
  *gx = IMU.getGyroX_rads() -  baseline[3];
  *gy = IMU.getGyroY_rads() -  baseline[4];
  *gz = (IMU.getGyroZ_rads() * 100) - baseline[5];
}

void display_Data()
{
  Serial.print(ax); Serial.print(',');
  Serial.print(ay); Serial.print(',');
  Serial.print(az); Serial.print(',');
  Serial.print(gx); Serial.print(',');
  Serial.print(gy); Serial.print(',');
  Serial.println(gz);
}


// ========================================== FUNCTIONS ========================================== //

void setup()
{
  // Start serial communication
  Serial.begin(115200);

  // Waits for serial connection
  while (!Serial) {}
  Serial.println("========================================== ESTABLISHED SERIAL COMMUNICATION ==========================================");

  // Start communication with IMU
  status = IMU.begin();
  if (status < 0)
  {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1)
    {}
  }
  Serial.println("========================================== ABOUT TO CALIBRATE ==========================================");

  calibrate();
  Serial.println("========================================== IMU CALIBRATED ==========================================");

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION)
  {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop()
{
  // Waits for movement from IMU sensor --> Reset counter --> Runs remaining portion of program
  while (samples_Read == number_Samples)
  {
    //Serial.println("MOVE SENSOR TO RESET COUNTER ");
    if (motionDetected(ax, ay, az))
    {
      //Serial.println("MOTION DETECTED");
      //delay(1000);
      //Serial.println("========================================== COUNTER RESET ==========================================");
      samples_Read = 0;
      //delay(1000);
      //return;
    }
    else
    {
      //Serial.println("------ MOVE IMU SENSOR TO COLLECT DATA ------");
    }
  }


  while (samples_Read < number_Samples)
  {
    // Read data from IMU sensor
    //read_IMU(&ax, &ay, &az, &gx, &gy, &gz);

    // If no motion is detected, loop here until then
    if (motionDetected(ax, ay, az))
    {
      read_IMU(&ax, &ay, &az, &gx, &gy, &gz);
      // Display the data
      //display_Data();

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samples_Read * 6 + 0] = (ax + 4.0) / 8.0;
      tflInputTensor->data.f[samples_Read * 6 + 1] = (ay + 4.0) / 8.0;
      tflInputTensor->data.f[samples_Read * 6 + 2] = (az + 4.0) / 8.0;
      tflInputTensor->data.f[samples_Read * 6 + 3] = (gx + 2000.0) / 4000.0;
      tflInputTensor->data.f[samples_Read * 6 + 4] = (gy + 2000.0) / 4000.0;
      tflInputTensor->data.f[samples_Read * 6 + 5] = (gz + 2000.0) / 4000.0;


      // Counter used to keep track on number of samples
      samples_Read++;
      //delay(20);

      // If the sample_Read counter hits 119 --> Reset counter --> Insert blank line for CSV file
      if (samples_Read == number_Samples)
      {
        //samples_Read = 0;
        //Serial.println();
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk)
        {
          Serial.println("Invoke failed!");
          while (1);
          // The return statement will cause the program to start back at the beginning
          return;
        }

        // Loop through the output tensor values from the model
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
          //delay(3000);
        }
        Serial.println();
      }

    }// End of 'if (motionDetected(ax, ay, az))'



  } // End of 'while (samples_Read < number_Samples)'

  delay(20);

}// End of 'void loop()'
