#include <DigitLedDisplay.h>

#include <Wire.h>

int16_t DEVICE_ADDRESS = 0x76;

DigitLedDisplay ld = DigitLedDisplay(7, 6, 5);

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  ld.setBright(15);  // range is 0-15
  ld.setDigitLimit(8);
}

void loop() {
  readCalibrationData();
  setupConfigRegister();
  setupControlRegister();
  waitForMeasurement();
  long pres = getPressure();
  long temp = getTemperature();

  ld.printDigit(temp);

  Serial.print(convertTemperature(temp));
  Serial.print(',');
  Serial.print(temp);
  Serial.print(',');
  Serial.print(pres);
  Serial.println();
}

void readCalibrationData(){
  dig_T1 = ReadInt16_LE(0x88);
  dig_T2 = ReadInt16_LE(0x8A);
  dig_T3 = ReadInt16_LE(0x8C);
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

int16_t ReadInt16_LE(byte address){
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(DEVICE_ADDRESS, 3);
  byte lsb = Wire.read();
  byte msb = Wire.read();

  return (msb << 8) | (lsb);
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

float convertTemperature(long adc_T) {
  adc_T >>= 4;
  int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  int32_t t_fine = var1 + var2;
  float T = (t_fine * 5 + 128) >> 8;
  T = T * 9 / 5 + 3200; // C to F
  return T / 100;
}