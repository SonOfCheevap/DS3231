#include "DS3231.h"

// convert BCD to number
static inline uint8_t bcdnum(uint8_t bcd) {
    return ((bcd/16) * 10) + (bcd % 16);
}

// convert number to BCD
static inline uint8_t numbcd(uint8_t num) {
    return ((num/10) * 16) + (num % 10);
}

bool checkDatetime(datetime_t *dt) {
    CHECK_DT(year, 0, 4095);
    CHECK_DT(month, 1, 12);
    CHECK_DT(day, 1, 31);
    CHECK_DT(dotw, 0, 6);
    CHECK_DT(hour, 0, 23);
    CHECK_DT(min, 0, 59);
    CHECK_DT(sec, 0, 59);

    return true;
}

int32_t DS3231::getDatetime(datetime_t *dt) {
    uint8_t buf[7];

    int err = readRegister(0, buf, sizeof(buf));
    if(err != sizeof(buf))
        return err;

    dt->sec = bcdnum(buf[0]);
    dt->min = bcdnum(buf[1]);
    dt->hour = bcdnum(buf[2]);
    dt->dotw = bcdnum(buf[3]) - 1;
    dt->day = bcdnum(buf[4]);
    dt->month = bcdnum(buf[5] & 0x1F);
    dt->year = bcdnum(buf[6]) + 2000;

    if(!checkDatetime(dt))
        return -101;
    return 0;
}

int32_t DS3231::setDatetime(datetime_t *dt) {
    if(!checkDatetime(dt))
        return -101;

    uint8_t buf[8];
    buf[0] = 0;
    buf[1] = numbcd(dt->sec);
    buf[2] = numbcd(dt->min);
    buf[3] = numbcd(dt->hour);
    buf[4] = numbcd(dt->dotw + 1);
    buf[5] = numbcd(dt->day);
    buf[6] = numbcd(dt->month);
    buf[7] = numbcd(dt->year - 2000);

    int err = writeRegister(0, buf, sizeof(buf)) != sizeof(buf);
    if(err != sizeof(buf))
        return err;
    return 0;
}

// returns Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged, no device present, or PICO_ERROR_TIMEOUT if a timeout occurred. 
int32_t DS3231::readRegister(uint8_t reg, uint8_t* pBuf, uint32_t len) {
    // Select the register we want to read from
    int32_t err = i2c_write_timeout_us(i2c, DS3231_ADDR, &reg, I2C_REG_READ, true, I2C_TIMEOUT_CHAR);
    // Read into buffer
    if(err == 1)
        err = i2c_read_timeout_us(i2c, DS3231_ADDR, pBuf, len, false, len * I2C_TIMEOUT_CHAR);
    return err;
}

// returns Number of bytes written, or PICO_ERROR_GENERIC if address not acknowledged, no device present, or PICO_ERROR_TIMEOUT if a timeout occurred. 
int32_t DS3231::writeRegister(uint8_t reg, uint8_t* pBuf, uint32_t len) {
    // Select the register we want to read from
    int32_t err = i2c_write_timeout_us(i2c, DS3231_ADDR, &reg, I2C_REG_WRITE, true, I2C_TIMEOUT_CHAR);
    if(err == 0)
        err = i2c_write_timeout_us(i2c, DS3231_ADDR, pBuf, len, false, len * I2C_TIMEOUT_CHAR);
    return err;
}

uint8_t getDayOfWeek(uint16_t year, uint8_t month, uint8_t day) {
    // adjust month year
    if (month < 3) {
        month += 12;
        year -= 1;
    }

    // split year
    uint32_t c = year / 100;
    year = year % 100;

    // Zeller's congruence
    return (c / 4 - 2 * c + year + year / 4 + 13 * (month + 1) / 5 + day - 1) % 7;
}
