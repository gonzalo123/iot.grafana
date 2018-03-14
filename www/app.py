from flask import Flask
from flask import request
from flask_httpauth import HTTPBasicAuth
import paho.mqtt.client as mqtt
import json

client = mqtt.Client()

app = Flask(__name__)
auth = HTTPBasicAuth()

# http basic auth credentials
users = {
    "user": "password"
}


@auth.get_password
def get_pw(username):
    if username in users:
        return users.get(username)
    return None


@app.route('/alert', methods=['POST'])
@auth.login_required
def alert():
    client.connect("docker", 1883, 60)
    data = json.loads(request.data.decode('utf-8'))
    if data['state'] == 'alerting':
        client.publish(topic="/alert", payload="1", retain=True)
    elif data['state'] == 'ok':
        client.publish(topic="/alert", payload="0", retain=True)

    client.disconnect()

    return "ok"


if __name__ == "__main__":
    app.run(host='0.0.0.0')
