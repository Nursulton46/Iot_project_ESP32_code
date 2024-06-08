#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi settings
const char* ssid = "Galaxy A31A638";
const char* password = "qwer1234y";

// MQTT settings
const char* mqtt_server = "34.125.161.225";
const int mqtt_port = 1883;
const char* mqtt_user = "mqttProj";
const char* mqtt_password = "anbm444555666";


// GPIO pins
const int RED_LED_PIN = 32;
const int GREEN_LED_PIN = 26;

WiFiClient espClient;
PubSubClient client(espClient);

String getClientId() {
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");
  return "ESP32_" + macAddress;
}

void setup_wifi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
  } else {
    Serial.println("\nFailed to connect to WiFi. Please check your credentials or WiFi network.");
    return;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (strncmp(topic, "ttpu/", 5) == 0) {
    if (doc.containsKey("Home")) {
      if (strcmp(doc["Home"], "on") == 0) {
        digitalWrite(RED_LED_PIN, HIGH);
        Serial.println("Home on");
      } else if (strcmp(doc["Home"], "off") == 0) {
        Serial.println("Home off");
        digitalWrite(RED_LED_PIN, LOW);
      }
    }
  }
  if(strncmp(topic, "ttpu/", 5)==0){
    if (doc.containsKey("Kitchen")) {
      if (strcmp(doc["Kitchen"], "on") == 0) {
        digitalWrite(GREEN_LED_PIN, HIGH);
        Serial.println("Kitchen on");
      } else if (strcmp(doc["Kitchen"], "off") == 0) {
        digitalWrite(GREEN_LED_PIN, LOW);
        Serial.println("Kitchen off");
      }
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(getClientId().c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe("ttpu/#");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}