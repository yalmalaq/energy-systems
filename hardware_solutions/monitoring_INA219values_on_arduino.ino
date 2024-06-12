#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  if (!ina219.begin()) {
    while (1) {
      delay(10);
    }
  }
}

void loop() {
  float shunt_volt = ina219.getShuntVoltage_mV();
//  float bus_volt = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA() ; 
  float power = ina219.getPower_mW();
    Serial.println(power);
//    Serial.print(shunt_volt);
//    Serial.print(bus_volt);
//    Serial.print(current);

  delay(10);
}
