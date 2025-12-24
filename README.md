
# Random ESPHome Scripts

A collection of small, self-contained **ESPHome YAML examples and experiments**. This repository is intended as a personal toolbox and reference for building ESPHome-based devices that integrate with Home Assistant.

The configs here range from quick tests to reusable patterns and are not guaranteed to be production-ready.

---

## What This Repo Is

* A grab bag of **ESPHome YAML snippets and full device configs**
* Experiments, prototypes, and proofs of concept
* A reference for solving common ESPHome problems
* A place to stash working examples before they’re reused elsewhere

---

## What This Repo Is NOT

* A polished library
* A single cohesive project
* Guaranteed to follow best practices everywhere
* Guaranteed to be stable across ESPHome releases

Use at your own risk 😄

---

## Typical Contents

You may find configurations for:

* Sensors (temperature, humidity, pressure, light, motion)
* Binary sensors (buttons, GPIO, PIR)
* Relays, switches, and outputs
* Displays (OLED, TFT, ePaper)
* Status LEDs (NeoPixel, RGB, single-color)
* Ethernet and Wi-Fi examples
* MQTT-based devices
* Debug and logging helpers

Each YAML file is generally intended to stand alone.

---

## Usage

1. Copy the YAML file you’re interested in
2. Adjust the following to match your setup:

   * `name:`
   * `device_name:`
   * Pins (GPIOs)
   * Network configuration (Wi-Fi / Ethernet)
3. Add secrets to your `secrets.yaml` if required
4. Compile and flash using ESPHome

---

## ESPHome Version Compatibility

Configs are written against whatever ESPHome version was current at the time.

If something fails:

* Check breaking changes in ESPHome release notes
* Validate indentation and schema updates
* Expect older examples to need small fixes

---

## File Naming Conventions

While not strictly enforced, most files follow patterns like:

* `device-name.yaml`
* `board_feature_test.yaml`
* `sensor_type_example.yaml`

Comments inside each file usually explain intent and wiring.

---

## Secrets & Credentials

This repository **does not include secrets**.

Expected secrets (when used):

```yaml
wifi_ssid: "your_ssid"
wifi_password: "your_password"
api_password: "your_api_password"
ot a real value
```

Create a `secrets.yaml` file locally and do not commit it.

---

## Troubleshooting

If a config doesn’t work:

* Double-check GPIO assignments
* Verify your board definition
* Enable verbose logging:

```yaml
logger:
  level: DEBUG
```

* Compare against ESPHome docs for the component in use

---

## Contributions

This is primarily a personal repo, but:

* Suggestions are welcome
* Improvements are welcome
* Forking is encouraged

---

## License

MIT License (unless otherwise stated in individual files).

---

## Disclaimer

These configurations are provided as-is.

They may:

* Break
* Be incomplete
* Use experimental features

If it works — great.
If it doesn’t — check the logs 😉

---

## TL;DR

A messy but useful collection of ESPHome examples for Home Assistant tinkerers.
