#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const int ledRed = 4;
const int ledGreen = 5;

// Wifi configuration
const char* ssid = "my_wifi_ssid";
const char* password = "my_wifi_password";

// mqtt configuration
const char* server = "192.168.1.111";
const char* topic = "/alert";
const char* clientName = "com.gonzalo123.nodemcu";

int value;
int percent;
String payload;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void wifiConnect() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttReConnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientName)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);

  String data;
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }
  cleanLeds();
  int value = data.toInt();
  switch (value)  {
    case 1:
      digitalWrite(ledRed, HIGH);
      break;
    case 0:
      digitalWrite(ledGreen, HIGH);
      break;
  }
  Serial.print("] value:");
  Serial.println((int) value);
}

void cleanLeds() {
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  cleanLeds();
  Serial.println("start");

  wifiConnect();
  client.setServer(server, 1883);
  client.setCallback(callback);

  delay(1500);
}

void loop() {
  Serial.print(".");
  if (!client.connected()) {
    mqttReConnect();
  }

  client.loop();
  delay(500);
}