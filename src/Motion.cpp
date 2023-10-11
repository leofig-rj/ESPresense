#include "Motion.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "GUI.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Motion {
int8_t lastMotionValue = -1;

int8_t pirType, pirPin, pirDetected = -1;
float pirTimeout = 0;
int8_t lastPirValue = -1;
unsigned long lastPirMilli = 0;

int8_t radarType, radarPin, radarDetected = -1;
float radarTimeout = 0;
int8_t lastRadarValue = -1;
unsigned long lastRadarMilli = 0;
bool online;

#ifdef LF_SW_0
int8_t radarTypeWiLight = INPUT_PULLDOWN;
int8_t radarPinWiLight = 19;
#endif

#ifdef LF_SW_1
int8_t radarTypeWiLight = INPUT_PULLDOWN;
int8_t radarPinWiLight = 19;
#endif

void Setup() {
#ifdef LF_SW_0
    if (radarPin >= 0) pinMode(radarPinWiLight, radarTypeWiLight);
#endif
#ifdef LF_SW_1
    if (radarPin >= 0) pinMode(radarPinWiLight, radarTypeWiLight);
#endif
#ifndef LF_SW
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (pirPin >= 0) pinMode(pirPin, pinTypes[pirType]);
    if (radarPin >= 0) pinMode(radarPin, pinTypes[radarType]);
#endif
}

void ConnectToWifi() {
#ifdef LF_SW_0
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    radarType = AsyncWiFiSettings.dropdown("radar_type", pinTypes, 0, "Radar motion pin type");
    radarPin = AsyncWiFiSettings.integer("radar_pin", -1, "Radar motion pin (-1 for disable)");
    radarTimeout = AsyncWiFiSettings.floating("radar_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Radar motion timeout (in seconds)");
    radarDetected = HIGH;
#endif
#ifdef LF_SW_1
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    radarType = AsyncWiFiSettings.dropdown("radar_type", pinTypes, 0, "Radar motion pin type");
    radarPin = AsyncWiFiSettings.integer("radar_pin", -1, "Radar motion pin (-1 for disable)");
    radarTimeout = AsyncWiFiSettings.floating("radar_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Radar motion timeout (in seconds)");
    radarDetected = HIGH;
#endif
#ifndef LF_SW
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    pirType = AsyncWiFiSettings.dropdown("pir_type", pinTypes, 0, "PIR motion pin type");
    pirPin = AsyncWiFiSettings.integer("pir_pin", -1, "PIR motion pin (-1 for disable)");
    pirTimeout = AsyncWiFiSettings.floating("pir_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "PIR motion timeout (in seconds)");
    pirDetected = pirType & 0x01 ? LOW : HIGH;

    radarType = AsyncWiFiSettings.dropdown("radar_type", pinTypes, 0, "Radar motion pin type");
    radarPin = AsyncWiFiSettings.integer("radar_pin", -1, "Radar motion pin (-1 for disable)");
    radarTimeout = AsyncWiFiSettings.floating("radar_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Radar motion timeout (in seconds)");
    radarDetected = radarType & 0x01 ? LOW : HIGH;
#endif
}

void SerialReport() {
#ifdef LF_SW_0
    Serial.print("Radar Sensor: ");
    Serial.println(radarPin >= 0 ? "enabled" : "disabled");
#endif
#ifdef LF_SW_1
    Serial.print("Radar Sensor: ");
    Serial.println(radarPin >= 0 ? "enabled" : "disabled");
#endif
#ifndef LF_SW
    Serial.print("PIR Sensor:   ");
    Serial.println(pirPin >= 0 ? "enabled" : "disabled");
    Serial.print("Radar Sensor: ");
    Serial.println(radarPin >= 0 ? "enabled" : "disabled");
#endif
}

#ifndef LF_SW
static void pirLoop() {
    if (pirPin < 0) return;
    bool detected = digitalRead(pirPin) == pirDetected;
    if (detected) lastPirMilli = millis();
    unsigned long since = millis() - lastPirMilli;
    int pirValue = (detected || since < (pirTimeout * 1000)) ? HIGH : LOW;

    if (lastPirValue == pirValue) return;
    pub((roomsTopic + "/pir").c_str(), 0, true, pirValue == HIGH ? "ON" : "OFF");
    lastPirValue = pirValue;
}
#endif

static void radarLoop() {
    if (radarPin < 0) return;
#ifdef LF_SW_0
    bool detected = digitalRead(radarPinWiLight) == radarDetected;
    if (detected) lastRadarMilli = millis();
    unsigned long since = millis() - lastRadarMilli;
    int radarValue = (detected || since < (radarTimeout * 1000)) ? HIGH : LOW;

    if (lastRadarValue == radarValue) return;
    pub((roomsTopic + "/radar").c_str(), 0, true, radarValue == HIGH ? "ON" : "OFF");
    lastRadarValue = radarValue;
#endif
#ifdef LF_SW_1
    bool detected = digitalRead(radarPinWiLight) == radarDetected;
    if (detected) lastRadarMilli = millis();
    unsigned long since = millis() - lastRadarMilli;
    int radarValue = (detected || since < (radarTimeout * 1000)) ? HIGH : LOW;

    if (lastRadarValue == radarValue) return;
    pub((roomsTopic + "/radar").c_str(), 0, true, radarValue == HIGH ? "ON" : "OFF");
    lastRadarValue = radarValue;
#endif
#ifndef LF_SW
    bool detected = digitalRead(radarPin) == radarDetected;

    if (detected) lastRadarMilli = millis();
    unsigned long since = millis() - lastRadarMilli;
    int radarValue = (detected || since < (radarTimeout * 1000)) ? HIGH : LOW;

    if (lastRadarValue == radarValue) return;
    pub((roomsTopic + "/radar").c_str(), 0, true, radarValue == HIGH ? "ON" : "OFF");
    lastRadarValue = radarValue;
#endif
}

void Loop() {
#ifdef LF_SW_0
    radarLoop();
    if (radarPin < 0) return;
    int motionValue = (lastRadarValue == HIGH) ? HIGH : LOW;
    if (lastMotionValue == motionValue) return;
    pub((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
    lastMotionValue = motionValue;
#endif
#ifdef LF_SW_1
    radarLoop();
    if (radarPin < 0) return;
    int motionValue = (lastRadarValue == HIGH) ? HIGH : LOW;
    if (lastMotionValue == motionValue) return;
    pub((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
    lastMotionValue = motionValue;
#endif
#ifndef LF_SW
    pirLoop();
    radarLoop();
    int motionValue = (lastRadarValue == HIGH || lastPirValue == HIGH) ? HIGH : LOW;
    if (lastMotionValue == motionValue) return;
    GUI::Motion(lastPirValue == HIGH, lastRadarValue == HIGH);
    pub((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
    lastMotionValue = motionValue;
#endif
}

bool SendDiscovery() {
#ifdef LF_SW_0
    if (radarPin < 0) return true;
    if (!sendNumberDiscovery("Radar Timeout", EC_CONFIG)) return false;
    return sendBinarySensorDiscovery("Motion", EC_NONE, "motion");
#endif
#ifdef LF_SW_1
    if (radarPin < 0) return true;
    if (!sendNumberDiscovery("Radar Timeout", EC_CONFIG)) return false;
    return sendBinarySensorDiscovery("Motion", EC_NONE, "motion");
#endif
#ifdef LF_SW_2
    return true;
#endif
#ifndef LF_SW
    if (pirPin < 0 && radarPin < 0) return true;

    if (pirPin >= 0)
        if (!sendNumberDiscovery("Pir Timeout", EC_CONFIG)) return false;
    if (radarPin >= 0)
        if (!sendNumberDiscovery("Radar Timeout", EC_CONFIG)) return false;
    return sendBinarySensorDiscovery("Motion", EC_NONE, "motion");
#endif
}

bool Command(String& command, String& pay) {
#ifdef LF_SW_0
    if (command == "radar_timeout") {
        radarTimeout = pay.toInt();
        spurt("/radar_timeout", pay);
    } else
        return false;
    return true;
#endif
#ifdef LF_SW_1
    if (command == "radar_timeout") {
        radarTimeout = pay.toInt();
        spurt("/radar_timeout", pay);
    } else
        return false;
    return true;
#endif
#ifdef LF_SW_2
    return false;
#endif
#ifndef LF_SW
    if (command == "pir_timeout") {
        pirTimeout = pay.toInt();
        spurt("/pir_timeout", pay);
    } else if (command == "radar_timeout") {
        radarTimeout = pay.toInt();
        spurt("/radar_timeout", pay);
    } else
        return false;
    return true;
#endif
}

bool SendOnline() {
#ifdef LF_SW_0
    if (online) return true;
    if (!pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str())) return false;
    online = true;
    return true;
#endif
#ifdef LF_SW_1
    if (online) return true;
    if (!pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str())) return false;
    online = true;
    return true;
#endif
#ifdef LF_SW_2
    return true;
#endif
#ifndef LF_SW
    if (online) return true;
    if (!pub((roomsTopic + "/pir_timeout").c_str(), 0, true, String(pirTimeout).c_str())) return false;
    if (!pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str())) return false;
    online = true;
    return true;
#endif
}
}  // namespace Motion
