#include <SPI.h>

#define MOSI 11
#define MISO 12
#define SCK 13
#define CS 10

volatile long SOURCE_ADDRESS;

char spi_transfer(volatile char data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF))) {};
  return SPDR;
}

void setup() {
  // configure SPI pins
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  // SPI leading edge clock, CPU/4 rate
  SPCR = (1 << SPE) | (1 << MSTR);
  uint8_t trash;
  trash = SPSR;
  trash = SPSR;

  // setup PWM on Timer 2
  pinMode(3, OUTPUT);
  TCCR2A |= bit(COM2B1);
  TCCR2B = bit(CS20);

  SOURCE_ADDRESS = 0;
}

void loop() {
  uint8_t pageA = SOURCE_ADDRESS >> 16;
  uint8_t pageB = SOURCE_ADDRESS >> 8;
  uint8_t pageC = SOURCE_ADDRESS >> 0;

  digitalWrite(CS, LOW);
  spi_transfer(0x03);
  spi_transfer(pageA);
  spi_transfer(pageB);
  spi_transfer(pageC);
  OCR2B = spi_transfer(255);
  digitalWrite(CS, HIGH);

  SOURCE_ADDRESS++;

  delayMicroseconds(88);
}
