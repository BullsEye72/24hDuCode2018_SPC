
/*
 Use servo library to control a FS90R motor

 Motor information : 
   http://www.feetechrc.com/product/analog-servo/micro-1-3kg-cm-360-degree-continuous-rotation-servo-fs90r/
   Must respect the pulse range in order not to damage servo.
   
*/

#include <Servo.h>

Servo fs90r_L;  // create servo object to control a servo
Servo fs90r_R;

int potpin = 0;   // analog pin used to connect the potentiometer
int val;
int val_R;          // variable to read the value from the analog pin
int val_L;
int servo_L = 6; // pin used to connect the servo
int servo_R = 5;

void setup() {
  Serial.begin(115200);

  fs90r_L.attach(servo_L, 900, 2100); // attaches the servo on pin 9 to the servo object
                                     // Be carefull to min and max values...
  fs90r_R.attach(servo_R, 900, 2100);                                   
}

void loop() {
  val = analogRead(potpin);        // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180); // scale it to use it with the servo (value between 0 and 180, at 90, 
                                   // motor is stopped)

  val_R = val;
  val_L = val;
  
  Serial.print("val = ");
  Serial.println(val);

  fs90r_L.write(val_L);                // sets the servo speed according to the scaled value
  fs90r_R.write(180-val_R);                // sets the servo speed according to the scaled value
  delay(15);
}


