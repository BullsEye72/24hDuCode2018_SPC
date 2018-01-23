#include <Wire.h>
#include <Servo.h>
#include <vl53l0x_class.h>


TwoWire WIRE1(14, 15);  //SDA=PB11 & SCL=PB10
VL53L0X sensor_vl53l0x(&WIRE1, 4, 7); //XSHUT=PC6 & INT=PC7
Servo fs90r_L;
Servo fs90r_R;
int servo_L = 6;
int servo_R = 5;
int compteur = 0;

/* Setup ---------------------------------------------------------------------*/

void setup() {
  int status;
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial for output.
  Serial.begin(115200);

  fs90r_L.attach(servo_L, 900, 2100); // attaches the servo on pin 9 to the servo object
  fs90r_R.attach(servo_R, 900, 2100); // Be carefull to min and max values..

  // Initialize I2C bus.
  WIRE1.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X top component.
  status = sensor_vl53l0x.InitSensor(0x10);
  if(status)
  {
    Serial.println("Init sensor_vl53l0x failed...");
  }
}


/* Loop ----------------------------------------------------------------------*/

void loop() {
  // Led blinking.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  // Read Range.
  uint32_t distance;
  int status;
  status = sensor_vl53l0x.GetDistance(&distance);

  Serial.println(distance);

  if(distance < 130)
  {
     if(status == 0)
    {
      compteur++;
    }

    if(compteur == 5)
    {
      fs90r_L.write(100);              // sets the servo speed according to the scaled value
      fs90r_R.write(80);              // sets the servo speed according to the scaled value
      //Serial.println("Tu m'as réveillé !");
    }
  
    if(compteur > 5)
    {
      fs90r_L.write(90);              // sets the servo speed according to the scaled value
      fs90r_R.write(90);              // sets the servo speed according to the scaled value
      //Serial.println("Je suis fatigué je retourne me coucher...");
      compteur = 0;
    }

    if (status == VL53L0X_ERROR_NONE)
    {
      // Output data.
      char report[64];
      snprintf(report, sizeof(report), "| Distance [mm]: %ld |", distance);
      //Serial.println(report);
    }
  }
}
