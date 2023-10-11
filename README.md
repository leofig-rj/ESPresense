# ESPresense

An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) or [ESPresense-companion](https://github.com/ESPresense/ESPresense-companion) for indoor positioning

Forked to create four firmware (`lf-sw-0`, `lf-sw-1`, `lf-sw-2` and `lf-sw-3`) using ESP32-C3 without sensors and with 0, 1, 2 or 3 switchs (input pins associated with output pins).

| Switch No  | Input Pin | Output Pin |
| ---------- | --------- | ---------- |
| #1 | GPIO 5  | GPIO 2 |
| #2 | GPIO 4  | GPIO 19 |
| #3 | GPIO 9  | GPIO 18 |

In firmwares `lf-sw-0` and `lf-sw-1`, Radar can be configured by defining `Radar motion pin` with any number other than -1. GPIO 19 will be used.
All other settings will be ignored.

**Documentation:** https://espresense.com/

**Building:** [building](./BUILDING.md).

**Release Notes:** [changelog](./CHANGELOG.md).
