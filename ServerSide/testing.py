import paho.mqtt.client as mqtt
import json
import threading
import OpenValveScript
import sys
import time

topic = "subscribe"
host = "localhost"
threads_list = []


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(topic)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("got a msg")
    try:
        data_str = str(msg.payload)
        data = json.loads(data_str)
        if ("id" in data):
            global threads_list
            processThread = threading.Thread(target=OpenValveScript.main, args=(data["id"],))
            processThread.daemon = True
            threads_list.append(processThread)
            processThread.start()
            print(msg.topic + " " + str(msg.payload))

    except:
        print "there was an error with subscribing data"


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(host, 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()



#except KeyboardInterrupt:
#	sys.exit()


