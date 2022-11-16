#include <Wire.h>

int16_t DEVICE_ADDRESS = 0x76;

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  setupConfigRegister();
  setupControlRegister();
  waitForMeasurement();
  //Serial.println(getPressure());
  Serial.println(getTemperature());
}

void waitForMeasurement() {
  while (true) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write(0xF3);
    Wire.write(0xF3);
    Wire.endTransmission();
    byte status = Wire.read();
    if (status & 0b00001000)
      return;
  }
}

void setupConfigRegister() {
  byte config = 0;
  config |= 0b00000000;  // t_sb (0.5ms)
  config |= 0b00000000;  // filter (disable filter)
  config |= 0b00000000;  // spi3w_en (disable 3-wire)

  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0xF5);
  Wire.write(config);
  Wire.endTransmission();
}

void setupControlRegister() {
  byte ctrl = 0;
  ctrl |= 0b00100000;  // oversampling (x1)
  ctrl |= 0b00000100;  // osrs_p (disable oversampling)
  ctrl |= 0b00000011;  // mode (normal)

  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0xF4);
  Wire.write(ctrl);
  Wire.endTransmission();
}

long getPressure() {
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0xF7);
  Wire.endTransmission();

  Wire.requestFrom(DEVICE_ADDRESS, 3);
  byte press_msb = Wire.read();
  byte press_lsb = Wire.read();
  byte press_xlsb = Wire.read();

  long result;
  result += (long)press_msb << 16;
  result += (long)press_lsb << 8;
  result += (long)press_xlsb;

  return result;
}

long getTemperature() {
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(0xFA);
  Wire.endTransmission();

  Wire.requestFrom(DEVICE_ADDRESS, 3);
  byte temp_msb = Wire.read();
  byte temp_lsb = Wire.read();
  byte temp_xlsb = Wire.read();
  
  long result;
  result += (long)temp_msb << 16;
  result += (long)temp_lsb << 8;
  result += (long)temp_xlsb;

  return result;
}