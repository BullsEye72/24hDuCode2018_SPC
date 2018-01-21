#include <Wire.h>
#include <vl53l0x_class.h>



TwoWire WIRE1(14, 15);  //SDA=PB11 & SCL=PB10
VL53L0X sensor_vl53l0x(&WIRE1, 4, 7); //XSHUT=PC6 & INT=PC7

  int status;
  // Led.
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize serial for output.
  Serial.begin(115200);

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

void loop() 
{
  // Led blinking.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  // Read Range.
  uint32_t distance;
  int status;
  status = sensor_vl53l0x.GetDistance(&distance);

  if (status == VL53L0X_ERROR_NONE)
  {
    // Output data.
    char report[64];
    snprintf(report, sizeof(report), "| Distance [mm]: %ld |", distance);
    Serial.println(report);
  }
}
