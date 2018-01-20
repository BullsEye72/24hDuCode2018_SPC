#include <Servo.h>

Servo fs90r_L;
Servo fs90r_R;
int val_L;
int val_R;
int servo_L = 6;
int servo_R = 5;
int sensor_R = 0;
int sensor_L = 1;
int detect_SL;
int detect_SR;

int speedval=15;

void setup()
{ 
  Serial.begin(115200);

  fs90r_L.attach(servo_L, 900, 2100); // attaches the servo on pin 9 to the servo object
  fs90r_R.attach(servo_R, 900, 2100); // Be carefull to min and max values..
  
  pinMode(sensor_R,INPUT);
  pinMode(sensor_L,INPUT);
}

void loop()
{
  detect_SL=digitalRead(sensor_L);
  detect_SR=digitalRead(sensor_R);
  
  if(detect_SL==LOW && detect_SR==LOW)
  { 
    fs90r_L.write(90+speedval);                // sets the servo speed according to the scaled value
    fs90r_R.write(90-speedval);              // sets the servo speed according to the scaled value
    Serial.println("En avant!");
  }
  else if(detect_SL==LOW && detect_SR==HIGH)
  {
    fs90r_L.write(90+speedval*2);
    fs90r_R.write(90);
    Serial.println("Detecté a D - Go a gauche!");
  }
  else if(detect_SL==HIGH && detect_SR==LOW)
  {
    fs90r_L.write(90);
    fs90r_R.write(90-speedval*2);
    Serial.println("Detecté a G - Go a droite!");
  }
  else
  {
    fs90r_L.write(90);
    fs90r_R.write(90);
    Serial.println("STOOPPPP!");
  }
  delay(15);
}
