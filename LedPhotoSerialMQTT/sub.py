import serial 
import time
from paho.mqtt.client import Client, MQTTMessage
from paho.mqtt.enums import CallbackAPIVersion
import random

connection = serial.Serial('/dev/ttyUSB0',9600, timeout=1)
# на загрузку платы
time.sleep(5)

# гасим свет
cmd = 'd'
connection.write(cmd.encode('utf-8'))
time.sleep(.1)
print(connection.read_all().decode())

broker = "broker.emqx.io"
luminosity_topic = "laboratory/greenhouse/backyard"
light_state = "off"

def on_connect(client: Client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("Connected to MQTT Broker!")
    else:
        print("Failed to connect, return code %d\n", reason_code)

def on_message(client: Client, userdata, message: MQTTMessage):
    data = str(message.payload.decode("utf-8"))
    if data == '':
        return
    topic = message.topic
    print(f"Received message {data} from topic {topic}")
    connection.write(data.encode('utf-8'))
    time.sleep(.1)
    print(connection.read_all().decode())
    

# ID is important to broker make sure it is unique.
if __name__ == "__main__":
    client= Client(
        callback_api_version=CallbackAPIVersion.VERSION2,
        client_id = f'MY_CLIENT_ID_{random.randint(10000, 99999)}'
    )
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker)

    client.connect(broker)
    client.loop_start()
    print(f"Subcribing to {luminosity_topic}")
    client.subscribe(luminosity_topic, qos=0)
    time.sleep(3600)
    client.disconnect()
    client.loop_stop()