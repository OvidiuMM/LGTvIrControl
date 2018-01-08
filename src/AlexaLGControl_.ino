/* IRremoteESP8266: Controlling LG tv with IRsend.
 * Latest version: https://github.com/OvidiuMM/LGTvIrControl
 * Version 1.0 April, 2017
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to ESP8266 pin 4 (D2).
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/markszabo/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include "WemoSwitch.h"
#include "WemoManager.h"
#include "CallbackFunction.h"

#define LED D0 
//on/off callbacks
void tvOn();
void tvOff();

IRsend irsend(D2);  // An IR LED is controlled by GPIO pin 4 (D2)

uint16_t rawData[67] = {8950, 4400, 550, 550, 600, 500, 600, 1650, 550, 550, 600, 500, 600, 550, 600, 500, 600, 550, 550, 1650, 600, 1600, 600, 550, 550, 1650, 600, 1600, 600, 1600, 600, 1650, 600, 1600, 600, 500, 600, 550, 600, 500, 600, 1600, 600, 550, 600, 500, 600, 550, 600, 500, 600, 1600, 600, 1650, 600, 1600, 600, 500, 600, 1650, 550, 1650, 600, 1600, 600, 1600, 600};
const unsigned long LG_POWER = 0x20DF10EF;

const char* ssid = "******";
const char* password = "*********";
const String deviceName = "SalonTV";


WemoManager wemoManager;
WemoSwitch *light = NULL;

void setup() {
  pinMode(LED, OUTPUT);
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  //connect to wifi
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  light_led(1);
  //connect to Alexa API
   wemoManager.begin();
  // Format: Alexa invocation name, local port no, on callback, off callback
  light = new WemoSwitch(deviceName, 80, tvOn, tvOff);
  wemoManager.addDevice(*light);
  light_led(5);
}
void light_led(int count){
  Serial.println("Led on");
  if (count > 1){
  for( int i = count; i <= 0; i--){
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage HIGH
    delay(500);
    digitalWrite(LED, HIGH);
    }
  }
  else {
      digitalWrite(LED, LOW);
      delay(2000);
      digitalWrite(LED, HIGH);
    }
  }
void loop() {
  wemoManager.serverLoop();
}

void tvOn() {
 
    Serial.print("Switch tv on/off ...");
    irsend.sendNEC(LG_POWER, 32);
  delay(2000);
  Serial.println("a rawData capture from IRrecvDumpV2");
  irsend.sendRaw(rawData, 67, 38);  // Send a raw data capture at 38kHz.
   light_led(1);
  delay(2000);
}

void tvOff() {
     tvOn();
}
