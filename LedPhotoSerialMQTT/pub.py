import time
import serial
import random
from paho.mqtt.client import Client
from paho.mqtt.enums import CallbackAPIVersion

SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
SERIAL_TIMEOUT = 2

BROKER = "broker.emqx.io"
PORT = 1883
LUMINOSITY_TOPIC = "laboratory/greenhouse/backyard"
CLIENT_ID = f"PC1_SENSOR_PUBLISHER_{random.randint(10000, 99999)}"

def process_and_publish_sensor_data(connection, client):
    mcu_output = connection.readline()

    if not mcu_output:
        return

    decoded = mcu_output.decode("utf-8").strip()

    if decoded.startswith("SENSOR_VALUE"):
        value_str = decoded.split(":")[1]
        value = int(value_str)

        message_to_publish = ''
        if value < 625:
            message_to_publish = 'u'
            print(f"Value u: {value}")
        elif 625 <= value <= 800:
            message_to_publish = 'b'
            print(f"Value b: {value}")
        else:
            message_to_publish = 'd'
            print(f"Value d: {value}")

        client.publish(LUMINOSITY_TOPIC, message_to_publish, qos=0)


client = Client(callback_api_version=CallbackAPIVersion.VERSION2, client_id=CLIENT_ID)
client.connect(BROKER, PORT)
client.loop_start()

connection = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=SERIAL_TIMEOUT)
time.sleep(2)

while True:
    send_type = random.choice(['p', 's', 'p', 'p'])
    print(f"\nSending command: '{send_type}'")

    command_to_send = f"{send_type}\n".encode('utf-8')
    connection.write(command_to_send)

    if send_type == 's':
        for _ in range(100):
            process_and_publish_sensor_data(connection, client)
    else:
        process_and_publish_sensor_data(connection, client)

    time.sleep(3)

client.loop_stop()
client.disconnect()
connection.close()