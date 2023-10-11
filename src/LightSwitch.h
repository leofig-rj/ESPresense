#pragma once

#ifdef LF_SW

#include <Arduino.h>

#define MQTT_STATE_ON_PAYLOAD   "ON"
#define MQTT_STATE_OFF_PAYLOAD  "OFF"
#ifdef LF_SW_1
const String LIGHT_SWITCH_NAME_1  = "LightSwitch1";
#endif
#ifdef LF_SW_2
const String LIGHT_SWITCH_NAME_2  = "LightSwitch2";
#endif
#ifdef LF_SW_3
const String LIGHT_SWITCH_NAME_3  = "LightSwitch3";
#endif

namespace LightSwitch {
void Setup();
void Loop();
bool SendDiscovery();
bool SendOnline();
bool Command(String& command, String& pay);
}  // namespace LightSwitch

#endif
