# High Level Overview of the Aplication

This module works by fetching data in specific configurable timeframes (default to 1 minute) from all weather sensors and sends it to the Master (ATMEGA328-PU).
The master then sends this data to the ESP-01S which posts the data on a web server for further use.

[!IMPORTANT]
This does not uses the Arduino Framework and it's a direct implementation of the ATMEGA328-PU bare metal. All APIs were created for this prurpose.

## Architecture of the Module
This is the architectural diagram of the design:
```

                                                               25Q128FVIQ
                                                               +--------------+
                                                               |              |
                                                        +----->+    Flash     |
 +5V Line                                               |      |              |
    +                        ATMEGA328P-U              |      +--------------+
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


## Hardware

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

## About the Webserver

The interfacing is made through POST requests sent after an interrupt from the ATMega to the ESP01-S. This `POST` request is sent to `api.php` with the sensor data to be saved to an external database.
The credentials and details of the host, port and database should be defined in the `include/config.h`.

The data collected will follow this schema after posted to a MySQL database. You can find the `CREATE TABLE` SQL statement at the `create_table.sql` file.

```
| id  |  data_type  |   sensor   | value |  unit   |   device     |     time_added      |
|-----|-------------|------------|-------|---------|--------------|---------------------|
|   0 | temperature | bme280     |  25.3 | celcius | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   1 | pressure    | bme280     |  1000 | mmHg    | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   2 | rain        | mh_rain    |     1 | -       | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   3 | wind        | anemometer |  10.0 | km/h    | HOME_CENTRAL | 2025-07-01 00:00:00 |
|   4 | humidity    | bme280     |  60.0 | percent | HOME_CENTRAL | 2025-07-01 00:00:00 |
| ... | ...         | ...        |   ... | ...     | ...          |                     |

```

