#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <string>


#define MQTT_SERVER "192.168.xx.xx"
const char* ssid = "**********";
const char* password = "*********";
int GPIO3 = 5;
int GPIO2 = 4;
char* topic = "valve";

void callback(char* topic, byte* payload, unsigned int length); //forward declaration
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {

  char str[12];

  //converting byte* to char*
  byteArayToCharArray(str, payload, length);

  //turn the valve on if the payload is 'ON-2' and publish to the MQTT server a confirmation message
  if (strcmp(str, "ON-2") == 0) {
    digitalWrite(4, LOW);
    client.publish(topic, "Light On");
  }

  //turn the valve off if the payload is 'OFF-2' and publish to the MQTT server a confirmation message
  else if (strcmp(str, "OFF-2") == 0) {
    digitalWrite(4, HIGH);
    client.publish(topic, "Light Off");
  }
}

void setup() {
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  Serial.begin(115200);
  delay(100);


  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
  delay(2000);
}

void loop() {


  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    //debug printing
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    //print out some more debug once connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(topic);
      }

      //otherwise print failed for debugging
      else {
        Serial.println("\tFailed.");
        abort();
      }
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac) {

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5) {
      result += ':';
    }
  }

  return result;
}


void byteArayToCharArray(char* str, byte* barr, unsigned int length) {
  int i = 0;
  for (; i < length; ++i) {
    str[i] = barr[i];
  }
  str[i] = '\0';
}


