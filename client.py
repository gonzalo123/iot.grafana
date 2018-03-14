import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient
import datetime
import logging


def persists(msg):
    current_time = datetime.datetime.utcnow().isoformat()
    json_body = [
        {
            "measurement": "pot",
            "tags": {},
            "time": current_time,
            "fields": {
                "value": int(msg.payload)
            }
        }
    ]
    logging.info(json_body)
    influx_client.write_points(json_body)


logging.basicConfig(level=logging.INFO)
influx_client = InfluxDBClient('docker', 8086, database='iot')
client = mqtt.Client()

client.on_connect = lambda self, mosq, obj, rc: self.subscribe("/pot")
client.on_message = lambda client, userdata, msg: persists(msg)

client.connect("docker", 1883, 60)

client.loop_forever()
