# E131-MQTT-RGBW-Lights

This repository is to accompany my RGBW E131 MQTT Firmware Video:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/r5gju3l6AFQ/0.jpg)](https://www.youtube.com/watch?v=r5gju3l6AFQ)

Control RGBW Lights via E131 (xlights or vixen)
## These lights have firmware written specifically for them:

Zemismart 6" RGBW Downlight: https://amzn.to/34MiRF7
Firmware: https://github.com/thehookup/E131-MQTT-RGBW-Lights/blob/master/E131_MQTT_Zemismart_CONFIGURE.ino


Lohas RGB-CCT Bulb: https://amzn.to/33GPkv2
Firmware: https://github.com/thehookup/E131-MQTT-RGBW-Lights/blob/master/E131RGBW_MQTT_Lohas_CONFIGURE.ino


Novostella RGB-CCT Flood Lights: https://amzn.to/2rV1zak
Firmware: https://github.com/thehookup/E131-MQTT-RGBW-Lights/blob/master/E131RGBW_MQTT_Novostella.ino


Firmware is uploaded for the Sonoff B1, I highly recommend using the Lohas instead, they are cheaper and have better white and color performance.

## Use These upload settings:

[![IMAGE ALT TEXT HERE](https://github.com/thehookup/E131-MQTT-RGBW-Lights/blob/master/upload_settings.JPG?raw=true)](https://github.com/thehookup/E131-MQTT-RGBW-Lights/blob/master/upload_settings.JPG?raw=true)

```yaml
  - platform: mqtt
    name: "RGBW E131 Firmware"
    command_topic: "[ClientID]/power"
    state_topic: "[ClientID]/powerState"
    brightness_command_topic: "[ClientID]/brightness"
    brightness_state_topic: "[ClientID]/brightnessState"
    rgb_command_topic: "[ClientID]/color"
    rgb_state_topic: "[ClientID]/colorState"
    white_value_command_topic: "[ClientID]/white"
    white_value_state_topic: "[ClientID]/whiteState"
    white_value_scale: 255
    brightness_scale: 255
    effect_command_topic: "[ClientID]/effect"
    effect_state_topic: "[ClientID]/effectState"
    effect_list:
      - MQTT
      - E131
    retain: true
```

Add the following lines of YAML if you are using a file with WW/CW control

```yaml
  color_temp_command_topic: "[ClientID]/color_temp"
  color_temp_state_topic: "[ClientID]/color_tempState"
```
