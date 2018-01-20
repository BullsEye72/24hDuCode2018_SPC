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

int speedval=20;
int turnFact=3;

void setup()
{ 
  Serial.begin(115200);

  fs90r_L.attach(servo_L, 900, 2100); // attaches the servo on pin 9 to the servo object
  fs90r_R.attach(servo_R, 900, 2100); // Be carefull to min and max values..
  
  pinMode(sensor_R,INPUT);
  pinMode(sensor_L,INPUT);
}

int trackingState=0;
bool lost=false;

void checkAndUpdateState()
{
  detect_SL=digitalRead(sensor_L);
  detect_SR=digitalRead(sensor_R);
Serial.print(trackingState);
Serial.print(lost);
Serial.println("==========");
  if(trackingState == 0 && (detect_SL==LOW || detect_SR==LOW))
  {
    Serial.println("0");
    trackingState=1;
    lost=false;
    return;
  }

  if(!lost && (detect_SL==HIGH || detect_SR==HIGH)){
    Serial.println("1");
    lost = true;
    if(detect_SL==HIGH) { trackingState=3; }
    if(detect_SR==HIGH) { trackingState=2; }
    return;
  }

  if(lost && trackingState==2 && detect_SL==LOW){
    Serial.println("2");
    lost=false;
    trackingState=1;
    return;
  }

  if(lost && trackingState==3 && detect_SR==LOW){
    Serial.println("3");
    lost=false;
    trackingState=1;
    return;
  }

  if(detect_SL==HIGH && detect_SR==HIGH){
    Serial.println("4");
    trackingState=0;
  }
  
  
}

void loop()
{
  switch(trackingState)
  {
    case 1 : //TOUT DROIT !
      fs90r_L.write(90+speedval);
      fs90r_R.write(90-speedval);
      break;
    case 2 : //A GAUCHE !
      fs90r_L.write(90+speedval*turnFact);
      fs90r_R.write(90+speedval*turnFact);
      break;
    case 3 : //A DROITE !
      fs90r_L.write(90-speedval*turnFact);
      fs90r_R.write(90-speedval*turnFact);
      break;
    default: //STOP !
      fs90r_L.write(90);
      fs90r_R.write(90);
      break;
  }

    checkAndUpdateState();
    //Serial.print("State : ");
    //Serial.println(trackingState);
    //delay(5);
}
