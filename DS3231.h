#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>
#include "pico/util/datetime.h"
#include "hardware/i2c.h"

#define DS3231_ADDR 0x68
#define I2C_TIMEOUT_CHAR 500
#define I2C_REG_READ 1
#define I2C_REG_WRITE 0

#define CHECK_DT(tag, min, max)  if ((dt->tag < min) || (dt->tag > max)) return false

class DS3231 {
public: 
    DS3231(i2c_inst_t* i2c) : i2c(i2c) {};
    int32_t getDatetime(datetime_t *dt);
    int32_t setDatetime(datetime_t *dt);

private:
    i2c_inst_t* i2c;

    // Returns number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged,
    // no device present, or PICO_ERROR_TIMEOUT if a timeout occurred. 
    int32_t readRegister(uint8_t reg, uint8_t* pBuf, uint32_t len);
    int32_t writeRegister(uint8_t reg, uint8_t* pBuf, uint32_t len);
};

bool checkDatetime(datetime_t *dt);

// Calculates the day of the week Sunday = 0
uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day);

#endif
