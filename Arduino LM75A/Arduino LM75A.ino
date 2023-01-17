#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
}


void loop() {
  byte buffer[2];
  Wire.requestFrom(0x48, 2);
  Wire.readBytes(buffer, 2);

  uint16_t value = buffer[0] << 8;
  value += buffer[1];
  value /= 32;

  Serial.println(value);
}