/*------------------------------------------------------------------------------
  10/20/2020
  Authors: Cisco • A C R O B O T I C | Edrig | JT | James Trevor | Van Robichaux
  Platforms: ESP32
  Language: C++/Arduino
  File: twitchchat_treat.ino
  ------------------------------------------------------------------------------
  Description:
  Code for controlling relay wired to a treat dispenser. Formerly YouTube video tutorial demonstrating how to use a websocket client to
  get messages from Twitch Chat. The code uses the messages—formatted in a
  specific way—to control a SHARPER IMAGE Mini Automatic Touch-Activated Candy & Snack Dispenser.
  ()
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <time.h>
#include "TwitchCommand.h"
#include "secret.h"
#define treatPin 22
// Initialize network parameters
const char ssid[] = SSID;
const char password[] = PASS;
// Declare websocket client class variable
WebSocketsClient webSocket;
const char twitch_root_ca[] = TWITCH_ROOT_CA;
static_assert(sizeof(twitch_root_ca) > 1, "Configure the trusted root CA for Twitch IRC");
// Parameters for Twitch channel
const char twitch_oauth_token[] = TWITCH_OAUTH_TOKEN;
const char twitch_nick[] = TWITCH_NICK;
const char twitch_channel[] = TWITCH_CHANNEL;
void setup()
{
  Serial.begin(115200);
  pinMode(treatPin, OUTPUT);
  digitalWrite(treatPin, LOW);
  delay(10);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // Certificate validity checks need a synchronized clock.
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  struct tm now;
  while (!getLocalTime(&now)) delay(1000);
  webSocket.beginSslWithCA("irc-ws.chat.twitch.tv", 443, "/", twitch_root_ca, "");
  // Event handler
  webSocket.onEvent(webSocketEvent);
  // Try every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);
}
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  // If the websocket connection is succesful, try to join the IRC server
  case WStype_CONNECTED:
    webSocket.sendTXT("CAP REQ :twitch.tv/tags twitch.tv/commands\r\n");
    webSocket.sendTXT("PASS " + String(twitch_oauth_token) + "\r\n");
    webSocket.sendTXT("NICK " + String(twitch_nick) + "\r\n");
    webSocket.sendTXT("JOIN " + String(twitch_channel) + "\r\n");
    break;
  case WStype_TEXT:
  {
    const std::string frame(reinterpret_cast<const char *>(payload), length);
    size_t start = 0;
    while (start < frame.size()) {
      const auto end = frame.find("\r\n", start);
      const auto line = frame.substr(start, end - start);
      if (line == "PING :tmi.twitch.tv") {
        webSocket.sendTXT("PONG :tmi.twitch.tv\r\n");
      } else if (shouldDispense(line, twitch_channel, TREATMASTER_ADMIN_ID, TREATMASTER_ID)) {
        digitalWrite(treatPin, HIGH);
        delay(300);
        digitalWrite(treatPin, LOW);
      }
      if (end == std::string::npos) break;
      start = end + 2;
    }
    break;
  }
  default:
    break;
  }
}

void loop()
{
  webSocket.loop();
}
