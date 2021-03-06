import paho.mqtt.client as mqtt
import pymongo
import json
import datetime
# Import library and create instance of REST client.
from Adafruit_IO import Client
aio = Client('66144fa802464d38bfa34c36e79e23f9')

topic = "sensor"
host = "localhost"



client = pymongo.MongoClient("localhost", 27017)
db = client['mqtt-db']
mqtt_collection = db['mqtt-collection']


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    try:
        #db handle
        data_str = str(msg.payload)
        data = json.loads(data_str)
        data["date"] = datetime.datetime.now()
        mqtt_collection.save(data)
            
        #Adafruit handle
        humidity = str(data["humidity"])
        temperature = str(data["temperature"])
        soilHumidity = str(data["soilHumidity"])

        aio.send('humidity', humidity)
        aio.send('temperature', temperature)
        aio.send('soilHumidity', soilHumidity)


        print(msg.topic+" "+str(msg.payload))

    except:
        print "there was an error with data"

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(host, 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
