#include "bmp280.h"
#include "bmp280_internal.h"
#include "logger.h"
#include "err_controller.h"
#include "errors_list.h"
#include "memory_utils.h"
#include "i2c_controller.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

/**
 * @brief BMP280 device type.
*/
enum bmp_device_type_t {
    BMP_BMP280 = 1,
    BMP_BME280,
};

/**
 * @brief BMP280 device object.
*/
struct bmp_handle_obj {
    uint8_t addr;                               ///< BMP280 device address.
    uint8_t id;                                 ///< BMP280 chip ID.
    bmp_mode_t mode;                            ///< BMP280 device mode.
    i2c_c_device_handle_t i2c_handle;           ///< I2C bus handle.
    /** BMP calibration information */
    struct {
        uint16_t T1;                            ///< Register address 0x88 / 0x89
        int16_t T2;                             ///< Register address 0x8A / 0x8B
        int16_t T3;                             ///< Register address 0x8C / 0x8D
        uint16_t P1;                            ///< Register address 0x8E / 0x8F
        int16_t P2;                             ///< Register address 0x90 / 0x91
        int16_t P3;                             ///< Register address 0x92 / 0x93
        int16_t P4;                             ///< Register address 0x94 / 0x95
        int16_t P5;                             ///< Register address 0x96 / 0x97
        int16_t P6;                             ///< Register address 0x98 / 0x99
        int16_t P7;                             ///< Register address 0x9A / 0x9B
        int16_t P8;                             ///< Register address 0x9C / 0x9D
        int16_t P9;                             ///< Register address 0x9E / 0x9F
    } cmps;
    uint32_t t_fine;
};

uint8_t bmp_get_i2c_addr(bmp_handle_t bmp) {
    return bmp->addr;
}

uint8_t bmp_get_chip_id(bmp_handle_t bmp) {
    return bmp->id;
}

uint8_t bmp_is_measuring(uint8_t status) {
    return ((BIT3 & status) == BIT3) ? 1 : 0;
}

uint8_t bmp_is_image_updating(uint8_t status) {
    return ((BIT0 & status) == BIT0) ? 1 : 0;
}

bmp_mode_t bmp_get_mode(bmp_handle_t bmp) {
    return bmp->mode;
}

int bmp_i2c_get_config(bmp_handle_t bmp, uint8_t* out_config) {
    int err = 0;

    ERR_C_CHECK_NULL_PTR(bmp, LOG_ERROR("BMP280 device handle cannot be NULL"));

    err = bmp_i2c_read(bmp, BMP_REG_CONFIG, out_config, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading config register of the BMP280 device: %s", err, esp_err_to_name(err));
        return err;
    }
    LOG_DEBUG("readed config register value: %#X", (*out_config));

    return err;    
}

int bmp_i2c_get_ctrl_meas(bmp_handle_t bmp, uint8_t* out_ctrl) {
    int err = 0;

    ERR_C_CHECK_NULL_PTR(bmp, LOG_ERROR("BMP280 device handle cannot be NULL"));

    err = bmp_i2c_read(bmp, BMP_REG_CTRL_MEAS, out_ctrl, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading data acquisition options of the BMP280 device: %s", err, esp_err_to_name(err));
        return err;
    }
    LOG_DEBUG("readed ctr_meas register value: %#X", (*out_ctrl));

    return err;    
}

int bmp_i2c_get_status(bmp_handle_t bmp, uint8_t* out_status) {
    int err = 0;

    ERR_C_CHECK_NULL_PTR(bmp, LOG_ERROR("BMP280 device handle cannot be NULL"));
    err = bmp_i2c_read(bmp, BMP_REG_CONFIG, out_status, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading BMP280 status register: %s", err, esp_err_to_name(err));
        return err;
    }

    LOG_DEBUG("sucessfully read BMP280 status register, value: %#X", (*out_status));
    return err;
}

int bmp_i2c_reset_chip(bmp_handle_t bmp) {
    int err = 0;
    uint8_t in_buf = 0;
    ERR_C_CHECK_NULL_PTR(bmp, LOG_ERROR("BMP280 device handle cannot be NULL"));

    in_buf = BMP_RESET_VECTOR;
    err = bmp_i2c_write(bmp, BMP_REG_RESET, &in_buf, sizeof(uint8_t));
    
    if(err != 0) {
        LOG_ERROR("error %d, cannot reset BMP280 chip: %s", err, esp_err_to_name(err));
        return err;
    }
    //delay for time needed for startup
    vTaskDelay(pdMS_TO_TICKS(BMP_STARTUP_TIME));     //https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BMP280-reset-timing/td-p/48401
    LOG_DEBUG("BMP280 chip was reset");
    return err;
}

int bmp_i2c_set_mode(bmp_handle_t bmp, bmp_mode_t mode) {
    err_c_t err = 0;
    uint8_t reg_val = 0;

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));
    //get current register value
    err = bmp_i2c_get_ctrl_meas(bmp, &reg_val);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when reading BMP device ctrl_meas register.", err);
        return err;
    }
    //update only mode, write value back to register
    reg_val = (reg_val & (~3)) | mode;
    err = bmp_i2c_write(bmp, BMP_REG_CTRL_MEAS, &reg_val, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d when writing to BMP device ctrl_meas register: %s", err, esp_err_to_name(err));
        return err;
    }
    bmp->mode = mode;
    LOG_DEBUG("BMP280 device mode was set to: %X", mode);
    return err;
}

int bmp_i2c_set_standby_time(bmp_handle_t bmp, bmp_stdby_time_t time) {
    err_c_t err = 0;
    uint8_t reg_value = 0;
    uint8_t mask = 0xe0;            // 1 1 1 0 0 0 0 0

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));
    err = bmp_i2c_get_config(bmp, &reg_value);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading config register, cannot change standby time.");
        return err;
    }

    reg_value = (reg_value & (~mask)) | time;
    err = bmp_i2c_write(bmp, BMP_REG_CONFIG, &reg_value, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during writing to config register, cannot change standby time.");
        return err;
    }

    LOG_DEBUG("config register value changed to: %u", reg_value);
    return err;
}

int bmp_i2c_set_press_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling) {
    err_c_t err = 0;
    uint8_t reg_val = 0;
    uint8_t mask = 0x1c;        // 0 0 0 1 1 1 0 0

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));

    err = bmp_i2c_get_ctrl_meas(bmp, &reg_val);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading ctrl_meas register, cannot change pressure oversampling.");
        return err;
    }

    reg_val = (reg_val & (~mask)) | (oversampling << 2);    //BIT MAGIC
    err = bmp_i2c_write(bmp, BMP_REG_CTRL_MEAS, &reg_val, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during writing to ctrl_meas register, cannot change oversampling.");
        return err;
    }

    LOG_DEBUG("BMP280 device ctrl_meas register was set to: %#X", reg_val);
    return err;
}

int bmp_i2c_set_temp_oversampling(bmp_handle_t bmp, bmp_oversampling_t oversampling) {
    err_c_t err = 0;
    uint8_t reg_val = 0;
    uint8_t mask = 0xe1;        // 0 0 0 1 1 1 0 0

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));

    err = bmp_i2c_get_ctrl_meas(bmp, &reg_val);
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during reading ctrl_meas register, cannot change temperature oversampling.");
        return err;
    }

    reg_val = (reg_val & (~mask)) | (oversampling << 5);    //BIT MAGIC
    err = bmp_i2c_write(bmp, BMP_REG_CTRL_MEAS, &reg_val, sizeof(uint8_t));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d during writing to ctrl_meas register, cannot change oversampling.");
        return err;
    }

    LOG_DEBUG("BMP280 device ctrl_meas register was set to: %#X", reg_val);
    return err;
}

int bmp_i2c_set_iir_filter(bmp_handle_t bmp, bmp_iir_filter_t filter) {
    volatile err_c_t err = 0;

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));

    Try {
        uint8_t config_reg = 0;
        uint8_t mask = 0x1c;        // 0 0 0 1 1 1 0 0
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_get_config(bmp, &config_reg));

        config_reg = (config_reg & (~mask)) | (filter << 5);
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_write(bmp, BMP_REG_CONFIG, &config_reg, sizeof(uint8_t)));
        LOG_DEBUG("config register value of BMP280 device changed to: %#x", config_reg);

    } Catch(err) {
        LOG_ERROR("error %d during setting IIR filter: %s", err, esp_err_to_name(err));
    }
    return err;
}

int bmp_i2c_calibrate(bmp_handle_t bmp) {
    err_c_t err = 0;
    uint8_t buf[26];
    memutil_zero_memory(&buf, sizeof(buf));

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));
    
    LOG_DEBUG("reading BMP280 out calibration values...");
    err = bmp_i2c_read(bmp, BMP_REG_CAL_LOW, buf, sizeof(buf));
    if(err != ERR_C_OK) {
        LOG_ERROR("error %d while reading low bank calibration values of BMP280 device: %s", err, esp_err_to_name(err));
        return err;
    }

    bmp->cmps.T1 = buf[0] | (buf[1] << 8);
    bmp->cmps.T2 = buf[2] | (buf[3] << 8);
    bmp->cmps.T3 = buf[4] | (buf[5] << 8);
    bmp->cmps.P1 = buf[6] | (buf[7] << 8);
    bmp->cmps.P2 = buf[8] | (buf[9] << 8);
    bmp->cmps.P3 = buf[10] | (buf[11] << 8);
    bmp->cmps.P4 = buf[12] | (buf[13] << 8);
    bmp->cmps.P5 = buf[14] | (buf[15] << 8);
    bmp->cmps.P6 = buf[16] | (buf[17] << 8);
    bmp->cmps.P7 = buf[18] | (buf[19] << 8);
    bmp->cmps.P8 = buf[20] | (buf[21] << 8);
    bmp->cmps.P9 = buf[22] | (buf[23] << 8);

    LOG_DEBUG("compensation values of BMP280 device updated:\n\tT1: %#x\n\tT2: %#x\n\tT3: %#x\n\tP1: %#x\n\tP2: %#x\n\tP3: %#x\n\tP4: %#x\n\tP5: %#x\n\tP6: %#x\n\tP7: %#x\n\tP8: %#x\n\tP9: %#x ");
    return err;
}

int bmp_i2c_identify_chip(bmp_handle_t bmp) {
    err_c_t err = 0;
    uint8_t id = 0;

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));

    //Try high address 0x77
    LOG_DEBUG("Trying to find BMP280 device on bus with 0x77 address...");
    bmp->addr = BMP_I2C_ADDR_HI;
    i2c_c_set_device_addr(bmp->i2c_handle, BMP_I2C_ADDR_HI);
    err = bmp_i2c_read(bmp, BMP_REG_CHIP_ID, &id, sizeof(uint8_t));
    if(err == ERR_C_OK && ((id == BMP_CHIP_ID_FIRST) || (id == BMP_CHIP_ID_SECOND) || (id == BMP_CHIP_ID_THIRD))) {
        // Ha! Found you!
        LOG_DEBUG("found BMP280 device on bus with address: %#X and ID: %#X", bmp->addr, bmp->id);
        bmp->id = id;
        assert(bmp->addr == BMP_I2C_ADDR_HI);
        return err;
    }

    //Try low address 0x76
    LOG_DEBUG("BMP280 device with 0x77 address not found on bus, trying 0x76 address...");
    bmp->addr = BMP_I2C_ADDR_LO;
    i2c_c_set_device_addr(bmp->i2c_handle, BMP_I2C_ADDR_LO);
    err = bmp_i2c_read(bmp, BMP_REG_CHIP_ID, &id, sizeof(uint8_t));
    if(err == ERR_C_OK && ((id == BMP_CHIP_ID_FIRST) || (id == BMP_CHIP_ID_SECOND) || (id == BMP_CHIP_ID_THIRD))) {
        // Ha! Found you!
        LOG_DEBUG("found BMP280 device on bus with address: %#X and ID: %#X", bmp->addr, bmp->id);
        bmp->id = id;
        assert(bmp->addr == BMP_I2C_ADDR_LO);
        return err;
    }

    LOG_WARN("no BMP280 device found on bus!");
    bmp->addr = 0;
    i2c_c_set_device_addr(bmp->i2c_handle, 0);
    bmp->id = 0;
    return BMP_ERR_NO_DEVICE_FOUND;
}

int bmp_i2c_readout(bmp_handle_t bmp, int32_t* temperature, uint32_t* pressure) {
    volatile err_c_t err = 0;

    CHECK_NULL_PTR(bmp, LOG_ERROR("bmp device handle cannot be NULL"));
    CHECK_NULL_PTR(temperature, LOG_ERROR("value where temperature is stored cannot be NULL"));
    CHECK_NULL_PTR(pressure, LOG_ERROR("value where pressure is stored cannot be NULL"));

    Try {
        uint8_t buffer[3];
        memutil_zero_memory(&buffer, sizeof(buffer));
        
        LOG_DEBUG("reading temperature of BMP280 device...");
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_read(bmp, BMP_REG_TEMP_MSB, &(buffer[0]), sizeof(buffer)));
        *temperature = bmp_compensate_T_int32(bmp, (buffer[0] << 12) | (buffer[1] << 4) | (buffer[0] >> 4));

        LOG_DEBUG("reading pressure of BMP280 device...");
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_read(bmp, BMP_REG_PRESS_MSB, &(buffer[0]), sizeof(buffer)));
        *pressure = bmp_compensate_P_int64(bmp, (buffer[0] << 12) | (buffer[1] << 4) | (buffer[0] >> 4));
    } Catch(err) {
        LOG_ERROR("error %d during reading results from BMP280 device: %s", err, esp_err_to_name(err));
    }
    return err;
}

int bmp_i2c_read(bmp_handle_t bmp, uint8_t addr, uint8_t* out_buffer, size_t buflen) {
    err_c_t err = 0;
    err = i2c_c_read(bmp->i2c_handle, addr, out_buffer, buflen, 500);
    return err;
}

int bmp_i2c_write(bmp_handle_t bmp, uint8_t addr, uint8_t* in_buffer, size_t buflen) {
    err_c_t err = 0;
    err = i2c_c_write(bmp->i2c_handle, addr, in_buffer, buflen, 500);
    return err;
}

// HERE BE DRAGONS
// This code is revised from the Bosch code within the datasheet of the BME280.
// I do not understand it enough to tell you what it does.
// No touchies.


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t bmp_compensate_T_int32(bmp_handle_t bmp, int32_t adc_T)
{
    int32_t var1, var2, T; 
    var1 = ((((adc_T>>3) -((int32_t)bmp->cmps.T1<<1))) * ((int32_t)bmp->cmps.T2)) >> 11;
    var2  =(((((adc_T>>4) -((int32_t)bmp->cmps.T1)) * ((adc_T>>4) -((int32_t)bmp->cmps.T1))) >> 12) * ((int32_t)bmp->cmps.T3)) >> 14;
    bmp->t_fine = var1 + var2;
    T  = (bmp->t_fine * 5 + 128) >> 8;
    return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t bmp_compensate_P_int64(bmp_handle_t bmp, int32_t adc_P)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)bmp->t_fine) -128000;
    var2 = var1 * var1 * (int64_t)bmp->cmps.P6;
    var2 = var2 + ((var1*(int64_t)bmp->cmps.P5)<<17);
    var2 = var2 + (((int64_t)bmp->cmps.P4)<<35);
    var1 = ((var1 * var1 * (int64_t)bmp->cmps.P3)>>8) + ((var1 * (int64_t)bmp->cmps.P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bmp->cmps.P1)>>33;
    if(var1 == 0){ 
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)bmp->cmps.P9) * (p>>13) * (p>>13)) >> 25;
    var2 =(((int64_t)bmp->cmps.P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp->cmps.P7)<<4);
    return (uint32_t)p;
}

//END OF DRAGONS

int bmp_configure(bmp_handle_t bmp, bmp_config_t* config) {
    volatile err_c_t err = 0;

    ERR_C_CHECK_NULL_PTR(bmp, LOG_ERROR("BMP device handle cannot be NULL"));
    ERR_C_CHECK_NULL_PTR(config, LOG_ERROR("BMP device config cannot be NULL"));

    Try {
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_set_standby_time(bmp, config->standby));
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_set_iir_filter(bmp, config->iir_filter));
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_set_press_oversampling(bmp, config->press_over));
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_set_temp_oversampling(bmp, config->temp_over));
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_set_mode(bmp, config->mode));
        bmp->mode = config->mode;
    } Catch(err) {
        LOG_ERROR("error %d while configuring BMP device: %s", err, esp_err_to_name(err));
    }
    return err;
}

int bmp_init(bmp_handle_t* out_handle, i2c_c_bus_handle_t bus) {
    volatile err_c_t err = 0;
    CHECK_NULL_PTR(out_handle, LOG_ERROR("BMP handle cannot be NULL"));
    CHECK_NULL_PTR(bus, LOG_ERROR("I2C bus handle cannot be NULL"));

    Try {
        uint8_t reg_val = 0;
        
        LOG_INFO("preparing new BMP280 device...");
        NEW((*out_handle), struct bmp_handle_obj);
        (*out_handle)->addr = 0;
        (*out_handle)->id = 0;
        (*out_handle)->mode = 0xff;

        i2c_c_add_device(0, bus, &((*out_handle)->i2c_handle));         //use mock addr, changed during bmp_i2c_identify_chip

        //set ID and ADDRESS
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_identify_chip((*out_handle)));

        //set MODE
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_get_ctrl_meas((*out_handle), &reg_val));
        (*out_handle)->mode = (reg_val & 3);    //https://bitwisecmd.com/#255ub%2C%26%2C3ub

        //calibrate
        ERR_C_CHECK_AND_THROW_ERR(bmp_i2c_calibrate((*out_handle)));
        LOG_INFO("new BMP280 device ready to use!");
    } Catch(err) {
        switch (err)
        {
        case BMP_ERR_NO_DEVICE_FOUND:
            LOG_ERROR("no BMP280 device was found");
            DELETE((*out_handle));      //free resources
            break;
        case BMP_ERR_MODE_ERROR:
            LOG_ERROR("cannot update device mode, exiting...");
            (*out_handle)->mode = 0xff;
            break;
        default:
            LOG_ERROR("error %d during bmp initialisation: %s", esp_err_to_name(err));
            break;
        }
    }
    return err;
}