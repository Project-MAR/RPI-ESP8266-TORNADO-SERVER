#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <string.h>

int LightStatus = 0;

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
int count = 0;
char Apple[30];
char buffer [30];
char nodeCMD[]= "wb.toggle.l.1";

int relayA = 4;
int relayB = 5;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("Disconnected from web Socket\n");
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to Web Socket\n");
            break;
        
        case WStype_TEXT:
            if(strcmp((const char*)payload, nodeCMD) == 0){
                Serial.printf("[WS] Receive: %s\n", payload);
                
                if(LightStatus){
                  LightStatus = 0;
                  Serial.printf("Light ON\n");
                  digitalWrite(relayA, LOW);
                  digitalWrite(relayB, HIGH);
                  delay(500);
                  digitalWrite(relayA, LOW);
                  digitalWrite(relayB, LOW);
   
                }else{
                  LightStatus = 1;
                  Serial.printf("Light OFF\n");
                  digitalWrite(relayA, HIGH);
                  digitalWrite(relayB, LOW);
                  delay(500);
                  digitalWrite(relayA, LOW);
                  digitalWrite(relayB, LOW);
                }
              }  
            break;
            
        case WStype_BIN:
            Serial.printf("[WS] get binary lenght: %u\n", lenght);
            //hexdump(payload, lenght);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();
    Serial.println();
    Serial.println();

    //IO Setup
    pinMode(relayA, OUTPUT);
    pinMode(relayB, OUTPUT);
    digitalWrite(relayA, LOW);
    digitalWrite(relayB, LOW);
    delay(500);
    
      for(uint8_t t = 4; t > 0; t--) {
          Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
          Serial.flush();
          delay(1000);
      }

    WiFiMulti.addAP("SSID1", "PASS1");
    WiFiMulti.addAP("SSID2", "PASS2");

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

    //webSocket.begin("tornado-server.local/", 8880, "/web_ws");
    webSocket.begin("192.168.1.90", 8880, "/web_ws");
    
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
}
