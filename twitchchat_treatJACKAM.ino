/*------------------------------------------------------------------------------
  10/20/2020
  Authors: Cisco • A C R O B O T I C | Edrig | JT
  Platforms: ESP32
  Language: C++/Arduino
  File: twitchchat_treat.ino
  ------------------------------------------------------------------------------
  Description:
  Code for controlling relay wired to a treat dispenser. Formerly YouTube video tutorial demonstrating how to use a websocket client to
  get messages from Twitch Chat. The code uses the messages—formatted in a
  specific way—to control 16 NeoPixel/WS2812B LEDs:
  https://youtu.be/xHSg7IpmLo4
  You can also try out the code live:
  https://www.twitch.tv/acrobotik
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "secret.h"
#define treatPin 22
// Initialize network parameters
const char ssid[] = SSID;
const char password[] = PASS;
// Declare websocket client class variable
WebSocketsClient webSocket;
// Allocate the JSON document
StaticJsonDocument<200> doc;
// Parameters for Twitch channel
const char twitch_oauth_token[] = TWITCH_OAUTH_TOKEN;
const char twitch_nick[] = TWITCH_NICK;
const char twitch_channel[] = TWITCH_CHANNEL;
void setup() {
  Serial.begin(115200);
  pinMode(treatPin, OUTPUT);
  delay(10);
  // Connect to WiFi
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  // Server address, port, and URL path
  webSocket.begin("irc-ws.chat.twitch.tv", 80, "/");
  // Event handler
  webSocket.onEvent(webSocketEvent);
  // Try every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);
}
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    // If the websocket connection is succesful, try to join the IRC server
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to: %s\n", payload);
      webSocket.sendTXT("PASS " + String(twitch_oauth_token) + "\r\n");
      webSocket.sendTXT("NICK " + String(twitch_nick) + "\r\n");
      webSocket.sendTXT("JOIN " + String(twitch_channel) + "\r\n");
      break;
    // If we get a response, print it to Serial
    case WStype_TEXT: {
      Serial.printf("> %s\n", payload);
      String payload_str = String((char*) payload);
      String treatmastername1 = TREATMASTER_ADMIN;
      String treatmastername2 = TREATMASTER;
       // Search for the whitelisted user, the treatmaster
      int treatmaster1 = payload_str.indexOf(":" + treatmastername1 + "!");
      int treatmaster2 = payload_str.indexOf(":" + treatmastername2 + "!");
      // Search for the beginning on the JSON-encoded message (":!treat")
      int treat = payload_str.indexOf(":!treat");
      int master = payload_str.indexOf(":!setmaster");
      int quote_start = master + 12;
      // If the message is addressed to the chat bot
      if(treatmaster1 > -1)
        {if(treat > 0)
         {
        digitalWrite(treatPin, HIGH);
        delay(300);
        digitalWrite(treatPin, LOW);
         }        
         else if(master > -1){
        int quote_end = payload_str.length();
  String master_str = payload_str.substring(quote_start, quote_end);
  treatmastername1 = master_str;
  }
             break;
      }
            if(treatmaster2 > -1)
        {if(treat > 0)
         {
        digitalWrite(treatPin, HIGH);
        delay(300);
        digitalWrite(treatPin, LOW);
         }        
         else if(master > -1){
        int quote_end = payload_str.length();
  String master_str = payload_str.substring(quote_start, quote_end);
  treatmastername1 = master_str;
  }
            break;
      }
      }     
    }
  }
void loop() {
  webSocket.loop();
}
