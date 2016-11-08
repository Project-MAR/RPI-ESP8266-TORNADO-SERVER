/*
 * This sketch will query the network for hosts providing the service defined by
 * QUESTION_SERVICE and will parse any replies with the aim of saving port and
 * network address in the hosts array.
 */

#include <ESP8266WiFi.h>
#include <mdns.h>

const char* ssid = "SSID";       // your network SSID (name)
const char* pass = "Pass";       // your network password

#define QUESTION_SERVICE "tornado-server.local"

#define MAX_HOSTS 1

#define HOSTS_SERVICE_NAME  0
#define HOSTS_PORT          1
#define HOSTS_HOST_NAME     2
#define HOSTS_ADDRESS       3
String hosts[MAX_HOSTS][4];  // Array containing information about hosts received over mDNS.


// When an mDNS packet gets parsed this callback gets called once per Query.
// See mdns.h for definition of mdns::Query.
void answerCallback(const mdns::Answer* answer) {

  // A typical A record matches an FQDN to network ipv4 address.
  // eg:
  //   name:    twinkle.local
  //   address: 192.168.192.9
  if (answer->rrtype == MDNS_TYPE_A and strstr(answer->name_buffer, QUESTION_SERVICE) != 0) {
    unsigned int i = 0;
    for (; i < MAX_HOSTS; ++i) {
      if (hosts[i][HOSTS_ADDRESS] == answer->rdata_buffer) {
        // Already in hosts[][].
      }
      else if (hosts[i][HOSTS_ADDRESS] == "") {
        // This hosts[][] entry is still empty.
        hosts[i][HOSTS_ADDRESS]   = answer->rdata_buffer;
        hosts[i][HOSTS_HOST_NAME] = answer->name_buffer;
      }
      else if (hosts[i][HOSTS_ADDRESS] != answer->rdata_buffer){
        // This hosts[][] entry need to update for new IP Address
        break;
      }
    }
    /*
    if (i == MAX_HOSTS) {
      Serial.print(" ** ERROR ** No space in buffer for ");
      Serial.print('"');
      Serial.print(answer->name_buffer);
      Serial.print('"');
      Serial.print("  :  ");
      Serial.print('"');
      Serial.println(answer->rdata_buffer);
      Serial.print('"');
    }
    */
  }

  Serial.println();
  for (int i = 0; i < MAX_HOSTS; ++i) {
    if (hosts[i][HOSTS_ADDRESS] != "") {
      
      Serial.print("HOST_NAME: ");
      Serial.print(hosts[i][HOSTS_HOST_NAME]);
      Serial.print(", HOSTS_ADDRESS: ");
      Serial.print(hosts[i][HOSTS_ADDRESS]);
      Serial.print(", SERVICE_NAME: ");
      Serial.print(hosts[i][HOSTS_SERVICE_NAME]);
      Serial.print(", PORT: ");
      Serial.print(hosts[i][HOSTS_PORT]);
    }
  }
}

// mDNS Callback Function
mdns::MDns my_mdns(NULL, NULL, answerCallback);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // setting up Station AP
  WiFi.begin(ssid, pass);

  // Wait for connect to AP
  Serial.println("");
  Serial.print("[Connecting]");
  Serial.print(ssid);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30) {
      break;
    }
  }
  Serial.println();

  printWifiStatus();

  Serial.println("Connected to wifi");

  // Query for all host information for a paticular service.
  my_mdns.Clear();
  struct mdns::Query query_tornadoServer;
  strncpy(query_tornadoServer.qname_buffer, QUESTION_SERVICE, MAX_MDNS_NAME_LEN);
  query_tornadoServer.qtype = MDNS_TYPE_A;
  query_tornadoServer.qclass = 1;    // "Internet"
  query_tornadoServer.unicast_response = 0;
  my_mdns.AddQuery(query_tornadoServer);
  my_mdns.Send();

}

void loop() {
  my_mdns.Check();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
