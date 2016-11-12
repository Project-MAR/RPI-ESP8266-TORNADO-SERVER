#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <string.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define ON      HIGH
#define OFF     LOW

#define TRUE    HIGH
#define FALSE   LOW

int LoadStatus = ON;
char nodeCMD[]= "wb.toggle.l.1";

// Port Define
int relayA = 4;
int relayB = 5;
int AC_Sense = 12;

int AC_Sense_readState = ON;

void toggleLoad(int *LoadStatus) {
  int nextLoadStatus = ON;

  if(*LoadStatus == ON) {
    nextLoadStatus = OFF;
    Serial.printf("Try to OFF: 1\n");
  }
  else {
    nextLoadStatus = ON;
    Serial.printf("Try to ON: 1\n");
  }
  digitalWrite(relayA, HIGH);
  digitalWrite(relayB, LOW);
  delay(200);
  digitalWrite(relayA, LOW);
  digitalWrite(relayB, LOW);
  delay(200);

  AC_Sense_readState = digitalRead(AC_Sense);
  if(AC_Sense_readState != nextLoadStatus) {
      if(*LoadStatus == ON)
        Serial.printf("Try to OFF: 2\n");
      else
        Serial.printf("Try to ON: 2\n");
    
    digitalWrite(relayA, LOW);
    digitalWrite(relayB, HIGH);
    delay(200);
    digitalWrite(relayA, LOW);
    digitalWrite(relayB, LOW);
    delay(200);
  } 

  *LoadStatus = nextLoadStatus;
  if(*LoadStatus)
    Serial.printf("Load ON\n");
  else
    Serial.printf("Load OFF\n");
}

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
                Serial.printf("\n[WS] Receive: %s\n", payload);
                toggleLoad(&LoadStatus);
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

    pinMode(AC_Sense, INPUT);
    delay(500);
    
    // init state
    LoadStatus = digitalRead(AC_Sense);
    if(LoadStatus)
        Serial.printf("Load ON\n");
    else
        Serial.printf("Load OFF\n");

    for(uint8_t t = 4; t > 0; t--) {
       Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
       Serial.flush();
       delay(1000);
    }

    WiFiMulti.addAP("WifiNaam", "qawsEDRF");
    //WiFiMulti.addAP("MIND-WIFI", "87654321");

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
    //webSocket.begin("192.168.1.90", 8880, "/web_ws");
    webSocket.beginSSL("192.168.1.90", 8880, "/web_ws");
    
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent); 
}

void loop() {
    // Web Socket loop
    webSocket.loop();

    AC_Sense_readState = digitalRead(AC_Sense);
    if((AC_Sense_readState != LoadStatus)) {
      // toggle switch state
      toggleLoad(&LoadStatus);
   }
}

