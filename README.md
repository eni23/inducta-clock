# inducta-clock

Docs, hardware and software for running `Type 5 BT600` based slave clocks.


## Electrical details


### Coil connection to 12V

According to the [original documentation](./doc/BT630_637-operation-instructions.pdf) the coil can be run with DC if a shunt is connected. For 12V the shunt needs to be connected like this:

![](./doc/coil-connection.png)![](./doc/coil-connection-photo.png)

To switch minutes on the clock revert polarity for every step and pulse at least 160ms.


### Arduino circuit

BOM:
* Arduino Pro Micro 5V Atmega32U4
* L9110H H bridge
* 5V to 12V step up module
* Encoder with button
* DS3131 RTC or DS1307 RTC module

![](./doc/schematics.png)


## Firmware

[Platformio](https://platformio.org) is needed to compile and upload firmware. Install the following libraries:

```
platformio lib install 275
platformio lib install 1371
```

Set `RTC_MODEL` in `src/config.h`. Use the following values:
```
1 = DS1307
2 = DS3231
3 = DS3232
```

Build the firmware like this:
```
make
```

Upload it like this:
```
make upload
```
