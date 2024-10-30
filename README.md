# MLX90614 Infra Red Thermometer C STMicroelectronics driver library

Version: 1.1.1.1

This library provides the definitions, variables and functions necessary so that its implementer can use and communicate
with a MLX90614 Infra Red Thermometer Device from a microcontroller/microprocessor of the STMicroelectronics device
family and, in particular, via the STM32CubeIDE app. Know that this Driver Library is very complete for professional
projects where you require to use this sensor and that all the available functions and features of this Driver Library
has been successfully validated. However, only one function provided here, whose purpose is to change this device's
slave address was not validated and, according to several sources over the internet, it seems that changing an MLX90614
slave address with the method implemented in this Driver Library may work for some MLX90614 sensors but not for others.
Nonetheless, if you do not need to change your MLX90614 device's slave address or if, in the worst case scenario, you
can solve that part externally, you can have this Driver Library at you disposal to work in very professional and even
educational projects by using the I2C Protocol. For more details about this and to learn how to use this library, feel
free to read the
<a href=https://github.com/Mortrack/MLX90614_STM_driver/tree/main/documentation>documentation of this project</a>.

# How to explore the project files.
The following will describe the general purpose of the folders that are located in the current directory address:

- **/'Inc'**:
    - This folder contains the <a href=https://github.com/Mortrack/MLX90614_STM_driver/blob/main/Inc/mlx90614_ir_thermometer_driver.h>header code file for this library</a>.
- **/'Src'**:
    - This folder contains the <a href=https://github.com/Mortrack/MLX90614_STM_driver/blob/main/Src/mlx90614_ir_thermometer_driver.c>source code file for this library</a>.
- **/documentation**:
    - This folder provides the documentation to learn all the details of this library and to know how to use it.

## Future additions planned for this library

This Driver Library is already very complete and professional, but if required and if there are any suggestion you would
like to give, I am completely open to integrate your ideas into this project. Let's make the most awesome, complete and
professional MLX90614 Driver Library together.


# SPECIAL THANKS:

To Mary Broadway (Sr. Inside Sales Responsible) and Karel from the Melexis team for taking the time to help me in understanding their procedure used to change the Slave Address in a MLX90614 EEPROM.
