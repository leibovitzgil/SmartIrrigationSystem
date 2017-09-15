import paho.mqtt.client as mqtt
import pymongo
import json
import datetime

topic = "valve"
host = "localhost"
broker_address = "localhost"
soil_humidity_level_to_irrigate = 350

# setup connection to db
client = pymongo.MongoClient("localhost", 27017)
db = client['mqtt-db']
mqtt_collection = db['mqtt-collection']

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic)

def mqtt_connection_setup:
    client = mqtt.Client()
    client.on_connect = on_connect
    client.connect(broker_address, 1883, 60)
    

# return a list of distinct ids
def get_available_sensors_ids():
    return mqtt_collection.distinct("id")

# return latest data from specified sensor id
def get_latest_sensor_data(id):
    return mqtt_collection.find({"id": id}, {'date': False ,'_id': False}, ).sort([("date", -1)]).limit(1)[0]

# checks if irrigation is necessary
def should_irrigate(id):
    data = get_latest_sensor_data(id)
    if(data["test"] > soil_humidity_level_to_irrigate):
        return True
    return False

def start_irrigate(id):
    return True
    
def irrigation_controller():
    available_sensors = get_available_sensors_ids()

    for id in available_sensors:
        if (should_irrigate(id)):
            start_irrigate(id)
        #print(str(get_latest_sensor_data(id)) + " should irrigate: " + str(should_irrigate(id)))

irrigation_controller()
#last_obs = get_latest_sensor_data(1)
#print(last_obs)
