# Deskstate

A simple example of using direct USB control from python to an arduino.

# Why

TBD

# Usage

## Webserver
`python3 deskstate.py` -> shows up at localhost:7100

## Flashing Arduino Code

```
    arduino-cli compile -b adafruit:samd:adafruit_circuitplayground_m0:usbstack=tinyusb -u -p PORT
```

Dependencies
- Adafruit Circuit Playground (both the device and the library)
- TinyUSB
- ArduinoJson