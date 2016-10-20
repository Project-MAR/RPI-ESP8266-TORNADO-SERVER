#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("Disconnected from web Socket\n");
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to Web Socket\n");

            //send message to server when Connected
            Serial.printf("[WS] Send: Hello from Client\n");
            webSocket.sendTXT("Hello from Client");
            break;
        
        case WStype_TEXT:
            Serial.printf("[WS] Receive: %s\n", payload);
            break;
            
        case WStype_BIN:
            Serial.printf("[WS] get binary lenght: %u\n", lenght);
            hexdump(payload, lenght);
            //send data to server
            //webSocket.sendBIN(payload, lenght);
            break;
    }
}

void setup() {
    // Serial.begin(921600);
    Serial.begin(115200);

    //Serial.setDebugOutput(true);
    Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("MIND-WIFI", "87654321");
    WiFiMulti.addAP("Wifi-Naam", "qawsEDRF");

    Serial.printf("Connecting Wifi...\n");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.printf(".");
        delay(100);
    }
    
    Serial.printf("\n");
    Serial.printf("WiFi connected\n");
    Serial.printf("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("\n");

    //webSocket.beginSocketIO("158.108.166.160", 8880, "/ws");
    webSocket.begin("158.108.166.160", 8880, "/ws");
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    delay(1000);
}
