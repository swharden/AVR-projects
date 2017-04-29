#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#define I2C_READ 0x01
#define I2C_WRITE 0x00

void i2c2_init(void);
uint8_t i2c2_start(uint8_t address);
uint8_t i2c2_write(uint8_t data);
uint8_t i2c2_read_ack(void);
uint8_t i2c2_read_nack(void);
uint8_t i2c2_transmit(uint8_t address, uint8_t* data, uint16_t length);
uint8_t i2c2_receive(uint8_t address, uint8_t* data, uint16_t length);
uint8_t i2c2_writeReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length);
uint8_t i2c2_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length);
void i2c2_stop(void);

#endif // I2C_MASTER_H
