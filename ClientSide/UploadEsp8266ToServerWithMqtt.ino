
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <string>

#define DHTPIN D5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity, soilHumidity;

WiFiClient tsclient;

#define MQTT_SERVER "192.168.43.49"
const char* ssid = "AndroidAP";
const char* password = "xbjx1325";
const char id = '2';


//mqtt setup
char* topic = "sensor";
void callback(char* topic, byte* payload, unsigned int length); //forward declaration
PubSubClient client(MQTT_SERVER, 1883, callback, tsclient);



void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //start wifi subsystem

  reconnect(); //attempt to connect to the WIFI network and then connect to the MQTT server

}


void loop() {

  //sensors data
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soilHumidity = analogRead(A0);


  Serial.print("Temperature Value is :");
  Serial.print(temperature);
  Serial.println("C");
  Serial.print("Humidity Value is :");
  Serial.print(humidity);
  Serial.println("%");


  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }

  //maintain MQTT connection
  client.loop();

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10);

  // We now create a URI for the request
  String url = "{\"humidity\":";
  url += humidity;
  url += ",";
  url += "\"temperature\":";
  url += temperature;
  url += ",";
  url += "\"soilHumidity\":";
  url += soilHumidity;
  url += ",";
  url += "\"id\":";
  url += id;
  url += "}";

  const char * urlc = url.c_str(); //converting string to char*

  //publishing the data of the sensors
  client.publish(topic, urlc);
  delay(20000);
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {

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

//i don't rellay need the callback method here, it's empty just to satisfy the mqtt signature
void callback(char* topic, byte* payload, unsigned int length) {

}

