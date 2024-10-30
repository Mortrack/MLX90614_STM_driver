/** @addtogroup mlx90614
 * @{
 */

#include "mlx90614_ir_thermometer_driver.h"

#define MLX90614_RAM_OR_EEPROM_ADDRESS_SIZE			            (1)		/**< @brief	Size in bytes of any single RAM or EEPROM address that the manufacturer has implemented in the MLX90614 Infra Red Thermometer. */
#define MLX90614_TA_RAM_ADDRESS			                        (0x06)	/**< @brief	RAM address that the manufacturer of the MLX90614 Infra Red Thermometer has designated for calling the \f$T_{A}\f$ Command. */
#define MLX90614_TOBJ1_RAM_ADDRESS			                    (0x07)	/**< @brief	RAM address that the manufacturer of the MLX90614 Infra Red Thermometer has designated for calling the \f$T_{OBJ1}\f$ Command. */
#define MLX90614_TOBJ2_RAM_ADDRESS			                    (0x08)	/**< @brief	RAM address that the manufacturer of the MLX90614 Infra Red Thermometer has designated for calling the \f$T_{OBJ2}\f$ Command. */
#define MLX90614_TEMPERATURE_RESULT_SIZE	                    (2)		/**< @brief	Temperature data size in bytes from a single temperature reading that the MLX90614 Infra Red Thermometer can do. */
#define MLX90614_EEPROM_SLAVE_ADDRESS_SIZE                      (2)     /**< @brief MLX90614's Slave Address EEPROM value size in bytes, where the first byte (i.e., the LSB) is where the actual Slave Address is located at and where the second byte (i.e., the MSB) contains unknown data. @note I could not find anywhere in the documentation what the most significant byte stands for, but it is required in the process of changing the Slave Address in the EEPROM of the MLX90614 Device according to the <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_cmd.cpp#L456-L512>code provided to me by the Melexis team</a> via email after requesting them for help in knowing how to change the slave address of a MLX90614 Device. */
#define MLX90614_I2C_WRITE_COMMAND_SIZE                         (4)     /**< @brief MLX90614's I2C Write command size. */
#define MLX90614_SLAVE_ADDRESS_EEPROM_ADDRESS                   (0x2E)  /**< @brief	EEPROM address that the MLX90614 Infra Red Thermometer has designated for storing its designated Slave Address to which it will respond via the I2C Protocol. @note <i><b style="color:orange;"><u>IMPORTANT-INFORMATION</u>:</b><b>The actual MLX90614 datasheet does not mention what is the EEPROM address value of the MLX90614 Slave Device and the nearest thing it states is what they defined/called as "SMBus address" whose EEPROM Address value is \c 0x0E , but where it seems that, according to several statements of the community, this address value does not change the MLX90614 Device's Slave Address to which it will respond in a I2C Communication. However, I found a statement in a <a href=https://forum.mikroe.com/viewtopic.php?t=67842&fbclid=IwZXh0bgNhZW0CMTAAAR201BxXMK0Zf8QaL2r8vzwVFsQCKXd4v_XcA3elM7qyCQ_S7QM_SOqympc_aem_GqwHbzcMEAGRhF8-MJe3OA>Mikroe Forum made by mynos</a>, where he assures that he was able to change the MLX90614 Device's Slave Address with a 4 steps process where the EEPROM Address he wrote instead was actually \c 0x2E . However, the author of @ref mlx90614 was still not able to change his MLX90614 Device's slave address even with this 4-step method, but decided to leave it in case it works for some people since some people over the internet have stated that this worked for them.</b></i> */
#define MLX90614_PEC_RESET_VALUE                   				(0x00)  /**< @brief	Value with which a new PEC byte to calculate should be defined/started with in order have a correct calculation by the @ref calculate_pec function. */
#define MLX90614_SLAVE_ADDRESS_EEPROM_ERASE_VALUE               (0x00)  /**< @brief	Value used to erase the currently configured Slave Address in the MLX90614 EEPROM. @note This value is not mentioned anywhere in the MLX90614 Datasheet; I was able to determine it via a <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_cmd.cpp#L456-L512>code provided to me by the Melexis team</a> via email after requesting them for help in knowing how to change the slave address of a MLX90614 Device. */
#define MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE                  (0X7E)  /**< @brief	Maximum valid slave address value that can be assigned to the MLX90614 Device. @note I got this value from a <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_cmd.cpp#L456-L512>code provided to me by the Melexis team</a> via email after requesting them for help in knowing how to change the slave address of a MLX90614 Device. */
#define MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE_PLUS_ONE			(0X7F)  /**< @brief	Maximum valid slave address value, plus one, that can be assigned to the MLX90614 Device. @note I got this value from a <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_cmd.cpp#L456-L512>code provided to me by the Melexis team</a> via email after requesting them for help in knowing how to change the slave address of a MLX90614 Device. */
#define MLX90614_MIN_VALID_SLAVE_ADDRESS_VALUE                  (0X03)  /**< @brief	Minimum valid slave address value that can be assigned to the MLX90614 Device. @note I got this value from a <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_cmd.cpp#L456-L512>code provided to me by the Melexis team</a> via email after requesting them for help in knowing how to change the slave address of a MLX90614 Device. */

static I2C_HandleTypeDef *p_hi2c;                                                            /**< @brief Pointer to the I2C Handle Structure of the I2C that will be used in this @ref mlx90614 to send/receive instructions and/or data to the MLX90614 Infra Red Thermometer Module. @note This pointer's value is defined in the @ref init_mlx90614_module function. */
static uint8_t mlx90614_slave_address = 0x5A;					                             /**< @brief Slave address of the MLX90614 Infra Red Thermometer device. @note The default slave address device of this Thermometer is 0x5A according to its datasheet. @note The value of this Global Variable is defined in the @ref init_mlx90614_module function but can be customized/changed in the @ref set_mlx90614_module_slave_address function. */
static uint8_t mlx90614_slave_address_one_bit_left_shifted;                                  /**< @brief Slave address of a MLX90614 Infra Red Thermometer device, but shifted to the left by one bit. @note This Global Variable is updated each time the slave address of this device changes via the @ref set_mlx90614_module_slave_address function. */
static MLX90614_Temp_t mlx90614_temperature_type;                                            /**< @brief Temperature Type with which the @ref mlx90614 will be currently responding whenever it is requested to give a temperature value. */
static float (*p_get_mlx90614_converted_temperature)(uint16_t raw_temp);                     /**< @brief Pointer to the function that gets the corresponding current object1, object2 or ambient temperature with the latest configured Temperature Type. @note The value of this pointer is initialized with the @ref init_mlx90614_module function and can be updated with the @ref set_mlx90614_temperature_type function. @note According to the MLX90614 Infra Red Thermometer Datasheet, the formula for calculating either the Object1, Object2 or Ambient Temperatures are the same. */

/**@brief	Gets the either the Object1, Object2 or Ambient Temperature in Kelvin units with respect to a given Decimal
 *          Value standing for an Object1/Object2/Ambient Temperature Raw Value read from the MLX90614 Infra Red
 *          Thermometer Device.
 *
 * @note    According to the MLX90614 Infra Red Thermometer Datasheet, the formula for getting either the Object1,
 *          Object2 or Ambient Temperature value are the same.
 *
 * @param raw_temp  Decimal Value of an Object1/Object2/Ambient Temperature Raw Value that wants to be converted into
 *                  its corresponding Object1/Object2/Ambient Temperature in Kelvin units.
 *
 * @return  Object1/Object2/Ambient Temperature value in Kelvin units.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
static float get_mlx90614_converted_temperature_in_kelvin(uint16_t raw_temp);

/**@brief	Gets the either the Object1, Object2 or Ambient Temperature in Celsius units with respect to a given Decimal
 *          Value standing for an Object1/Object2/Ambient Temperature Raw Value read from the MLX90614 Infra Red
 *          Thermometer Device.
 *
 * @note    According to the MLX90614 Infra Red Thermometer Datasheet, the formula for getting either the Object1,
 *          Object2 or Ambient Temperature value are the same.
 *
 * @param raw_temp  Decimal Value of an Object1/Object2/Ambient Temperature Raw Value that wants to be converted into
 *                  its corresponding Object1/Object2/Ambient Temperature in Celsius units.
 *
 * @return  Object1/Object2/Ambient Temperature value in Celsius units.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
static float get_mlx90614_converted_temperature_in_celsius(uint16_t raw_temp);

/**@brief	Gets the either the Object1, Object2 or Ambient Temperature in Fahrenheit units with respect to a given
 *          Decimal Value standing for an Object1/Object2/Ambient Temperature Raw Value read from the MLX90614 Infra Red
 *          Thermometer Device.
 *
 * @note    According to the MLX90614 Infra Red Thermometer Datasheet, the formula for getting either the Object1,
 *          Object2 or Ambient Temperature value are the same.
 *
 * @param raw_temp  Decimal Value of an Object1/Object2/Ambient Temperature Raw Value that wants to be converted into
 *                  its corresponding Object1/Object2/Ambient Temperature in Fahrenheit units.
 *
 * @return  Object1/Object2/Ambient Temperature value in Fahrenheit units.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
static float get_mlx90614_converted_temperature_in_fahrenheit(uint16_t raw_temp);

/**@brief   Function that calculates the PEC byte for the I2C Transaction with an MLX90614 Device.
 *
 * @note    This
 *          <a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_smbus_driver.cpp#L181>code is a replica of the one made by Melexis</a>
 *          for calculating the PEC byte of an MLX90614 Device I2C transaction, according to the email they sent to the
 *          author of @ref mlx90614 , but where the author of this @ref mlx90614 library took the liberty of just changing
 *          some variable names so that it followed the same coding standard as used in @ref mlx90614 .
 *
 * @details The way this function calculated the MLX90614 Device's PEC byte is in an iterative way. The very first step
 *          is to define an initial PEC value (via \p init_pec ) ov \c 0 and then to make the first PEC calculation with
 *          the first or, in other words, the least significant byte of the actual data to be sent over the I2C to the
 *          MLX90614 Device (via \p init_pec ). Then, the resulting value (i.e., the returned value of the first/previous
 *          calculation) is to be placed as the now initial PEC value (via \p init_pec ) and the now \p new_data value
 *          must be the next byte of data to be sent over the I2C. This process is to be repeated until the last or
 *          most significant byte of the data to be sent to the MLX90614 Device is reached. Finally, the last resulting,
 *          i.e., the last returned value of this iterative process is the final and definitive PEC byte for the
 *          corresponding data to be sent over the I2C.
 *
 * @param init_pec   Either the initial or current value with which the PEC byte was last calculated with.
 * @param new_data   New data with which a new PEC byte value will be calculated.
 *
 * @return  The resulting PEC value calculation between the two given data values in the arguments of this function.
 *
 * @author	<a href=https://github.com/melexis/i2c-stick/blob/main/i2c-stick-arduino/mlx90614_smbus_driver.cpp#L92>Melexis</a>
 * @date    October 29, 2024 (Reference date from which the code was replicated).
 */
static uint8_t calculate_pec(uint8_t init_pec, uint8_t new_data);

/**@brief	Gets the corresponding @ref MLX90614_Status value depending on the given @ref HAL_StatusTypeDef value.
 *
 * @param HAL_status	HAL Status value (see @ref HAL_StatusTypeDef ) that wants to be converted into its equivalent
 * 						of a @ref MLX90614_Status value.
 *
 * @retval				MLX90614_EC_NR if \p HAL_status param equals \c HAL_BUSY or \c HAL_TIMEOUT .
 * @retval				MLX90614_EC_ERR if \p HAL_status param equals \c HAL_ERROR .
 * @retval				HAL_status param otherwise.
 *
 * @note	For more details on the returned values listed, see @ref MLX90614_Status and @ref HAL_StatusTypeDef .
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
static MLX90614_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status);

MLX90614_Status init_mlx90614_module(I2C_HandleTypeDef *hi2c, uint8_t slave_address, MLX90614_Temp_t temp_t)
{
    /* Validate the given slave address to have a valid value. */
    if (slave_address > MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE)
    {
        return MLX90614_EC_ERR;
    }

    /* Update and persist the slave address in the @ref mlx90614 if requested. */
    if (slave_address != 0)
    {
        /** <b>Local uint8_t variable tmp_slave_addr_one_bit_left_shifted:</b> Contains the given slave address, but with one bit left shift. */
        uint8_t tmp_slave_addr_one_bit_left_shifted = slave_address << 1;
        if (HAL_I2C_IsDeviceReady(p_hi2c, tmp_slave_addr_one_bit_left_shifted, IS_MLX90614_READY_NUMBER_OF_TRIALS, MLX90614_I2C_TIMEOUT) != HAL_OK)
        {
            return MLX90614_EC_NR;
        }
        mlx90614_slave_address = slave_address;
        mlx90614_slave_address_one_bit_left_shifted = tmp_slave_addr_one_bit_left_shifted;
    }
    else
    {
        mlx90614_slave_address_one_bit_left_shifted = mlx90614_slave_address << 1;
    }

    /* Persist the pointer to the I2C that is desired for the MLX90614 module to use. */
    p_hi2c = hi2c;

    /* Initialize the @ref p_get_mlx90614_converted_temperature pointer and update @ref mlx90614_temperature_type . */
    switch (temp_t)
    {
        case MLX90614_Temp_K:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_kelvin;
            break;
        case MLX90614_Temp_C:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_celsius;
            break;
        case MLX90614_Temp_F:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_fahrenheit;
            break;
        default:
            return MLX90614_EC_ERR; // The requested temperature value type is not recognized. Therefore, send Error Exception Code.
    }
    mlx90614_temperature_type = temp_t;

    return MLX90614_EC_OK;
}

MLX90614_Status find_mlx90614_slave_address(void)
{
    /** <b>Local uint8_t variable current_slave_address:</b> Contains the current slave address with which it is being attempted to get a response from a MLX90614 device. */
    uint8_t current_slave_address = MLX90614_MIN_VALID_SLAVE_ADDRESS_VALUE;
    /** <b>Local uint8_t variable current_slave_address_one_bit_left_shifted:</b> Contains the current slave address with which it is being attempted to get a response from a MLX90614 device, but with one bit left shift. */
    uint8_t current_slave_address_one_bit_left_shifted;

    /*
     NOTE:  We do not start from mlx90614_slave_address=0, because that value will make @ref HAL_I2C_IsDeviceReady
            function to give back @ref HAL_OK as long as a single I2C device responds to the I2C protocol on the
            specified I2C pins of the MCU via the given @ref p_hi2c Pointer, which will not allow us to identify the
            currently stored slave address of our actual I2C device.
     */
    for (; current_slave_address<MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE_PLUS_ONE; current_slave_address++)
    {
        current_slave_address_one_bit_left_shifted = current_slave_address<<1;
        if (HAL_I2C_IsDeviceReady(p_hi2c, current_slave_address_one_bit_left_shifted, IS_MLX90614_READY_NUMBER_OF_TRIALS, MLX90614_I2C_TIMEOUT) == HAL_OK)
        {
            mlx90614_slave_address = current_slave_address;
            mlx90614_slave_address_one_bit_left_shifted = current_slave_address_one_bit_left_shifted;
            return MLX90614_EC_OK;
        }
    }

    return MLX90614_EC_NR;
}

uint8_t get_mlx90614_module_slave_address(void)
{
    return mlx90614_slave_address;
}

MLX90614_Status set_mlx90614_module_slave_address(uint8_t slave_address)
{
    /* Validate the given slave address to have a valid value. */
    if ((slave_address>MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE) || (slave_address<MLX90614_MIN_VALID_SLAVE_ADDRESS_VALUE))
    {
        return MLX90614_EC_ERR;
    }

    /* Update and persist the slave address in the @ref mlx90614 . */
    /** <b>Local uint8_t variable tmp_slave_addr_one_bit_left_shifted:</b> Contains the given slave address, but with one bit left shift. */
    uint8_t tmp_slave_addr_one_bit_left_shifted = slave_address << 1;
    if (HAL_I2C_IsDeviceReady(p_hi2c, tmp_slave_addr_one_bit_left_shifted, IS_MLX90614_READY_NUMBER_OF_TRIALS, MLX90614_I2C_TIMEOUT) != HAL_OK)
    {
        return MLX90614_EC_NR;
    }
    mlx90614_slave_address = slave_address;
    mlx90614_slave_address_one_bit_left_shifted = tmp_slave_addr_one_bit_left_shifted;

    return MLX90614_EC_OK;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers" // This pragma definition will tell the compiler to ignore an expected Compilation Warning, that should not affect the program at all and that was decided to put this way for performance purposes. For the record, this Warningstates the following: passing argument 5 of 'HAL_I2C_Mem_Write' discards 'const' qualifier from pointer target type
MLX90614_Status set_mlx90614_device_slave_address(uint8_t new_slave_address)
{
    /* Validate the given slave address to have a valid value. */
    if ((new_slave_address>MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE) || (new_slave_address<MLX90614_MIN_VALID_SLAVE_ADDRESS_VALUE))
    {
        return MLX90614_EC_ERR;
    }

    /* STEP 1: Read the currently stored 2 bytes of data in the MLX90614 EEPROM address where the Slave Address is stored in order to get the most significant byte. */
    /** <b>Local int8_t variable ret:</b> Return value of either a HAL function or a @ref MLX90614_Status function type. */
    uint8_t ret = 0;
    /** <b>Local uint8_t variable received_i2cdata:</b> Used to hold 2-bytes of data read from the EEPROM of the MLX90614 Device, received via the I2C Communication Protocol. */
    uint8_t received_i2cdata[MLX90614_EEPROM_SLAVE_ADDRESS_SIZE] = {0, 0};
    ret = HAL_I2C_Mem_Read(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, MLX90614_SLAVE_ADDRESS_EEPROM_ADDRESS, MLX90614_RAM_OR_EEPROM_ADDRESS_SIZE, received_i2cdata, MLX90614_EEPROM_SLAVE_ADDRESS_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }

    /* STEP 2: Erase the 2 bytes of data stored corresponding to the Slave Address value in the MLX90614 EEPROM. */
    /** <b>Local uint8_t 3 bytes array variable new_mlx90614_eeprom_value:</b> Holds the data that wants to be written into the MLX90614 Device's EEPROM, where the first or least significant byte should stand for the MLX90614's EEPROM value where it is desired to start writing data, the next 2 bytes should contain the actual data that wants to be written into the MLX90614's EEPROM, and the last or most significant byte should stand for the PEC byte of the previously described values as calculated for the MLX90614 device. */
    uint8_t new_mlx90614_eeprom_value[MLX90614_I2C_WRITE_COMMAND_SIZE] = {MLX90614_SLAVE_ADDRESS_EEPROM_ADDRESS, MLX90614_SLAVE_ADDRESS_EEPROM_ERASE_VALUE, MLX90614_SLAVE_ADDRESS_EEPROM_ERASE_VALUE, MLX90614_PEC_RESET_VALUE};
    // Calculating the PEC byte before sending the corresponding data.
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], mlx90614_slave_address_one_bit_left_shifted);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[0]);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[1]);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[2]);
    ret = HAL_I2C_Master_Transmit(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, new_mlx90614_eeprom_value, MLX90614_I2C_WRITE_COMMAND_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }
    HAL_Delay(MLX90614_ERASE_OR_WRITE_CELL_TIME); // Giving time to the MLX90614 Device to Erase its EEPROM Slave Address value.

    /* STEP 3: Writing new Slave Address into MLX90614 EEPROM while keeping its original most significant byte. */
    new_mlx90614_eeprom_value[1] = new_slave_address;
    new_mlx90614_eeprom_value[2] = received_i2cdata[1];
    // Calculating the PEC byte before sending the corresponding data.
    new_mlx90614_eeprom_value[3] = calculate_pec(MLX90614_PEC_RESET_VALUE, mlx90614_slave_address_one_bit_left_shifted);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[0]);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[1]);
    new_mlx90614_eeprom_value[3] = calculate_pec(new_mlx90614_eeprom_value[3], new_mlx90614_eeprom_value[2]);
    ret = HAL_I2C_Master_Transmit(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, new_mlx90614_eeprom_value, MLX90614_I2C_WRITE_COMMAND_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }
    HAL_Delay(MLX90614_ERASE_OR_WRITE_CELL_TIME); // Giving time to the MLX90614 Device to Write the new Slave Address value in its EEPROM.

    /* Updating the MLX90614 Slave Address in the @ref mlx90614 . */
    mlx90614_slave_address = new_slave_address;
    mlx90614_slave_address_one_bit_left_shifted = new_slave_address << 1;

    /* STEP 4: Power Cycle (this must be done by implementer or user of this MLX90614 Driver either by external circuit; or by manually electrically disconnecting the MLX90614 Device and subsequently by manually electrically reconnecting it). */
    // NOTE: A Software Reset will not be enough; Electrical Power reconnection of the MLX90614 must strictly be made.
    return MLX90614_EC_OK;
}
#pragma GCC diagnostic pop

MLX90614_Temp_t get_mlx90614_temperature_type(void)
{
    return mlx90614_temperature_type;
}

MLX90614_Status set_mlx90614_temperature_type(MLX90614_Temp_t temp_t)
{
    /* Update the @ref p_get_mlx90614_converted_temperature pointer and update @ref mlx90614_temperature_type . */
    switch (temp_t)
    {
        case MLX90614_Temp_K:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_kelvin;
            break;
        case MLX90614_Temp_C:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_celsius;
            break;
        case MLX90614_Temp_F:
            p_get_mlx90614_converted_temperature = &get_mlx90614_converted_temperature_in_fahrenheit;
            break;
        default:
            return MLX90614_EC_ERR; // The requested temperature value type is not recognized. Therefore, send Error Exception Code.
    }
    mlx90614_temperature_type = temp_t;

    return MLX90614_EC_OK;
}

MLX90614_Status get_mlx90614_ambient_temperature(float *dst)
{
    /** <b>Local int8_t variable ret:</b> Return value of either a HAL function or a @ref MLX90614_Status function type. */
    uint8_t ret;
    /** <b>Local 2 bytes uint8_t array i2cdata:</b> Used to hold the Raw Data given back by the MLX90614 Device after requesting it to read temperature. */
    uint8_t i2cdata[2];

    /* Reading current Ambient Temperature Raw Value from MLX90614 Infra Red Thermometer device. */
    ret = HAL_I2C_Mem_Read(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, MLX90614_TA_RAM_ADDRESS, MLX90614_RAM_OR_EEPROM_ADDRESS_SIZE, i2cdata, MLX90614_TEMPERATURE_RESULT_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }

    /* Converting Raw Data read from MLX90614 Infra Red Thermometer into an actual temperature value according to its datasheet. */
    /** <b>Local uint16_t variable raw_temp:</b> Holds the Decimal Value corresponding to the Raw Data read from the MLX90614 Device after requesting to it a temperature value. */
    uint16_t raw_temp = ((i2cdata[1]<<8) | i2cdata[0]);
    if (raw_temp > 0x7FFF)
    {
        return MLX90614_EC_ERR; // According to the datasheet, if \c raw_temp > 0x7FFF, then this means that the MLX90614 Device has raised an Error Flag. However, I could not find information about the meaning of this or these possible Error Flags.
    }
    *dst = (*p_get_mlx90614_converted_temperature)(raw_temp);

    return MLX90614_EC_OK;
}

MLX90614_Status get_mlx90614_object1_temperature(float *dst)
{
    /** <b>Local int8_t variable ret:</b> Return value of either a HAL function or a @ref MLX90614_Status function type. */
    uint8_t ret;
    /** <b>Local 2 bytes uint8_t array i2cdata:</b> Used to hold the Raw Data given back by the MLX90614 Device after requesting it to read temperature. */
    uint8_t i2cdata[2];

    /* Reading current Object1 Temperature Raw Value from MLX90614 Infra Red Thermometer device. */
    ret = HAL_I2C_Mem_Read(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, MLX90614_TOBJ1_RAM_ADDRESS, MLX90614_RAM_OR_EEPROM_ADDRESS_SIZE, i2cdata, MLX90614_TEMPERATURE_RESULT_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }

    /* Converting Raw Data read from MLX90614 Infra Red Thermometer into an actual temperature value according to its datasheet. */
    /** <b>Local uint16_t variable raw_temp:</b> Holds the Decimal Value corresponding to the Raw Data read from the MLX90614 Device after requesting to it a temperature value. */
    uint16_t raw_temp = ((i2cdata[1]<<8) | i2cdata[0]);
    if (raw_temp > 0x7FFF)
    {
        return MLX90614_EC_ERR; // According to the datasheet, if \c raw_temp > 0x7FFF, then this means that the MLX90614 Device has raised an Error Flag. However, I could not find information about the meaning of this or these possible Error Flags.
    }
    *dst = (*p_get_mlx90614_converted_temperature)(raw_temp);

    return MLX90614_EC_OK;
}

MLX90614_Status get_mlx90614_object2_temperature(float *dst)
{
    /** <b>Local int8_t variable ret:</b> Return value of either a HAL function or a @ref MLX90614_Status function type. */
    uint8_t ret;
    /** <b>Local 2 bytes uint8_t array i2cdata:</b> Used to hold the Raw Data given back by the MLX90614 Device after requesting it to read temperature. */
    uint8_t i2cdata[2];

    /* Reading current Object2 Temperature Raw Value from MLX90614 Infra Red Thermometer device. */
    ret = HAL_I2C_Mem_Read(p_hi2c, mlx90614_slave_address_one_bit_left_shifted, MLX90614_TOBJ2_RAM_ADDRESS, MLX90614_RAM_OR_EEPROM_ADDRESS_SIZE, i2cdata, MLX90614_TEMPERATURE_RESULT_SIZE, MLX90614_I2C_TIMEOUT);
    ret = HAL_ret_handler(ret);
    if (ret != HAL_OK)
    {
        return ret;
    }

    /* Converting Raw Data read from MLX90614 Infra Red Thermometer into an actual temperature value according to its datasheet. */
    /** <b>Local uint16_t variable raw_temp:</b> Holds the Decimal Value corresponding to the Raw Data read from the MLX90614 Device after requesting to it a temperature value. */
    uint16_t raw_temp = ((i2cdata[1]<<8) | i2cdata[0]);
    if (raw_temp > 0x7FFF)
    {
        return MLX90614_EC_ERR; // According to the datasheet, if \c raw_temp > 0x7FFF, then this means that the MLX90614 Device has raised an Error Flag. However, I could not find information about the meaning of this or these possible Error Flags.
    }
    *dst = (*p_get_mlx90614_converted_temperature)(raw_temp);

    return MLX90614_EC_OK;
}

static float get_mlx90614_converted_temperature_in_kelvin(uint16_t raw_temp)
{
    return ((float) raw_temp)/50.0;
}

static float get_mlx90614_converted_temperature_in_celsius(uint16_t raw_temp)
{
    return ((float) raw_temp)/50.0 - 273.15;
}

static float get_mlx90614_converted_temperature_in_fahrenheit(uint16_t raw_temp)
{
    return (((float) raw_temp)/50.0-273.15)*1.8 + 32.0;
}

static uint8_t calculate_pec(uint8_t init_pec, uint8_t new_data)
{
    uint8_t data;
    uint8_t bit_check;
    data = init_pec ^ new_data;
    for (int i=0; i<8; i++ )
    {
        bit_check = data & 0x80;
        data = data << 1;

        if (bit_check != 0)
        {
            data = data ^ 0x07;
        }

    }
    return data;
}

static MLX90614_Status HAL_ret_handler(HAL_StatusTypeDef HAL_status)
{
    switch (HAL_status)
    {
        case HAL_BUSY:
        case HAL_TIMEOUT:
            return MLX90614_EC_NR;
        case HAL_ERROR:
            return MLX90614_EC_ERR;
        default:
            return HAL_status;
    }
}

/** @} */

