# RP2350 AHT30 Ethernet MQTT Sensor

This project runs on an RP2350-based board using a W5500 Ethernet controller. It reads temperature data from an AHT20/AHT30 sensor over I²C and publishes the value to Home Assistant via MQTT using the ArduinoHA library. A single NeoPixel LED provides real-time system status feedback.

---

## Features

* Ethernet connectivity via **W5500** (DHCP)
* MQTT integration using **ArduinoHA**
* Temperature sensing with **AHT20 / AHT30**
* Home Assistant–friendly sensor publishing
* NeoPixel status LED for diagnostics

---

## Hardware Used

* RP2350-based microcontroller
* W5500 Ethernet module
* AHT20 or AHT30 temperature/humidity sensor
* Single WS2812 / NeoPixel LED

### Pin Assignments

| Function      | Pin |
| ------------- | --- |
| W5500 CS      | 17  |
| I2C SDA       | 0   |
| I2C SCL       | 1   |
| NeoPixel Data | 25  |

---

## Network Configuration

Update these values to match your network:

* **MAC Address**: Must be unique on your LAN
* **MQTT Broker IP**: Typically your Home Assistant or Mosquitto host
* **MQTT Credentials**: Optional, but supported

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x23, 0x50 };
IPAddress mqttServer(192, 168, 1, 116);
```

---

## MQTT Topics

The device publishes sensor data to the following topics:

* `home/rp2350/status`
* `home/rp2350/temperature`
* `home/rp2350/humidity`

(Only temperature is currently published in the code.)

---

## AHT20 / AHT30 Library Behavior (Important)

This project uses the **AHT20 library by Rob Tillaart**.

### Key Behavior

* `begin()` performs **one initial measurement**
* `getTemperature()` and `getHumidity()` **only return cached values**
* To obtain a new reading, you **must call `read()`** before accessing values

### Correct Read Pattern

```cpp
if (aht.read() == AHT20_OK) {
    float temperature = aht.getTemperature();
}
```

Failing to call `read()` will result in:

* First temperature reading being valid
* All subsequent readings being stale or invalid

This behavior is expected and by design.

---

## LED Status Indicator

The NeoPixel LED provides visual feedback:

| Color / Pattern | Meaning                         |
| --------------- | ------------------------------- |
| Blinking Blue   | Ethernet not configured (no IP) |
| Blinking Yellow | Ethernet OK, MQTT not connected |
| Blinking Purple | MQTT OK, sensor not detected    |
| Solid Green     | All systems operational         |

---

## Main Loop Behavior

1. Ensure Ethernet has a valid DHCP lease
2. Attempt MQTT connection
3. Update LED status based on system state
4. Every 10 seconds:

   * Trigger AHT sensor read
   * Convert temperature to Fahrenheit
   * Publish value via ArduinoHA

---

## Notes & Recommendations

* Do not poll the AHT sensor too frequently (10s interval is safe)
* I²C clock is set to 100 kHz for reliability
* The system safely retries DHCP and MQTT connections

---

## Known Limitations

* Humidity is not yet published (sensor supports it)
* No MQTT Last Will or availability topic configured
* Blocking timing is minimal but still present in setup

---

## License

This project is provided as-is for personal or educational use. Modify freely.

---

## Summary

This firmware provides a stable Ethernet-based temperature sensor node for Home Assistant, with clear diagnostics and correct handling of AHT20/AHT30 sensor behavior.
