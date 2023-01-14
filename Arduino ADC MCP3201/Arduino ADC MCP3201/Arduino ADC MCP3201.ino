// MCP3201 Arduino

#include <SPI.h>
const byte DAT = 12;
const byte CLK = 13;
const byte CS = 10;
const float V_REF = 4.096;

void setup() {

  Serial.begin(115200);
  
  SPI.beginTransaction(SPISettings(1500000, MSBFIRST, SPI_MODE0));
  SPI.begin();

  pinMode(DAT, INPUT);
  pinMode(CS, OUTPUT);

  // toggle CS pin once to reset it
  digitalWrite(CS, LOW); 
  digitalWrite(CS, HIGH);

  digitalWrite(CLK, LOW);
}

void loop() {

  unsigned int reading = 0;

  digitalWrite(CS, LOW);
  reading = SPI.transfer16(0);
  digitalWrite(CS, HIGH);

  reading = reading << 3;  // shift 3 MSBs (trash bits) out
  reading = reading >> 4;  // shift 1 LSB (trah bit) out

  float voltage = reading * V_REF / 4095;
  Serial.println(voltage, 3);

  //delay(10);
}