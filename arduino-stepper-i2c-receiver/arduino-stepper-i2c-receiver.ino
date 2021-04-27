// Arduino Stepper I2C Receiver

// A stepper motor driver acting as I2C receiver
// Receive a single byte from I2C controller:
// 1 to activate and move stepper
// 0 to disable

#include <Wire.h>
#include <SpeedyStepper.h>

#define MAX_SPEED 800.0 // mm per second
#define ACCELERATION 800.0 // mm per second squared
#define DIST_OF_TRAVEL 800.0 // mm
#define STEPS_PER_MM 0.5 // 65mm radius at 200 steps per revolution = 200 / 2 x pi x 65

const int MOTOR_STEP_PIN = 3;
const int MOTOR_DIRECTION_PIN = 4;
const int MOTOR_ENABLE = 5;

// create stepper motor object
SpeedyStepper stepper;

volatile boolean lift = false;
volatile boolean lower = false;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);  
  Wire.begin(4);                // i2c bus address 4
  Wire.onReceive(receiveEvent); // register event

  // connect and configure stepper motor
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  stepper.setStepsPerMillimeter(STEPS_PER_MM);
  stepper.setSpeedInMillimetersPerSecond(MAX_SPEED);
  stepper.setAccelerationInMillimetersPerSecondPerSecond(ACCELERATION);

  digitalWrite(MOTOR_ENABLE, HIGH);
}

void loop()
{
  if (lift) doLift();
  if (lower) doDrop();
  delay(1);
}

// executes whenever data is received from controller 
void receiveEvent(int howMany)
{
  while (Wire.available()) {
    if (int x = Wire.read())
      lift = true; // 1 to lift
    else
      lower = true;  // 0 to drop   
  }
}

void doLift() {
  // move stepper by distance - holds until ENABLE pin goes HIGH
  lift = false;
  digitalWrite(MOTOR_ENABLE, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  stepper.moveRelativeInMillimeters(-DIST_OF_TRAVEL);  
}

void doDrop() {
  // disable ENABLE pin - motor disengages
  lower = false;
  digitalWrite(MOTOR_ENABLE, HIGH);  
  digitalWrite(LED_BUILTIN, LOW);
}
