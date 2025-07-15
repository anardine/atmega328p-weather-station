# High Level Overview of the Aplication

This module works by fetching data in specific configurable timeframes (default to 1 minute) from all wather sensors and sending it to the Master (ATMEGA328-PU).
The master then sends this data to the ESP-01S to be able to post this to a server that collects the data for further usage.
The flash is to store data if the wifi reports failure on connection. The data is then fetched and sent when the wifi resumes working with the correct timeframe that the data was collected.

[!IMPORTANT]
This does not uses the Arduino Framework and it's a direct implementation of the ATMEGA328-PU bare metal. All APIs were created for this prurpose.

#Architecture
This is the architectural diagram of the design:
```

                                                               25Q128FVIQ
                                                               +--------------+
                                                               |              |
                                                        +----->+    Flash     |
 +5V Line                                               |      |              |
    +                        STM32F411RET6              |      +--------------+
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

The data collected will follow this schema after posted to a MySQL database:

```
| id  |  data_type  |   sensor   | value |  unit   |   time_collected    |     time_added      |
|-----|-------------|------------|-------|---------|---------------------|---------------------|
|   0 | temperature | bme280     |  25.3 | celcius | 2025-07-01 00:00:00 | 2025-07-01 00:00:01 |
|   1 | pressure    | bme280     |  1000 | mmHg    | 2025-07-01 00:00:01 | 2025-07-01 00:00:02 |
|   2 | rain        | mh_rain    |     1 | -       | 2025-07-01 00:00:01 | 2025-07-01 00:00:02 |
|   3 | wind        | anemometer |  10.0 | km/h    | 2025-07-01 00:00:01 | 2025-07-01 00:00:02 |
| ... | ...         | ...        |   ... | ...     | ...                 |                     |

```

