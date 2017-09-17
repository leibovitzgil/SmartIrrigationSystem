import paho.mqtt.client as mqtt
import pymongo
import json
import datetime
import time



topic = "valve"
host = "localhost"
broker_address = "localhost"
mqtt_client = mqtt.Client()
mongo_port = 27017
soil_humidity_level_to_irrigate = 350
sleep_timer = 60

# setup connection to db
client = pymongo.MongoClient("localhost", mongo_port)
db = client['mqtt-db']
mqtt_collection = db['mqtt-collection']

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic)

def mqtt_connection_setup():
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.connect(broker_address, 1883, 60)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    try:
        # db handle
        data_str = str(msg.payload)
        data = json.loads(data_str)
        print("got msg")

        if("sleeptimer" in data):
            global sleep_timer
            sleep_timer = data["sleeptimer"]

    except:
        print "there was an error with data2"


# return a list of distinct ids
def get_available_sensors_ids():
    return mqtt_collection.distinct("id")

# return latest data from specified sensor id
def get_latest_sensor_data(id):
    return mqtt_collection.find({"id": id}, {'date': False ,'_id': False}, ).sort([("date", -1)]).limit(1)[0]

# checks if irrigation is necessary
def should_irrigate(id):
    try:
        data = get_latest_sensor_data(id)
        if(data["soilHumidity"] > soil_humidity_level_to_irrigate):
            return True
        else:
            return False

    except:
        print("there was an error with data")
        return False
        
            

def start_irrigate(id):
    mqtt_client.publish(topic, "ON-" + str(id))

def stop_irrigate(id):
    mqtt_client.publish(topic, "OFF-" + str(id))
    
def irrigation_controller():
    while(True):
        available_sensors = get_available_sensors_ids()
		
        for id in available_sensors:
            if (should_irrigate(id)):
                start_irrigate(id)
            else:
                stop_irrigate(id)
                
        time.sleep(sleep_timer)
        
#starting the program
mqtt_connection_setup()
mqtt_client.loop_start()
irrigation_controller()

mqtt_client.loop_stop()



