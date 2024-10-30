/**@file
 * @brief	MLX90614 Infra Red Thermometer's driver Header file.
 *
 * @defgroup mlx90614 MLX90614 Infra Red Thermometer Driver module
 * @{
 *
 * @brief   This module provides the functions, definitions, structures and variables that together work as the driver
 *          for the MLX90614 Infra Red Thermometer with the purpose of being used by the application.
 *
 * @details As for now, this @ref mlx90614 provides the necessary things so that the user can read the Object1, Object2
 *          and Ambient Temperatures from a MLX90614 Device, as well as to configure and even reconfigure the desired
 *          units at which it is desired to get those temperature values, which may either be at Kelvin, Celsius and
 *          Fahrenheit units.
 *@details The way that the @ref mlx90614 works is that this module must first be initialized via the
 *          @ref init_mlx90614_module function in order to enable all the other functions to work properly. In
 *          addition, this initialization function is the means with which the implementer will designate to the
 *          @ref mlx90614 the I2C of the MCU/MPU that this module will use in Polling Mode to execute the main function.
 *
 * @note    Functions to change the "ConfigRegister1" Register will not be provided since the MLX90614 Datasheet states
 *          that this registered must not be tampered with and that the reason that this Register is writable is due to
 *          factory calibration relevant purposes. It is also stated there that this Register is meant to be changed
 *          only via special tools provided by Melexis for whenever your particular MLX90614 Device requires to be
 *          calibrated.
 * @note    Similarly, the Max, Min and Range Temperature values that can be changed in the EEPROM of the MLX90614 are
 *          meant for the PWM Mode according to the MLX90614 datasheet. Therefore, because this library provides a means
 *          of communication with the MLX90614 Device via the I2C Protocol, it is unnecessary to provide functions for
 *          editing those EEPROM values.
 * @note    Another thing to highlight is that this @ref mlx90614 has included the "stm32f1xx_hal.h" header file
 *          to be able to use the I2C in this module. However, this header file is specifically meant for the STM32F1
 *          series devices. If yours is from a different type, then you will have to substitute the right one here for
 *          your particular STMicroelectronics device. However, if you cant figure out what the name of that header file
 *          is, then simply substitute that line of code from this @ref mlx90614 by: #include "main.h"
 *
 * @details <b><u>Code Example for reading the Object1, Object2 and Ambient Temperatures Device via the @ref mlx90614
 *          :</u></b>
 *
 * @code
  #include <stdio.h>	// Library from which "printf" is located at.
  #include <stdint.h>   // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.
  #include "mlx90614_ir_thermometer_driver.h" // This custom Mortrack's library contains the functions, definitions and variables that together operate as the driver for the MLX90614 Device.

  // ################################################# //
  // ##### INITIALIZATION OF THE MLX90614 MODULE ##### //
  // ################################################# //
  // IMPORTANT-NOTE: Please use this test code while having only one MLX90614 connected to our MCU/MPU. However, feel free to modify it for your needs and particular project, where you can even use several MLX90614 device simultaneously.
  MLX90614_Status ret; // Variable used to hold the Status Codes returned by the functions of the MLX90614 Driver Library Module.
  const uint8_t DO_NOT_UPDATE_SLAVE_ADDRESS = 0; // This variable holds the value that the @ref init_mlx90614_module interprets for whenever its implementer does not know/want to assign a slave address during the initialization of the MLX90614 Driver Library Module.
  printf("\r\n\r\n\r\n\r\nINFO: Initializing the MLX90614 Driver Library Module...\r\n");
  ret = init_mlx90614_module(&hi2c2, DO_NOT_UPDATE_SLAVE_ADDRESS, MLX90614_Temp_C);
  if (ret == MLX90614_EC_OK)
  {
	  printf("SUCCESS: The MLX90614 Driver Library Module has been successfully initialized.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library Module could not be initialized.\r\n");
	  while(1);
  }


  // ########################################################## //
  // ##### FINDING THE SLAVE ADDRESS OF A MLX90614 DEVICE ##### //
  // ########################################################## //
  // ----- Finding the slave address of a MLX90614 Device ----- //
  printf("INFO: Requesting the MLX90614 Driver Library to find a device ready for I2C communication via the I2C peripheral with which this Driver Library was initialized with...\r\n");
  ret = find_mlx90614_slave_address();
  if (ret == MLX90614_EC_OK)
  {
	  printf("SUCCESS: The MLX90614 Driver Library has successfully found a device ready for I2C communication.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library could not find a device ready for I2C communication.\r\n");
	  while(1);
  }
  uint8_t slave_address_configured_in_the_driver_library = 0; // This variable will hold the slave address that the MLX90614 Driver Library found.
  printf("INFO: Getting the Slave Address value previously found by the Driver Library...\r\n");
  slave_address_configured_in_the_driver_library = get_mlx90614_module_slave_address();
  printf("INFO: Slave Address value found = %d (in decimal value).\r\n", slave_address_configured_in_the_driver_library);

  // ----- Proving that configuring a NON-MATCHING Slave Address value will not work. ----- //
  uint8_t non_matching_mlx90614_slave_address; // This variable will intentionally hold a slave address value that is different than the one found by the MLX90614 Driver Library.
  if (slave_address_configured_in_the_driver_library == 0x01)
  {
	  non_matching_mlx90614_slave_address = 0x02;
  }
  else
  {
	  non_matching_mlx90614_slave_address = 0x01;
  }
  printf("INFO: Proving that configuring a Slave Address in the MLX90614 Driver Library that does not match with the connected MLX90614 Device will not work...\r\n");
  ret = set_mlx90614_module_slave_address(non_matching_mlx90614_slave_address); // Prove that non-matching Slave Address will not work.
  if (ret == MLX90614_EC_NR)
  {
	  printf("SUCCESS: The non-matching Slave Address given was not accepted by the MLX90614 Driver Library...\r\n");
  }
  else
  {
	  printf("ERROR: Unexpectedly, the MLX90614 Driver Library accepted and configured a non-matching Slave Address in it.\r\n");
	  while(1);
  }

  // ----- Proving that configuring an INVALID Slave Address value will not work. ----- //
  uint8_t non_valid_mlx90614_slave_address = 128; // This variable will intentionally hold an invalid MLX90614 slave address.
  printf("INFO: Proving that requesting the MLX90614 Driver Library to configure in it an invalid MLX90614 Slave Address will not be allowed...\r\n");
  ret = set_mlx90614_module_slave_address(non_valid_mlx90614_slave_address); // Prove that a non-valid Slave Address will not work.
  if (ret == MLX90614_EC_ERR)
  {
	  printf("SUCCESS: The MLX90614 Driver Library did not accept to configure in it the given slave address because it has an invalid MLX90614 slave address value.\r\n");
  }
  else
  {
	  printf("ERROR: Unexpectedly, the MLX90614 Driver Library accepted and configured a non-valid Slave Address in it.\r\n");
	  while(1);
  }
  printf("INFO: Proving that the MLX90614 Driver Library still has the same/previously valid configured Slave Address in it.\r\n");
  if (get_mlx90614_module_slave_address() == slave_address_configured_in_the_driver_library)
  {
	  printf("SUCCESS: The currently configured Slave Address in the MLX90614 Driver Library is still %d (in decimal value).\r\n", slave_address_configured_in_the_driver_library);
  }
  else
  {
	  printf("ERROR: Unexpectedly, the MLX90614 Driver Library has a different Slave Address to that of %d (in decimal value).\r\n", slave_address_configured_in_the_driver_library);
	  while(1);
  }


  // ####################################################### //
  // ##### ASSIGNATION OF A CUSTOM SLAVE ADDRESS TO IT ##### //
  // ####################################################### //
  //// NOTE: This commented code was not used by the author of this Driver Library to validate it since he was not able to make the 4-step method used in the @ref set_mlx90614_device_slave_address function to work as it did for some people in the internet (apparently, it worked for some people and not for others due to that those people, such as it happened to the author of this Driver Library, they were not able to to write into the required EEPROM address to change the slave address in the device, where this author's conclusion is that perhaps some MLX90614 cannot have that EEPROM address written, while others may be).
  //uint8_t new_slave_address; // This variable will hold the New Slave Address that wants to be written into the MLX90614 Device's EEPROM and to also be configured in the MLX90614 Driver Library.
  //if (slave_address_configured_in_the_driver_library == 0x5A)
  //{
  //	  new_slave_address = 0x41;
  //}
  //else
  //{
  //	  new_slave_address = 0x5A;
  //}
  //printf("INFO: Changing the Device Slave value in the EEPROM of the MLX90614 Device and also configuring it in the MLX90614 Driver Library...\r\n");
  //ret =  set_mlx90614_device_slave_address(new_slave_address);
  //if (test_ret == MLX90614_EC_OK)
  //{
  //	  printf("SUCCESS: The MLX90614 Slave Address in its EEPROM has been changed to %d (in decimal value).\r\n", new_slave_address);
  //}
  //else
  //{
  //	  printf("ERROR: The MLX90614 Driver Library could not change the Slave Address in the MLX90614 EEPROM.\r\n");
  //	  while(1);
  //}
  //printf("WAITING: Please indicate to the MCU/MPU, via the assigned Input GPIO, that the Power Cycle of the MLX90614 Device has been made.\r\n");
  //// NOTE: If the following GPIO Peripheral Pin is "Set", then it will be considered by this program that our MCU/MPU is being informed that the MLX90614 Power-Cycle has be done. Otherwise, our MCU/MPU will stand by waiting for that, since this is required after changing the Slave Address of a MLX90614 Device.
  //if (HAL_GPIO_ReadPin(is_MLX90614_powered_cycled_GPIO_Input_GPIO_Port, is_MLX90614_powered_cycled_GPIO_Input_Pin) != GPIO_PIN_SET)
  //{
  //	  while(1);
  //}
  //printf("INFO: Proving that the MLX90614 Driver Library has now configured the New Slave Address in it because the MLX90614 Device now responds to that one...\r\n");
  //slave_address_configured_in_the_driver_library = get_mlx90614_module_slave_address();
  //if (slave_address_configured_in_the_driver_library == new_slave_address)
  //{
  //	  printf("SUCESS: The MLX90614 Driver Library currently has the expected value of %d (in decimal value) configured as the Slave Address in it.\r\n", slave_address_configured_in_the_driver_library);
  //}
  //else
  //{
  //	  printf("ERROR: The MLX90614 did not configured the expected Slave Address of %d (in decimal value) in it.\r\n", slave_address_configured_in_the_driver_library);
  //	  while(1);
  //}

  // ############################################################################################ //
  // ##### MEASURING THE OBJECT1, OBJECT2 AND AMBIENT TEMPERATURES FROM THE MLX90614 DEVICE ##### //
  // ############################################################################################ //
  // ----- Reading Ambient, Object1 and Object2 Temperatures in Kelvin units ----- //
  printf("INFO: Reconfiguring the MLX90614 Driver Library to respond back with Kelvin units for all temperature readings...\r\n");
  ret = set_mlx90614_temperature_type(MLX90614_Temp_K); // Set to Kelvin.
  if (ret == MLX90614_EC_OK)
  {
	  printf("SUCCESS: The MLX90614 Driver Library has been successfully configured to respond back with Kelvin units for all temperature readings from now on.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library could not configure to respond back with Kelvin units.\r\n");
	  while(1);
  }
  printf("INFO: Verifying that the MLX90614 Driver Library is going to now respond back with Kelving units for all temperature readings made...\r\n");
  if (MLX90614_Temp_K == get_mlx90614_temperature_type()) // Confirm having changed the Temperature Type.
  {
	  printf("SUCESS: The MLX90614 Driver Library further readings in Kelvin units has been successfully confirmed.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library was not really configured to respond back in Kelvin units from now on.\r\n");
	  while(1);
  }
  printf("INFO: Requesting the MLX90614 Device for the current Object1, Object2 and Ambient Temperatures...\r\n");
  float current_temperature_reading = 0; // This variable will hold the currently or last requested temperature reading from the MLX90614 Device.
  ret = get_mlx90614_ambient_temperature(&current_temperature_reading); // Get Ambient Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Ambient Temperature = %f Kelvin\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Ambient Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object1_temperature(&current_temperature_reading); // Get Object1 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object1 Temperature = %f Kelvin\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object1 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object2_temperature(&current_temperature_reading); // Get Object2 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object2 Temperature = %f Kelvin\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object2 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  printf("SUCESS: The Object1, Object2 and Ambient Temperatures were successfully read from the MLX90614 device.\r\n");

  // ----- Reading Ambient, Object1 and Object2 Temperatures in Celsius units ----- //
  printf("INFO: Reconfiguring the MLX90614 Driver Library to respond back with Celsius units for all temperature readings...\r\n");
  ret = set_mlx90614_temperature_type(MLX90614_Temp_C); // Set to Celsius.
  if (ret == MLX90614_EC_OK)
  {
	  printf("SUCCESS: The MLX90614 Driver Library has been successfully configured to respond back with Celsius units for all temperature readings from now on.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library could not configure to respond back with Celsius units.\r\n");
	  while(1);
  }
  printf("INFO: Verifying that the MLX90614 Driver Library is going to now respond back with Kelving units for all temperature readings made...\r\n");
  if (MLX90614_Temp_C == get_mlx90614_temperature_type()) // Confirm having changed the Temperature Type.
  {
	  printf("SUCESS: The MLX90614 Driver Library further readings in Celsius units has been successfully confirmed.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library was not really configured to respond back in Celsius units from now on.\r\n");
	  while(1);
  }
  printf("INFO: Requesting the MLX90614 Device for the current Object1, Object2 and Ambient Temperatures...\r\n");
  ret = get_mlx90614_ambient_temperature(&current_temperature_reading); // Get Ambient Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Ambient Temperature = %f Celsius\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Ambient Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object1_temperature(&current_temperature_reading); // Get Object1 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object1 Temperature = %f Celsius\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object1 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object2_temperature(&current_temperature_reading); // Get Object2 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object2 Temperature = %f Celsius\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object2 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  printf("SUCESS: The Object1, Object2 and Ambient Temperatures were successfully read from the MLX90614 device.\r\n");

  // ----- Reading Ambient, Object1 and Object2 Temperatures in Fahrenheit units ----- //
  printf("INFO: Reconfiguring the MLX90614 Driver Library to respond back with Fahrenheit units for all temperature readings...\r\n");
  ret = set_mlx90614_temperature_type(MLX90614_Temp_F); // Set to Fahrenheit.
  if (ret == MLX90614_EC_OK)
  {
	  printf("SUCCESS: The MLX90614 Driver Library has been successfully configured to respond back with Fahrenheit units for all temperature readings from now on.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library could not configure to respond back with Fahrenheit units.\r\n");
	  while(1);
  }
  printf("INFO: Verifying that the MLX90614 Driver Library is going to now respond back with Kelving units for all temperature readings made...\r\n");
  if (MLX90614_Temp_F == get_mlx90614_temperature_type()) // Confirm having changed the Temperature Type.
  {
	  printf("SUCESS: The MLX90614 Driver Library further readings in Fahrenheit units has been successfully confirmed.\r\n");
  }
  else
  {
	  printf("ERROR: The MLX90614 Driver Library was not really configured to respond back in Fahrenheit units from now on.\r\n");
	  while(1);
  }
  printf("INFO: Requesting the MLX90614 Device for the current Object1, Object2 and Ambient Temperatures...\r\n");
  ret = get_mlx90614_ambient_temperature(&current_temperature_reading); // Get Ambient Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Ambient Temperature = %f Fahrenheit\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Ambient Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object1_temperature(&current_temperature_reading); // Get Object1 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object1 Temperature = %f Fahrenheit\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object1 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  ret = get_mlx90614_object2_temperature(&current_temperature_reading); // Get Object2 Temperature reading.
  if (ret == MLX90614_EC_OK)
  {
	  printf("Current Object2 Temperature = %f Fahrenheit\r\n", current_temperature_reading);
  }
  else
  {
	  printf("ERROR: The current Object2 Temperature could not be read from the MLX90614 Device.\r\n");
	  while(1);
  }
  printf("SUCESS: The Object1, Object2 and Ambient Temperatures were successfully read from the MLX90614 device.\r\n");
  printf("The MLX90614 Driver Library validation test has successfully concluded!\r\n");


  while(1); // Stop the program here.
 * @endcode
 *
 * @author 	Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date	June 23, 2024.
 */

#ifndef MLX90614_IR_THERMOMETER_H_
#define MLX90614_IR_THERMOMETER_H_

#include "stm32f1xx_hal.h" // This is the HAL Driver Library for the STM32F1 series devices. If yours is from a different type, then you will have to substitute the right one here for your particular STMicroelectronics device. However, if you cant figure out what the name of that header file is, then simply substitute this line of code by: #include "main.h"
#include <stdint.h> // This library contains the aliases: uint8_t, uint16_t, uint32_t, etc.

#define MLX90614_I2C_TIMEOUT                (100)     /**< @brief Time in milliseconds that our MCU/MPU will wait for the MLX90614 Infra Red Thermometer device to respond to an I2C transaction between them. */
#define MLX90614_ERASE_OR_WRITE_CELL_TIME   (1000)    /**< @brief Time in milliseconds that our MCU/MPU will wait for the MLX90614 Infra Red Thermometer device for either Erasing or Writing Cells in its EEPROM. @note  This is only used whenever erasing and writing a new Slave Address into the MLX90614 EEPROM. @note  Have in consideration that the MLX90614 Datasheet states that either erasing or writing EEPROM cells typically takes 5ms. However, they do not tell what is the maximum expected wait time for this. Therefore, in order to be very safe, a very larger time was assigned, but feel free to change it according to your needs as long as you have this information in mind. */
#define IS_MLX90614_READY_NUMBER_OF_TRIALS	(1)       /**< @brief Number of attempts to be made whenever checking if the MLX90614 Infra Red Thermometer is ready for I2C communication. */

/**@brief	MLX90614 Infra Red Thermometer Driver Exception codes.
 *
 * @details	These Exception Codes are returned by the functions of the @ref mlx90614 to indicate the resulting
 *          status of having executed the process contained in each of those functions. For example, to indicate that
 *          the process executed by a certain function was successful or that it has failed.
 */
typedef enum
{
    MLX90614_EC_OK      = 0U,    //!< MLX90614 Infra Red Thermometer Driver Process was successful.
    MLX90614_EC_STOP    = 1U,    //!< MLX90614 Infra Red Thermometer Driver Process has been stopped.
    MLX90614_EC_NR      = 2U,	 //!< MLX90614 Infra Red Thermometer Driver Process has concluded with no response.
    MLX90614_EC_NA      = 3U,    //!< MLX90614 Infra Red Thermometer Driver Data received or to be received Not Applicable.
    MLX90614_EC_ERR     = 4U     //!< MLX90614 Infra Red Thermometer Driver Process has failed.
} MLX90614_Status;

/**@brief	MLX90614 Infra Red Thermometer Temperature values types definitions.
 *
 * @note    These definitions are defined with respect to the temperature values types that can be managed by this
 *          @ref mlx90614 .
 */
typedef enum
{
    MLX90614_Temp_K = 0,    //!< MLX90614 Infra Red Thermometer values read in Kelvin.
    MLX90614_Temp_C = 1,    //!< MLX90614 Infra Red Thermometer values read in Celsius.
    MLX90614_Temp_F = 2     //!< MLX90614 Infra Red Thermometer values read in Fahrenheit.
} MLX90614_Temp_t;

/**@brief	Finds a Device that is ready for I2C communication, if there is any, and configures its slave address to
 *          this @ref mlx90614 .
 *
 * @details This function will start searching for a device via the I2C assigned to this @ref mlx90614 during
 *          the @ref init_mlx90614_module function. The search will start from the lowest slave address value up to
 *          highest one and, if an I2C capable device is actually found, then this function will update the following
 *          Global Static Variables:<br>
 *          - The @ref mlx90614_slave_address Global Static Variable with the value of the MLX90614 device slave address found.
 *          - The @ref mlx90614_slave_address_one_bit_left_shifted Global Static Variable with the value towards which \p mlx90614_slave_address points to, but with one bit shifted to the left, as long as the value towards which \p mlx90614_slave_address points has been successfully updated.
 * @details <i><b style="color:red;"><u>WARNING</u>:</b><b>It is the responsibility of the implementer to make sure that
 *          the I2C capable device wired to the I2C assigned to this @ref mlx90614 , is actually a MLX90614 Infra Red
 *          Thermometer Device because this function will only be able to tell that it found a device ready for I2C
 *          Communication, but it will not be able to know if it is an actual MLX90614 device</b>.</i>
 *
 * @note    Know that if no device is found, then this will mean that @ref mlx90614_slave_address will be disconfigured.
 *          Therefore, If a functional MLX90614 sensor is connected while the MCU/MPU is still powered-On and running,
 *          then the MCU/MPU will require to either run this function again or to configure the Slave Address of that
 *          sensor via the @ref set_mlx90614_module_slave_address function. However, if the implementer of this library
 *          was expecting this function to work, but it did not happened that way, then the implementer will have to
 *          check the wiring between the MCU/MPU and MLX90614 Sensor(s) being used and to make sure that both/all of
 *          them are functional and working correctly.
 *
 * @retval  MLX90614_EC_OK  If the a MLX90614 Infra Red Thermometer was found to be ready for I2C communication and if
 *                          its slave address was successfully configured in the @ref mlx90614 .
 * @retval  MLX90614_EC_NR  If no MLX90614 Infra Red Thermometer was found to be ready for I2C communication.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 19, 2024.
 */
MLX90614_Status find_mlx90614_slave_address(void);

/**@brief	Gets the slave address of the MLX90614 Infra Red Thermometer that is currently configured for use in the
 *          @ref mlx90614 .
 *
 * @return  The slave address of the MLX90614 device that is currently configured in the @ref mlx90614 for use.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 19, 2024.
 */
uint8_t get_mlx90614_module_slave_address(void);

/**@brief	Sets and configures a new MLX90614 device slave address in the @ref mlx90614 .
 *
 * @details This function will validate that the given slave address value is valid for a MLX90614 device and will then
 *          check if there is a device ready for I2C communication, under the I2C configured in @ref mlx90614 , with
 *          that slave address. However, beware that if this function detects a device ready for I2C communication under
 *          the given slave address, this function will not really be able to tell if that device corresponds to an
 *          MLX90614 device, but will simply know that a certain device responded to be ready for I2C communication with
 *          that particular slave address. Therefore:
 * @details <i><b style="color:red;"><u>WARNING</u>:</b><b>It is the responsibility of the implementer to make sure that
 *          the given slave address stands for an actual MLX90614 device</b>.</i>
 *
 * @param slave_address     Slave address value that must match the one that has been designated to the MLX90614 Infra
 *                          Red Thermometer. The following are the possible values of this param:<br><br>
 *                          * \f$1_{d}\f$ up to \f$127_{d}\f$ = Valid slave address.<br>
 *                          * \f$0\f$ and any value greater than \f$127_{d}\f$ = Invalid slave address values.
 *
 * @retval  MLX90614_EC_OK  If the given slave address was successfully validated and configured, and if a MLX90614
 *                          device is ready for an I2C communication.
 * @retval  MLX90614_EC_NR If there was no MLX90614 device ready for an I2C communication.
 * @retval  MLX90614_EC_ERR <ul>
 *                              <li>
 *                                  If the \p slave_address param contains an invalid slave address value.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 19, 2024.
 */
MLX90614_Status set_mlx90614_module_slave_address(uint8_t slave_address);

/**@brief	Sets a new Slave Address value in the corresponding MLX90614 EEPROM Address and also configured that Slave
 *          Address value in the @ref mlx90614 .
 *
 * @details This function will first validate that the given slave address value is valid for a MLX90614 device. Then,
 *          the Slave Address in the MLX90614 EEPROM will be erased and subsequently written with the new/given slave
 *          address, followed by a delay of @ref MLX90614_ERASE_OR_WRITE_CELL_TIME after each of the erasing and writing
 *          processes. After that, a Read Request will be made to the MLX90614 Device in order to validate that the new
 *          slave address was correctly written into its EEPROM. Next, the slave address in the @ref mlx90614 will be
 *          updated with the new one. Finally, the MLX90614 device is expected to be Power-Cycled (i.e., Software Reset
 *          is not going to be enough; make sure to electrically power-Off the MLX90614 Device and then to electrically
 *          power it On again).
 *
 * @note    <i><b style="color:red;"><u>WARNING 1</u>:</b><b>The author of @ref mlx90614 made sure that this function
 *          applies <a href=https://forum.mikroe.com/viewtopic.php?t=67842&fbclid=IwZXh0bgNhZW0CMTAAAR201BxXMK0Zf8QaL2r8vzwVFsQCKXd4v_XcA3elM7qyCQ_S7QM_SOqympc_aem_GqwHbzcMEAGRhF8-MJe3OA>
 *          the 4-step method to change the slave address of a MLX90614 device as described by mynos at a Mikroe
 *          Forum</a>. After some research of the author of this Driver Library ,</b> <b style="color:orange;">it was
 *          determined that this method has apparently worked for several individuals, but it did not work for the
 *          author of this Driver Library</b><b>. Instead, what happened to this author was that he was able to read the
 *          EEPROM address, where its stored value seems to correspond to the actual MLX90614 Slave Address, but where
 *          it was not possible to rewrite that value.</b> <b style="color:orange;"> However, it was decided to leave
 *          this function as it is in hopes that it may be useful for others by giving the benefit of the doubt that
 *          <u>perhaps some particular MLX90614 models can be written at the required EEPROM</u> address, while others
 *          may not</b><b>.</b></i>
 *
 * @note    <i><b style="color:red;"><u>WARNING 2</u>:</b><b>It is the responsibility of the implementer to Power-Cycle
 *          the MLX90614 device right after this function ends and before using another function from the @ref mlx90614
 *          .</b></i>
 *
 * @note    <i><b style="color:red;"><u>WARNING 3</u>:</b><b>It is the responsibility of the implementer to make sure to
 *          only connect/wire the MLX90614 device or devices to the assigned I2C Peripheral of this Driver Library (via
 *          the @ref init_mlx90614_module function) that want to have their Slave Addresses changed, since this function
 *          will most probably change the Slave Addresses in the EEPROM of all the MLX90614 devices that are currently
 *          simultaneously connected this that I2C Peripheral. Therefore, it is highly suggested to have only one
 *          MLX90614 Device connected at a time whenever it is intended to use this function.</b></i>
 *
 * @param new_slave_address New slave address value that wants to be assigned and stored in the EEPROM of to the
 *                          MLX90614 Infra Red Thermometer. The following are the possible values of this param:<br><br>
 *                          * \f$1_{d}\f$ up to \f$127_{d}\f$ = Valid slave address.<br>
 *                          * \f$0\f$ and any value greater than \f$127_{d}\f$ = Invalid slave address values.
 *
 * @retval  MLX90614_EC_OK  If the given slave address was successfully validated; stored in the MLX90614 EEPROM; and
 *                          configured in the @ref mlx90614 .
 * @retval  MLX90614_EC_NR If there was no MLX90614 device ready for an I2C communication.
 * @retval  MLX90614_EC_ERR <ul>
 *                              <li>
 *                                  If the \p slave_address param contains an invalid slave address value. A valid slave
 *                                  address should be within the range of @ref MLX90614_MIN_VALID_SLAVE_ADDRESS_VALUE up
 *                                  to @ref MLX90614_MAX_VALID_SLAVE_ADDRESS_VALUE .
 *                              </li>
 *                              <li>
 *                                  If, after requesting to write the \p slave_address param into the MLX90614 EEPROM,
 *                                  the MLX90614 Device responded back with a Slave Address that did not match with
 *                                  the one given via the \p slave_address param.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 23, 2024.
 */
MLX90614_Status set_mlx90614_device_slave_address(uint8_t slave_address);

/**@brief	Gets the Temperature Type with which the @ref mlx90614 is currently responding with whenever a temperature
 *          value is requested from the MLX90614 Infra Red Thermometer.
 *
 * @return  The Temperature Type with which the @ref mlx90614 is currently responding with.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
MLX90614_Temp_t get_mlx90614_temperature_type(void);

/**@brief	Sets and configures a new Temperature Type in the @ref mlx90614 for whenever a temperature value is
 *          requested from the MLX90614 Infra Red Thermometer.
 *
 * @param temp_t    Temperature Type that wants to be configured in the @ref mlx90614.
 *
 * @retval  MLX90614_EC_OK  If the given Temperature Type is valid and was successfully configured in the @ref mlx90614 .
 * @retval  MLX90614_EC_ERR If the \p temp_t param has an invalid value.
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
MLX90614_Status set_mlx90614_temperature_type(MLX90614_Temp_t temp_t);

/**@brief	Gets the Ambient Temperature from the MLX90614 Infra Red Thermometer Device in the units corresponding to
 *          the currently configured Temperature Type in @ref mlx90614 .
 *
 * @note    Use the @ref get_mlx90614_temperature_type and @ref set_mlx90614_temperature_type functions to respectively
 *          read or change the Temperature Type currently configured in @ref mlx90614 .
 *
 * @param[out] dst  Pointer to the Memory Address where this function will store the Ambient Temperature value read from
 *                  the MLX90614 Infra Red Thermometer Device.
 *
 * @retval  MLX90614_EC_OK  If the Ambient Temperature was successfully read from the MLX90614 Infra Red Thermometer
 *                          Device with no Error Flags raised by that Device; if such reading was correctly
 *                          converted into the corresponding units according to the currently configured Temperature
 *                          Type in @ref mlx90614 ; and if this converted reading was successfully stored into where the
 *                          \p dst param points to.
 * @retval  MLX90614_EC_NR  If the MLX90614 Infra Red Thermometer did not respond while attempting to communicate with
 *                          it via the I2C Communication Protocol.
 * @retval  MLX90614_EC_ERR The Ambient Temperature could not be obtained and stored in our MCU due to one of the
 *                          following errors:
 *                          <ul>
 *                              <li>
 *                                  If the MLX90614 Device raised an Error Flag.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 22, 2024.
 */
MLX90614_Status get_mlx90614_ambient_temperature(float *dst);

/**@brief	Gets the Object1 Temperature from the MLX90614 Infra Red Thermometer Device in the units corresponding to
 *          the currently configured Temperature Type in @ref mlx90614 .
 *
 * @note    Use the @ref get_mlx90614_temperature_type and @ref set_mlx90614_temperature_type functions to respectively
 *          read or change the Temperature Type currently configured in @ref mlx90614 .
 *
 * @param[out] dst  Pointer to the Memory Address where this function will store the Object1 Temperature value read from
 *                  the MLX90614 Infra Red Thermometer Device.
 *
 * @retval  MLX90614_EC_OK  If the Object1 Temperature was successfully read from the MLX90614 Infra Red Thermometer
 *                          Device with no Error Flags raised by that Device; if such reading was correctly
 *                          converted into the corresponding units according to the currently configured Temperature
 *                          Type in @ref mlx90614 ; and if this converted reading was successfully stored into where the
 *                          \p dst param points to.
 * @retval  MLX90614_EC_NR  If the MLX90614 Infra Red Thermometer did not respond while attempting to communicate with
 *                          it via the I2C Communication Protocol.
 * @retval  MLX90614_EC_ERR The Object1 Temperature could not be obtained and stored in our MCU due to one of the
 *                          following errors:
 *                          <ul>
 *                              <li>
 *                                  If the MLX90614 Device raised an Error Flag.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
MLX90614_Status get_mlx90614_object1_temperature(float *dst);

/**@brief	Gets the Object2 Temperature from the MLX90614 Infra Red Thermometer Device in the units corresponding to
 *          the currently configured Temperature Type in @ref mlx90614 .
 *
 * @note    Use the @ref get_mlx90614_temperature_type and @ref set_mlx90614_temperature_type functions to respectively
 *          read or change the Temperature Type currently configured in @ref mlx90614 .
 * @note    <i><b style="color:red;"><u>WARNING</u>:</b><b>The Object2 Temperature readings made in the validation
 *          tests by the author of @ref mlx90614 seemed to not work and gives 0 Kelvin or equivalent readings in other
 *          Temperature Unit Types. However, a deep analysis of the code of this function was made and it was determined
 *          that it works correctly. Therefore, this author suspects that either Object2 Temperature readings work only
 *          for some MLX90614 models; or it is used only in the PWM Mode (this mode was not reviewed/studied in detail);
 *          or the special tools that the manufacturer Melexis talks about in the MLX90614 Datasheet may be required
 *          in order to configure and/or enable correct Object2 Temperature readings</b>.</i>
 *
 * @param[out] dst  Pointer to the Memory Address where this function will store the Object2 Temperature value read from
 *                  the MLX90614 Infra Red Thermometer Device.
 *
 * @retval  MLX90614_EC_OK  If the Object2 Temperature was successfully read from the MLX90614 Infra Red Thermometer
 *                          Device with no Error Flags raised by that Device; if such reading was correctly
 *                          converted into the corresponding units according to the currently configured Temperature
 *                          Type in @ref mlx90614 ; and if this converted reading was successfully stored into where the
 *                          \p dst param points to.
 * @retval  MLX90614_EC_NR  If the MLX90614 Infra Red Thermometer did not respond while attempting to communicate with
 *                          it via the I2C Communication Protocol.
 * @retval  MLX90614_EC_ERR The Object2 Temperature could not be obtained and stored in our MCU due to one of the
 *                          following errors:
 *                          <ul>
 *                              <li>
 *                                  If the MLX90614 Device raised an Error Flag.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 21, 2024.
 */
MLX90614_Status get_mlx90614_object2_temperature(float *dst);

/**@brief   Initializes the @ref mlx90614 in order to be able to use its provided functions and also starts that
 *          module.
 *
 * @details This function will also update several of its Global Static Pointers and Variables that are used in the
 *          @ref mlx90614.
 *
 * @note    This Global Static Pointers and variables that will be updated, initialized and/or persisted in the
 *          @ref mlx90614 by this function are the following:<br>
 *          - The @ref p_hi2c Global Static Pointer to point to the address towards which the \p hi2c param points to.
 *          - The @ref mlx90614_slave_address Global Static Variable with the value of the \p slave_address param in the case that it contains a valid value and whenever its value is different than 0, and as long as there is a Device ready for I2C Communication under the given I2C via the \p hi2c param.
 *          - The @ref mlx90614_slave_address_one_bit_left_shifted Global Static Variable with the value towards which \p mlx90614_slave_address points to, but with one bit shifted to the left, as long as the value towards which \p mlx90614_slave_address points has been successfully updated.
 *          - The @ref p_get_mlx90614_object1_temperature Global Static Pointer to point to the address of the corresponding get function of the MLX90614 object1 temperature with respect to the requested Temperature Type via the @ref temp_t param.
 *          - The @ref p_get_mlx90614_object2_temperature Global Static Pointer to point to the address of the corresponding get function of the MLX90614 object2 temperature with respect to the requested Temperature Type via the @ref temp_t param.
 *          - The @ref p_get_mlx90614_ambient_temperature Global Static Pointer to point to the address of the corresponding get function of the MLX90614 ambient temperature with respect to the requested Temperature Type via the @ref temp_t param.
 *          - The @ref mlx90614_temperature_type Global Static Variable containing the Temperature Type with which the @ref mlx90614 will be responding whenever it is requested to give a temperature value.
 *
 * @note    <b>This function must be called only once</b> before calling any other function of the @ref mlx90614 .
 *
 * @param[in] hi2c          Pointer to the I2C is desired for the @ref mlx90614 to use for exchanging information with
 *                          the MLX90614 Infra Red Thermometer via the I2C Communication Protocol.
 * @param slave_address     Slave address value to be configured in the @ref mlx90614, which must match the slave
 *                          address that has been designated in the MLX90614 Infra Red Thermometer.
 *                          The following are the possible values of this param:<br><br>
 *                          * \f$0\f$  = The slave address will not be updated and the default \c 0x5A value will be kept.<br>
 *                          * \f$1_{d}\f$ up to \f$127_{d}\f$ = Update the slave address with the given custom value.<br>
 *                          * \f$\geq 128_{d}\f$ = Invalid slave address values.
 * @param temp_t            Desired Temperature Type with which it is desired for the @ref mlx90614 to respond back
 *                          whenever requesting it to get a temperature reading.
 *
 * @retval  MLX90614_EC_OK  If the @ref mlx90614 has been successfully initialized.
 * @retval  MLX90614_EC_NR  If either the given slave address via the @ref slave_address param is incorrect or the
 *                          MLX90614 Infra Red Thermometer wired to our MCU/MPU is not ready for I2C Communication (if
 *                          this happens check that the wiring is correct, that your MLX90614 is functioning correctly
 *                          and that your MCU/MPU I2C peripheral has been correctly configured). If any of these two
 *                          cases give place, then the @ref mlx90614 will not be initialized.
 * @retval  MLX90614_EC_ERR The @ref mlx90614 was not initialized due to one of the following errors:
 *                          <ul>
 *                              <li>
 *                                  If the \p slave_address param contains an invalid slave address value.
 *                              </li>
 *                              <li>
 *                                  If a non-recognized value is given via the \p temp_t param.
 *                              </li>
 *                              <li>
 *                                  If anything else went wrong.
 *                              </li>
 *                          </ul>
 *
 * @author	César Miranda Meza (cmirandameza3@hotmail.com)
 * @date    June 19, 2024.
 */
MLX90614_Status init_mlx90614_module(I2C_HandleTypeDef *hi2c, uint8_t slave_address, MLX90614_Temp_t temp_t);

#endif /* MLX90614_IR_THERMOMETER_H_ */

/** @} */

