#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  if (!ina219.begin()) {
    // If the sensor is not found, stop the execution
    Serial.println("INA219 not found");
    while (1) {
      delay(10);
    }
  }
//  Serial.println("INA219 Found!");
}

void loop() {
  // Get the shunt voltage (voltage drop across the shunt resistor)
  float shunt_volt = ina219.getShuntVoltage_mV();
  
  // Get the current in mA (directly, without dividing by 10,000)
  float current = ina219.getCurrent_mA()/ 1000.0;
  
  // Get the power in mW (directly, without dividing by 10,000)
  float power = ina219.getPower_mW()/ 1000.0;

  // Print the values for better insight
//  Serial.print("Shunt Voltage: ");
//  Serial.print(shunt_volt);
//  Serial.print(" mV, Current: ");
//Serial.println(current);
//  Serial.print(" mA, Power: ");
  Serial.println(power);
//  Serial.println(" mW");

  // Add some delay for stability in reading
  delay(20); // Increased the delay for more stable output
}


// #include <Wire.h>
// #include <Adafruit_INA219.h>

// Adafruit_INA219 ina219;

// void setup() {
//   Serial.begin(9600);
//   while (!Serial) {
//     delay(10);
//   }

//   if (!ina219.begin()) {
//     while (1) {
//       delay(10);
//     }
//   }
// }

// void loop() {
//   float shunt_volt = ina219.getShuntVoltage_mV();
// //  float bus_volt = ina219.getBusVoltage_V();
//   float current = ina219.getCurrent_mA() ; 
//   float power = ina219.getPower_mW();
//     Serial.println(power);
// //    Serial.print(shunt_volt);
// //    Serial.print(bus_volt);
// //    Serial.print(current);

//   delay(10);
// }
