#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <PubSubClient.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// Include the UI lib
#include "OLEDDisplayUi.h"

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

WiFiClient espClient;
PubSubClient client(espClient);

const char* host = "esp8266-webupdate";
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
int sensor = 13;  // Digital pin D7

//initial state
long state = -1;
// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D2, D1);

OLEDDisplayUi ui ( &display );
int count = 0;
void setup(void) {

        Serial.begin(115200);
        Serial.println();
        Serial.println("Booting Sketch...");
        // Initialising the UI will init the display too.

        WiFi.mode(WIFI_AP_STA);
        WiFi.begin(ssid, password);
        pinMode(sensor, INPUT); // declare sensor as input
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
                WiFi.begin(ssid, password);
                Serial.println("WiFi failed, retrying.");
                delay(1000);
        }

        MDNS.begin(host);

        httpUpdater.setup(&httpServer);
        httpServer.begin();

        MDNS.addService("http", "tcp", 80);
        Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
        boolean init = display.init();
        if(!init)
          Serial.printf("Error monitor init\n");
        display.flipScreenVertically();
       display.setFont(ArialMT_Plain_10);

}

void loop(void) {
  // clear the display
display.clear();
// draw the current demo method
        httpServer.handleClient();
        MDNS.update();

        long newState = digitalRead(sensor);
        if(newState == HIGH) {
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
                display.drawString(130, 30, "Motion detected!");
                delay(1000);
        }
        else {
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
                display.drawString(130, 30, "Motion absent!");
                delay(1000);
        }

        if(newState != state) {

                //TODO SEND
                state = newState;
                  display.setTextAlignment(TEXT_ALIGN_RIGHT);
                display.drawString(130, 40, "Send State!");
        }
        display.setTextAlignment(TEXT_ALIGN_RIGHT);

  // write the buffer to the display
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(30, 10, String(count++));
display.display();
delay(500);

}
