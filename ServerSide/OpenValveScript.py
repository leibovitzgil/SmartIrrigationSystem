import paho.mqtt.client as mqtt
import pymongo
import json
import datetime

topic = "valve"
host = "localhost"

# setup connection to db
client = pymongo.MongoClient("localhost", 27017)
db = client['mqtt-db']
mqtt_collection = db['mqtt-collection']

# return a list of distinct ids
def get_available_sensors_ids():
    return mqtt_collection.distinct("id")

# return latest data from specified sensor id
def check_if_should_irrigate(id):
    return mqtt_collection.find({"id": id}, {'date': False ,'_id': False}, ).sort([("date", -1)]).limit(1)

last_obs = check_if_should_irrigate(1)[0]
print(last_obs)
