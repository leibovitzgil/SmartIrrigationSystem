
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h> 
#include <string>

#define DHTPIN D5  
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity, light;

WiFiClient tsclient;

#define MQTT_SERVER "192.168.14.69"
//const char* ssid = "Bezeq-n_3FD6";
//const char* password = "oritl4040";
//const char* ssid = "AndroidAP";
//const char* password = "xbjx1325";
const char* ssid = "Orit";
const char* password = "0507689766";


//mqtt setup
char* lightTopic = "sensor";
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
  light = analogRead(A0);

//  Serial.print("connecting to ");
//  Serial.println(MQTT_SERVER);
  
  Serial.print("Temperature Value is :");  
  Serial.print(temperature);  
  Serial.println("C");  
  Serial.print("Humidity Value is :");  
  Serial.print(humidity);  
  Serial.println("%");


  //reconnect if connection is lost
  if (!client.connected() && WiFi.status() == 3) {reconnect();}

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
  url += "\"light\":";
  url += light;
  url += "}";

  const char * urlc = url.c_str(); //converting string to char*

  //publishing the data of the sensors
  client.publish(lightTopic, urlc);
  
//  Serial.print("Requesting URL: ");
//  Serial.println(url);
  delay(20000);
}


void reconnect() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED){
    //debug printing
//    Serial.print("Connecting to ");
//    Serial.println(ssid);

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
//      Serial.print(".");
    }

    //print out some more debug once connected
//    Serial.println("");
//    Serial.println("WiFi connected");  
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED){
  // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
//      Serial.print("Attempting MQTT connection...");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      if (client.connect((char*) clientName.c_str())) {
        Serial.print("\tMTQQ Connected");
        client.subscribe(lightTopic);
      }

      //otherwise print failed for debugging
      else{Serial.println("\tFailed."); abort();}
    }
  }
}

//generate unique name from MAC addr
String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 

  //Print out some debugging info
  Serial.println("Callback update.");
  Serial.print("Topic: ");
  Serial.println(topicStr);

  //turn the light on if the payload is '1' and publish to the MQTT server a confirmation message
  if(payload[0] == '1'){
    digitalWrite(LED_BUILTIN, HIGH);
    client.publish("sensor", "Light On");

  }

  //turn the light off if the payload is '0' and publish to the MQTT server a confirmation message
  else if (payload[0] == '0'){
    digitalWrite(LED_BUILTIN, LOW);
    client.publish("sensor", "Light Off");
  }

}

