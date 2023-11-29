/*
 * Copyright (c) 2022, CATIE
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_2SMPB02E_H_
#define CATIE_SIXTRON_2SMPB02E_H_

#include "mbed.h"

namespace sixtron {

class O2SMPB02E {
public:
    enum class I2CAddress : char {
        Address1 = 0x56,
        Address2 = 0x70,
    };

    /*!
     * \brief Chip registers
     */
    enum class RegisterAddress : char {
        TEMP_TXD0 = 0xFC,
        TEMP_TXD1 = 0xFB,
        TEMP_TXD2 = 0xFA,
        PRESS_TXD0 = 0xF9,
        PRESS_TXD1 = 0xF8,
        PRESS_TXD2 = 0xF7,
        IO_SETUP = 0xF5,
        CTRL_MEAS = 0xF4,
        DEVICE_STAT = 0xF3,
        I2C_SET = 0xF2,
        IIR_CNT = 0xF1,
        RESET = 0xE0,
        CHIP_ID = 0xD1,
        COE_b00_a0_ex = 0xB8,
        COE_a2_0 = 0xB7,
        COE_a2_1 = 0xB6,
        COE_a1_0 = 0xB5,
        COE_a1_1 = 0xB4,
        COE_a0_0 = 0xB3,
        COE_a0_1 = 0xB2,
        COE_bp3_0 = 0xB1,
        COE_bp3_1 = 0xB0,
        COE_b21_0 = 0xAF,
        COE_b21_1 = 0xAE,
        COE_b12_0 = 0xAD,
        COE_b12_1 = 0xAC,
        COE_bp2_0 = 0xAB,
        COE_bp2_1 = 0xAA,
        COE_b11_0 = 0xA9,
        COE_b11_1 = 0xA8,
        COE_bp1_0 = 0xA7,
        COE_bp1_1 = 0xA6,
        COE_bt2_0 = 0xA5,
        COE_bt2_1 = 0xA4,
        COE_bt1_0 = 0xA3,
        COE_bt1_1 = 0xA2,
        COE_b00_0 = 0xA1,
        COE_b00_1 = 0xA0,
    };

    /*!
     * \brief Power mode
     */
    enum class PowerMode : char {
        SLEEP = 0x00,
        FORCED = 0x01,
        NORMAL = 0x03,
    };

    O2SMPB02E(PinName i2c_sda, PinName i2c_scl, I2CAddress i2c_address = I2CAddress::Address2);

    bool init();

    int measure_single_shot();

    int start_periodic_measurement();

    double temperature();

    float pressure();

private:
    I2CAddress _i2c_address;
    I2C _i2c;

    int read_chip_id();

    int reset();

    void read_coefficient();

    typedef struct {
        /** otp data by multi bytes */
        int32_t b00, a0;
        float bt1, bp1;
        float bt2;
        float b11, bp2;
        float b12, b21, bp3;
        float a1, a2;
    } _2smpb02e_t;

    _2smpb02e_t _coef;
    /*!
     *  \brief Read register data
     *
     *  \param register_address Address of the register
     *  \param value Pointer to the value read from the register
     *  \return 0 on success, 1 on failure
     */
    int i2c_read_register(RegisterAddress register_address, char *value, uint8_t len);

    /*!
     *  \brief Write to a register
     *
     *  \param register_address Address of the register to write to
     *  \param value Data to store in the register
     *  \return 0 on success, 1 on failure
     */
    int i2c_write_register(RegisterAddress register_address, char value);
};

} // namespace sixtron

#endif // CATIE_SIXTRON_2SMPB02E_H_
