#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <string.h>
#include "PasswordList.h"

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define Relay_ON    1
#define Relay_OFF   0

#define Switch_ON   0
#define Switch_OFF  1

#define ON      HIGH
#define OFF     LOW

int LoadStatus  = ON;
int switchState = ON;

char SocketServer[] = "192.168.1.24";
char SocketURL[]    = "/web_ws";
int  SocketPort     = 8880;
char nodeCMD[]      = "wb.toggle.l.1";

/* ---------- IO Define ----------------*/
const int LED1_PIN = 5;
const int LED2_PIN = 4;
const int LED3_PIN = 0;

const int RelayA_PIN = 2;
const int RelayC_PIN = 14;

const int Switch_PIN = 10;
/*---------------------------------------*/

int RelayStatus = 0;

int lastSwitchState    = 0;
int currentSwitchState = 0;

void turnON()
{
  digitalWrite(RelayA_PIN, HIGH);
  digitalWrite(RelayC_PIN, LOW);
  delay(50);
  digitalWrite(RelayA_PIN, LOW);
  digitalWrite(RelayC_PIN, LOW);
  
  //digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  RelayStatus = Relay_ON;
  //Blynk.virtualWrite(V0, HIGH);
}

void turnOFF()
{
  digitalWrite(RelayA_PIN, LOW);
  digitalWrite(RelayC_PIN, HIGH);
  delay(50);
  digitalWrite(RelayA_PIN, LOW);
  digitalWrite(RelayC_PIN, LOW);
  
  //digitalWrite(LED3_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  RelayStatus = Relay_OFF;
  //Blynk.virtualWrite(V0, LOW);
}

void togglRelay()
{
  if(RelayStatus == Relay_ON)
  {
    turnOFF();
    //Blynk.virtualWrite(V0, LOW);
    Serial.println(">>> Relay: OFF");    
  }
  else
  {
    turnON();
    //Blynk.virtualWrite(V0, HIGH);
    Serial.println(">>> Relay: ON");    
  }
  Serial.println("");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) 
{
    //digitalWrite(LED3_PIN, HIGH);
    switch(type) 
    {
        case WStype_DISCONNECTED:
            Serial.printf("Disconnected from web Socket\n");
            digitalWrite(LED3_PIN, HIGH);
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to Web Socket\n");
            digitalWrite(LED3_PIN, LOW);
            break;
        
        case WStype_TEXT:
            if(strcmp((const char*)payload, nodeCMD) == 0){
                Serial.printf("[WS] Receive: %s\n", payload);
                togglRelay();
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
    pinMode(RelayA_PIN, OUTPUT);
    pinMode(RelayC_PIN, OUTPUT);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    digitalWrite(LED3_PIN, HIGH);

    pinMode(Switch_PIN, INPUT);
    currentSwitchState = digitalRead(Switch_PIN);
    lastSwitchState = currentSwitchState;

    Serial.println();
    if(lastSwitchState == Switch_ON)
    {
      Serial.println("\n>>>Init: Switch is at ON\n");
      RelayStatus = Relay_ON;
      turnON();
    }
    else
    {
      Serial.println("\n>>>Init: Switch is at OFF\n");
      RelayStatus = Relay_OFF;
      turnOFF();
    }  

    for(uint8_t t = 4; t > 0; t--) 
    {
      Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
    }
    Serial.println();

    //Serial.printf("\n");
    for(int i = 0; i < numPassWordList; i++)
    {
         WiFiMulti.addAP(APName[i], APPassword[i]); 
    }

    Serial.printf("Connecting Wifi...\n");
    
    while(WiFiMulti.run() != WL_CONNECTED) 
    {
        Serial.printf(".");
        delay(100);
    }
    
    Serial.printf("WiFi connected\n");
    Serial.printf("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.printf("Connect to Web Socket Server IP: %s:%d%s\n",SocketServer, SocketPort, SocketURL);
    //webSocket.begin("tornado-server.local/", 8880, "/web_ws");
    webSocket.begin(SocketServer, SocketPort, SocketURL);
    //webSocket.beginSSL("192.168.1.90", 8880, "/web_ws");
    
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent); 
    Serial.printf("\n");
}

unsigned int count = 0;
unsigned char normalState = 0;

void BlinkNormalStatus(){  
  count++;
  if(count >= 60000)
  {
     count = 0;
     if(normalState == 1)
     {
        normalState = 0;
        digitalWrite(LED1_PIN, LOW);   
     }
     else
     {
        normalState = 1;
        digitalWrite(LED1_PIN, HIGH);
     } 

     if(webSocket.sendPing())
    {
      Serial.printf("Socket is normal\n");
    }
    else
    {
      digitalWrite(LED3_PIN, HIGH);
      Serial.printf("Socket Disappear!!!\n");
    } 
  }
}

void loop() 
{
    // Web Socket loop
    webSocket.loop();
    BlinkNormalStatus();
    //digitalWrite(LED3_PIN, LOW);
    
    currentSwitchState = digitalRead(Switch_PIN);
    if (currentSwitchState != lastSwitchState)
    {
      if (currentSwitchState == Switch_ON)
      {
         Serial.println("[Manual Event] : Switch is ON");
         //turnON();
      }
      else
      {
         Serial.println("[Manual Event] : Switch is OFF");
         //turnOFF();
      }
      lastSwitchState = currentSwitchState;
      togglRelay();
    }

}

