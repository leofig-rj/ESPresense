#ifdef LF_SW

#include "LightSwitch.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace LightSwitch {

int8_t buttonType = INPUT_PULLUP; // pinTypes = {INPUT_PULLUP, INPUT_PULLDOWN, INPUT};
int8_t buttonDetected = LOW;
float buttonTimeout = 0.05;

int8_t switchType = OUTPUT;

#ifdef LF_SW_1
int8_t buttonPin1 = 5;
int8_t lastButtonValue1 = -1;
unsigned long lastButtonMilli1 = 0;

int8_t switchPin1 = 2; 
int8_t switchValueIni1 = HIGH; 
int8_t lastSwitchValue1 = -1;
#endif

#ifdef LF_SW_2
int8_t buttonPin2 = 4;
int8_t lastButtonValue2 = -1;
unsigned long lastButtonMilli2 = 0;

int8_t switchPin2 = 19; 
int8_t switchValueIni2 = LOW; 
int8_t lastSwitchValue2 = -1;
#endif

#ifdef LF_SW_3
//int8_t buttonPin3 = 3;
int8_t buttonPin3 = 9;
int8_t lastButtonValue3 = -1;
unsigned long lastButtonMilli3 = 0;

int8_t switchPin3 = 18; 
int8_t switchValueIni3 = LOW; 
int8_t lastSwitchValue3 = -1;
#endif

bool online;

void Setup() {
#ifdef LF_SW_1
    pinMode(switchPin1, switchType);
    digitalWrite(switchPin1, switchValueIni1);
    pinMode(buttonPin1, buttonType);
#endif
#ifdef LF_SW_2
    pinMode(switchPin2, switchType);
    digitalWrite(switchPin2, switchValueIni2);
    pinMode(buttonPin2, buttonType);
#endif
#ifdef LF_SW_3
    pinMode(switchPin3, switchType);
    digitalWrite(switchPin3, switchValueIni3);
    pinMode(buttonPin3, buttonType);
#endif
}

#ifdef LF_SW_1
bool getState1() {
    return !digitalRead(switchPin1);
}
#endif

#ifdef LF_SW_2
bool getState2() {
    return digitalRead(switchPin2);
}
#endif

#ifdef LF_SW_3
bool getState3() {
    return digitalRead(switchPin3);
}
#endif

#ifdef LF_SW_1
bool setState1(bool state) {
    if (state == getState1())
       return false;
    digitalWrite(switchPin1, !state);
    return true;
}
#endif

#ifdef LF_SW_2
bool setState2(bool state) {
    if (state == getState2())
       return false;
    digitalWrite(switchPin2, state);
    return true;
}
#endif

#ifdef LF_SW_3
bool setState3(bool state) {
    if (state == getState3())
       return false;
    digitalWrite(switchPin3, state);
    return true;
}
#endif

#ifdef LF_SW_1
void buttonAction1(bool buttonOn) {
    if (buttonOn)
        setState1(!getState1());
}
#endif

#ifdef LF_SW_2
void buttonAction2(bool buttonOn) {
    if (buttonOn)
        setState2(!getState2());
}
#endif

#ifdef LF_SW_3
void buttonAction3(bool buttonOn) {
    if (buttonOn)
        setState3(!getState3());
}
#endif

#ifdef LF_SW_1
bool sendState1() {
    char buffer[512];
    DynamicJsonDocument doc(512);
    auto slug = slugify(LIGHT_SWITCH_NAME_1);
    auto state = getState1();
    doc["state"] = state ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    serializeJson(doc, buffer);
    String setTopic = Sprintf("%s/%s", roomsTopic.c_str(), slug.c_str());
    return pub(setTopic.c_str(), 0, true, buffer);
}
#endif

#ifdef LF_SW_2
bool sendState2() {
    char buffer[512];
    DynamicJsonDocument doc(512);
    auto slug = slugify(LIGHT_SWITCH_NAME_2);
    auto state = getState2();
    doc["state"] = state ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    serializeJson(doc, buffer);
    String setTopic = Sprintf("%s/%s", roomsTopic.c_str(), slug.c_str());
    return pub(setTopic.c_str(), 0, true, buffer);
}
#endif

#ifdef LF_SW_3
bool sendState3() {
    char buffer[512];
    DynamicJsonDocument doc(512);
    auto slug = slugify(LIGHT_SWITCH_NAME_3);
    auto state = getState3();
    doc["state"] = state ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    serializeJson(doc, buffer);
    String setTopic = Sprintf("%s/%s", roomsTopic.c_str(), slug.c_str());
    return pub(setTopic.c_str(), 0, true, buffer);
}
#endif

#ifdef LF_SW_1
static void buttonLoop1() {
    if (buttonPin1 < 0) return;
    bool detected = digitalRead(buttonPin1) == buttonDetected;
    if (detected) lastButtonMilli1 = millis();
    unsigned long since = millis() - lastButtonMilli1;
    int buttonValue = (detected || since < (buttonTimeout * 1000)) ? HIGH : LOW;

    if (lastButtonValue1 == buttonValue) return;

    lastButtonValue1 = buttonValue;
    buttonAction1(lastButtonValue1 == HIGH);
}
#endif

#ifdef LF_SW_2
static void buttonLoop2() {
    if (buttonPin2 < 0) return;
    bool detected = digitalRead(buttonPin2) == buttonDetected;
    if (detected) lastButtonMilli2 = millis();
    unsigned long since = millis() - lastButtonMilli2;
    int buttonValue = (detected || since < (buttonTimeout * 1000)) ? HIGH : LOW;

    if (lastButtonValue2 == buttonValue) return;

    lastButtonValue2 = buttonValue;
    buttonAction2(lastButtonValue2 == HIGH);
}
#endif

#ifdef LF_SW_3
static void buttonLoop3() {
    if (buttonPin3 < 0) return;
    bool detected = digitalRead(buttonPin3) == buttonDetected;
    if (detected) lastButtonMilli3 = millis();
    unsigned long since = millis() - lastButtonMilli3;
    int buttonValue = (detected || since < (buttonTimeout * 1000)) ? HIGH : LOW;

    if (lastButtonValue3 == buttonValue) return;

    lastButtonValue3 = buttonValue;
    buttonAction3(lastButtonValue3 == HIGH);
}
#endif

void Loop() {
#ifdef LF_SW_1
    buttonLoop1();
    
    int switch_Value1 = getState1();
    if (lastSwitchValue1 != switch_Value1) {
        lastSwitchValue1 = switch_Value1;
        sendState1();
    }
#endif

#ifdef LF_SW_2
    buttonLoop2();
    
    int switch_Value2 = getState2();
    if (lastSwitchValue2 != switch_Value2) {
        lastSwitchValue2 = switch_Value2;
        sendState2();
    }
#endif

#ifdef LF_SW_3
    buttonLoop3();
    
    int switch_Value3 = getState3();
    if (lastSwitchValue3 != switch_Value3) {
        lastSwitchValue3 = switch_Value3;
        sendState3();
    }
#endif
}

bool SendDiscovery() {
#ifdef LF_SW_1
    if (!sendLightSwitchDiscovery(LIGHT_SWITCH_NAME_1, EC_NONE))
        return false;
#endif

#ifdef LF_SW_2
    if (!sendLightSwitchDiscovery(LIGHT_SWITCH_NAME_2, EC_NONE))
        return false;
#endif

#ifdef LF_SW_3
    if (!sendLightSwitchDiscovery(LIGHT_SWITCH_NAME_3, EC_NONE))
        return false;
#endif

    return true;
}

bool SendOnline() {
    if (online) return true;

#ifdef LF_SW_1
    if (!sendState1()) return false;
#endif

#ifdef LF_SW_2
    if (!sendState2()) return false;
#endif

#ifdef LF_SW_3
    if (!sendState3()) return false;
#endif

    online = true;
    return true;
}

bool Command(String& command, String& pay) {

#ifdef LF_SW_0
    return false;
#endif

#ifdef LF_SW_1
    if (command == "lightswitch1") {
        DynamicJsonDocument root(pay.length() + 100);
        auto err = deserializeJson(root, pay);
        if (err) {
            Serial.printf("LightSwitch1::Command: deserializeJson: %s\r\n", err.c_str());
            return true;
        }
        bool sendNewState = false;
        if (root.containsKey("state"))
            sendNewState = setState1(root["state"] == MQTT_STATE_ON_PAYLOAD);

        if (sendNewState) sendState1();
    }
#endif

#ifdef LF_SW_2
    if (command == "lightswitch2") {
        DynamicJsonDocument root(pay.length() + 100);
        auto err = deserializeJson(root, pay);
        if (err) {
            Serial.printf("LightSwitch2::Command: deserializeJson: %s\r\n", err.c_str());
            return true;
        }
        bool sendNewState = false;
        if (root.containsKey("state"))
            sendNewState = setState2(root["state"] == MQTT_STATE_ON_PAYLOAD);

        if (sendNewState) sendState2();
    }
#endif

#ifdef LF_SW_3
    if (command == "lightswitch3") {
        DynamicJsonDocument root(pay.length() + 100);
        auto err = deserializeJson(root, pay);
        if (err) {
            Serial.printf("LightSwitch3::Command: deserializeJson: %s\r\n", err.c_str());
            return true;
        }
        bool sendNewState = false;
        if (root.containsKey("state"))
            sendNewState = setState3(root["state"] == MQTT_STATE_ON_PAYLOAD);

        if (sendNewState) sendState3();
    }
#endif

    return true;
}

}  // namespace LightSwitch

#endif
