#include <Stepper.h>
#include <SFE_TSL2561.h>
#include <Wire.h>

// Create an SFE_TSL2561 object, here called "light":
SFE_TSL2561 light;

// Global variables:
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for your motor
boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in millisecond
double oldLux=0;
double lux;
double target = 10;
int ledPin = 13;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4, 5, 6, 7);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(150);
  //limit is about 150
  
  light.begin();
  
  pinMode(ledPin, OUTPUT);
  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)
  gain = 0;
  
  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration
  unsigned char time = 0;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):
  light.setTiming(gain,time,ms);

  // To start taking measurements, power up the sensor:
  light.setPowerUp();
  
  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.

}

void loop() {
  
  // step one revolution  in one direction:
  int dir = 1;
  for (int d = 0; d < 2; ++d) {
    dir = 1 - 2 * d;
    for (int i = 0; i < 12; ++i) {
      myStepper.step(1 * dir);
      
      lightLoop();
    }
  }  
}
  
  
void lightLoop() {
   // Wait between measurements before retrieving the result
  // Once integration is complete, we'll retrieve the data.
  
  // There are two light sensors on the device, one for visible light
  // and one for infrared. Both sensors are needed for lux calculations.
  
  // Retrieve the data from the device:
  unsigned int data0, data1;
  
  if (light.getData(data0,data1))
  {
    // getData() returned true, communication was successful
    
    // To calculate lux, pass all your settings and readings
    // to the getLux() function.
    
    // The getLux() function will return 1 if the calculation
    // was successful, or 0 if one or both of the sensors was
    // saturated (too much light). If this happens, you can
    // reduce the integration time and/or gain.
   
    //double lux;    // Resulting lux value
    boolean good;  // True if neither sensor is saturated
    
    // Perform lux calculation:
    good = light.getLux(gain,ms,data0,data1,lux);
    
    // Monitor lux changes
    monitorLux();
    oldLux = lux;
    
   }
  else
  {
    // getData() returned false because of an I2C error, inform the user.
    byte error = light.getError();
    printError(error);
  }
}



void monitorLux() {
  
  double diff = lux - oldLux;
  
  if (diff > target) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}


void printError(byte error)
  // If there's an I2C error, this function will
  // print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}
