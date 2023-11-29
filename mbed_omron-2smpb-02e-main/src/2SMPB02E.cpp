/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#include "2SMPB02E/2SMPB02E.h"

namespace sixtron {

namespace {
#define SUCCESS 0
#define FAILURE -1

#define SOFTRESET_CMD 0xE6
#define HW_CHIP_ID 0x5C

#define U16TOS16(x) (-(x & 0x8000) | (x & 0x7FFF))
#define U20TOS32(x) (-(x & 0x00080000) + (x & 0xFFF7FFFF))
}

O2SMPB02E::O2SMPB02E(PinName i2c_sda, PinName i2c_scl, I2CAddress i2c_address):
        _i2c(i2c_sda, i2c_scl), _i2c_address(i2c_address)
{
}

bool O2SMPB02E::init()
{
    reset();
    ThisThread::sleep_for(10ms);
    if (read_chip_id() != SUCCESS) {
        return false;
    }

    read_coefficient();

    i2c_write_register(RegisterAddress::IIR_CNT, 0x03);

    return true;
}

int O2SMPB02E::read_chip_id()
{
    char chip_id = 0;
    if (i2c_read_register(RegisterAddress::CHIP_ID, &chip_id, 1) != SUCCESS) {
        return FAILURE;
    }
    return (chip_id == HW_CHIP_ID) ? SUCCESS : FAILURE;
}

int O2SMPB02E::reset()
{
    if (i2c_write_register(RegisterAddress::RESET, SOFTRESET_CMD) != SUCCESS) {
        return FAILURE;
    }
    return SUCCESS;
}

int O2SMPB02E::start_periodic_measurement()
{
    if (i2c_write_register(RegisterAddress::CTRL_MEAS,
                0x101 << 5 | 0x101 << 2 | static_cast<char>(PowerMode::NORMAL))
            != SUCCESS) {
        return FAILURE;
    }

    return FAILURE;
}

int O2SMPB02E::measure_single_shot()
{
    if (i2c_write_register(RegisterAddress::CTRL_MEAS,
                0x101 << 5 | 0x101 << 2 | static_cast<char>(PowerMode::FORCED))
            != SUCCESS) {
        return FAILURE;
    }

    return FAILURE;
}

double O2SMPB02E::temperature()
{
    char data[3];
    if (i2c_read_register(RegisterAddress::TEMP_TXD2, data, 3) != SUCCESS) {
        return FAILURE;
    }

    int32_t dt = (data[0] << 16 | data[1] << 8 | data[2]) - ((uint32_t)1 << 23);

    float tr = _coef.a0 + (_coef.a1 + _coef.a2 * dt) * dt;

    return tr;
}

float O2SMPB02E::pressure()
{
    double tr = temperature();

    char data[3];
    if (i2c_read_register(RegisterAddress::PRESS_TXD2, data, 3) != SUCCESS) {
        return FAILURE;
    }

    int32_t dp = (data[0] << 16 | data[1] << 8 | data[2]) - ((uint32_t)1 << 23);

    double pressure = _coef.b00 + _coef.bt1 * tr + _coef.bp1 * (float)dp + _coef.b11 * dp * tr
            + _coef.bt2 * tr * tr + _coef.bp2 * dp * dp + _coef.b12 * dp * tr * tr
            + _coef.b21 * dp * dp * tr + _coef.bp3 * dp * dp * dp;

    return pressure;
}

void O2SMPB02E::read_coefficient()
{
    char data[25];
    i2c_read_register(RegisterAddress::COE_b00_1, data, 25);

    _coef.a0 = U20TOS32((data[18] << 12 | data[19] << 4 | (data[24] & 0x0F)));
    _coef.a0 /= 16;

    int16_t a1 = U16TOS16((data[20] << 8 | data[21]));
    _coef.a1 = (-6.3E-03 + (((4.3E-04 * (float)a1) / 32767.0)));

    int16_t a2 = U16TOS16((data[22] << 8 | data[23]));
    _coef.a2 = (-1.9E-11 + (((1.2E-10 * (float)a2) / 32767.0)));

    _coef.b00 = U20TOS32((data[0] << 12 | data[1] << 4 | (data[24] & 0xF0) >> 4));
    _coef.b00 /= 16;

    int16_t bt1 = U16TOS16((data[2] << 8 | data[3]));
    _coef.bt1 = (1.0E-01 + (((9.1E-02 * (float)bt1) / 32767.0)));

    int16_t bt2 = U16TOS16((data[4] << 8 | data[5]));
    _coef.bt2 = (1.2E-08 + (((1.2E-06 * (float)bt2) / 32767.0)));

    int16_t bp1 = U16TOS16((data[6] << 8 | data[7]));
    _coef.bp1 = (3.3E-02 + (((1.9E-02 * (float)bp1) / 32767.0)));

    int16_t b11 = U16TOS16((data[8] << 8 | data[9]));
    _coef.b11 = (2.1E-07 + (((1.4E-07 * (float)b11) / 32767.0)));

    int16_t bp2 = U16TOS16((data[10] << 8 | data[11]));
    _coef.bp2 = (-6.3E-10 + (((3.5E-10 * (float)bp2) / 32767.0)));

    int16_t b12 = U16TOS16((data[12] << 8 | data[13]));
    _coef.b12 = (2.9E-13 + (((7.6E-13 * (float)b12) / 32767.0)));

    int16_t b21 = U16TOS16((data[14] << 8 | data[15]));
    _coef.b21 = (2.1E-15 + (((1.2E-14 * (float)b21) / 32767.0)));

    int16_t bp3 = U16TOS16((data[16] << 8 | data[17]));
    _coef.bp3 = (1.3E-16 + (((7.9E-17 * (float)bp3) / 32767.0)));
}

int O2SMPB02E::i2c_read_register(RegisterAddress register_address, char *value, uint8_t len)
{
    static char data;
    data = static_cast<char>(register_address);
    if (_i2c.write(static_cast<int>(_i2c_address) << 1, &data, 1) != SUCCESS) {
        return FAILURE;
    }
    char *char_value = reinterpret_cast<char *>(value);
    if (_i2c.read(static_cast<int>(_i2c_address) << 1, char_value, len) != SUCCESS) {
        return FAILURE;
    }

    return SUCCESS;
}

int O2SMPB02E::i2c_write_register(RegisterAddress register_address, char value)
{
    char data[2];
    data[0] = static_cast<char>(register_address);
    data[1] = value;
    if (_i2c.write(static_cast<int>(_i2c_address) << 1, data, 2)) {
        return FAILURE;
    }
    return SUCCESS;
}

} // namespace sixtron
