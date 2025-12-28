# ☀️🌦️ ATMEGA328P Weather Station ⛈️❄️
Fetch data directly from Local Weather Sensors and send to a Webserver and Database in specific timeframes. Use the way you like after.

![9c313f0f28ba4589b9f9a578aeba3b96](https://github.com/user-attachments/assets/b0a6ab17-f3fc-470f-b193-6b5bbbe376d8)

## Table of Contents
- [Project Details](#project-details)
    - [High Level Overview of the Aplication](#high-level-overview-of-the-aplication)
    - [Architecture of the Module](#architecture-of-the-module)
    - [Hardware](#hardware)
    - [About the Webserver](#about-the-webserver)
- [Instructions to Flash and Use](#instructions-to-use-this-successfully)

## Project Details
### High Level Overview of the Aplication

This module works by fetching data in specific configurable timeframes (default to 1 minute) from all weather sensors and sends it to the Master (ATMEGA328-PU).
The master then sends this data to the ESP-01S which posts the data on a web server for further use.

> ⚠️ **IMPORTANT**
> 
> This does not uses the Arduino Framework and it's a direct implementation of the ATMEGA328-PU bare metal. All APIs were created for this prurpose, and flashing as arduino will not work

### Architecture of the Module
This is the architectural diagram of the design:
```

                                                               25Q128FVIQ
                                                               +----------------+
                                                               |     Flash      |
                                                        +----->+ (Disabled by   |
 +5V Line                                               |      |    Default)    |
    +                        ATMEGA328P-U               |      +----------------+
    |      +-----------+     +--------------------+     |      
    |      |           |     |                    |     |      BME280
    +----->+ Power Reg +---->+ 3V3           SPI1 +<----+      +--------------+
           |           |     |                    |            |              |
           +-----------+     |                    |            |  Temp, Hum,  |
                         +---> HSE           I2C1 +<---------->+  Pressure    |
              +------+   |   |                    |            |              |
              | 8MHz +---+   |             USART1 +<-----+     +--------------+
              +------+       |  IO     IO         |      |
                             +--------------------+      |     ESP01S
                                ^       ^                |     +--------------+       +------------+
                                |       +------+         |     |              |  POST |            |
                                |              |         +---->+  Wifi Module +-------> Web Server |
                                |              |               |              |       |            |
                          +------------+     +------------+    +--------------+       +------^-----+
                          |            |     |            |                                  |
                          |    Rain    |     | Anemometer*|                                  |
                          |            |     |            |                               +--v--+
                          +------------+     +------------+                               | db  |
                                             * Not Connected                              +-----+
                                             Needs 12V Dedicated


```


### Hardware

The module is composed of:

- **Microcontroller:** ATMEGA328P-U
- **Temperature Sensor:** Bosch BME280
- **Pressure Sensor:** Bosch BME280
- **Humidity Sensor:** Bosch BME280
- **Wind Sensor:** Not present on the original project but can be connected on the Wind Sensor GPIO
- **Rain Sensor:** Generic Arduino MH Rain Sensor
- **Wifi Module:** ESP01-S
- **Flash:** Winbound 25Q128FVIQ

The hardware, gerber, fabrication files and assembly pictures are included at: https://oshwlab.com/anardine.ef/weather-station

### About the Webserver

The interfacing is made through GET requests sent after an interrupt from the ATMega to the ESP01-S. This `GET` request is sent to `api.php` with the sensor data to be saved to an external database.
The credentials and details of the host, port and database should be defined in the `include/config.h`.

The data collected will follow this schema after posted to a MySQL database. You can find the `CREATE TABLE` SQL statement at the `create_table.sql` file.

```
| id  |  data_type  |   sensor   | value |  unit   |   device     |     time_added      |
|-----|-------------|------------|-------|---------|--------------|---------------------|
|   0 | temperature | bme280     |  25.3 | celcius | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   1 | pressure    | bme280     |  1000 | mBar    | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   2 | rain        | mh_rain    |     1 | bool    | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   3 | wind        | anemometer |  10.0 | km/h    | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   4 | humidity    | bme280     |  60.0 | percent | HOME_CENTRAL | 2025-07-01 00:00:00 |
| ... | ...         | ...        |   ... | ...     | ...          |                     |

```

## Instructions to Use this Successfully
1. **Do not solder the flash, ATMEGA328P and sensors to the board directly.** There are some bugs when uploading the code using an USBasp that crashes the system. Please upload the code to the board with the sensors removed.
2. **Do not use the `USE_FLASH` on `main.c` yet** as this is still under implementation.
3. This runs using the PlatformIO Environment. Please install the PlatformIO plugin on your IDE and open this project as a PlatformIO project. 
4. Most of the details that the user has to provide are defined at `include/config.h`. Please set define there your Wifi SSID, password and the domain this module will send data to. You can also give this board a name, which will be the name saved as the `device` on your database.
5. Create a database on your webserver (MySQL preferentially and run the create table statement described at `create_table.sql`)
6. Insert all the database details that you created at `webserver/api.php` and `webserver/fetch_data.php`
7. Upload `webserver/api.php`, `webserver/fetch_data.php` and `webserver/weather_dashboard.html` on the root of your domain.
8. Check if all sensors are removed and Connect the USBasp to the SPI interface of the board. **Please check the pinnout (shown below) correctly and use the jumper for 3.3V instead of 5V on the USBAsp** 
```
SPI Interface
RST  SCK  MISO
┌┌──┐┌──┐┌──┐┐
│└──┘└──┘└──┘│|
│┌──┐┌──┐┌──┐│
└└──┘└──┘└──┘┘
GND  MOSI 3V3
```
9. Run the `Set Fuses` from PlatformIO. This will set the necessary fuses for to use the 8MHz clock.
10. Run the `Upload` from PlatformIO. This should upload the code to the board. 
11. Insert all sensors 
12. Power the board 
13. Check if the data is being collected within the timebox defined and check for any error traces reported on the dashboard footer
