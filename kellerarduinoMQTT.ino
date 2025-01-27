#include <Adafruit_Sensor.h>
#include <DHT.h>
//#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#define DHTPIN1 D5  // GPIO14, indoor sensor
#define DHTPIN2 D6  // GPIO12, outdoor sensor
#define RELAY_PIN D1 // GPIO5, connected to the relay

// Uncomment the DHT type you're using
#define DHTTYPE DHT22

// Wi-Fi credentials
const char* ssid = "SSID";
const char* password = "Key";

// MQTT broker details
const char* mqtt_server = "192.168.1.100";  // Replace with your MQTT broker's IP
const int mqtt_port = 1883;                 // Default MQTT port
const char* mqtt_user = "mqtt_user";        // Replace with your MQTT username
const char* mqtt_password = "mqtt_pass";    // Replace with your MQTT password

WiFiClient espClient;
PubSubClient mqttClient(espClient);
ESP8266WebServer server(80);

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");

  // Initialize DHT sensors
  dht1.begin();
  dht2.begin();

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Ensure relay is off

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up MQTT client
  mqttClient.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  // Set up web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  updateRelay();
  delay(2000); // Update data every 2 seconds
}

void updateRelay() {
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();

  if (isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT sensors");
    return;
  }

  // Calculate absolute humidity
  float hA1 = calculateAbsoluteHumidity(h1, t1);
  float hA2 = calculateAbsoluteHumidity(h2, t2);

  // Print values to Serial Monitor
  Serial.printf("Indoor: Humidity=%.2f%% Temperature=%.2fC Absolute Humidity=%.2fg/m^3\n", h1, t1, hA1);
  Serial.printf("Outdoor: Humidity=%.2f%% Temperature=%.2fC Absolute Humidity=%.2fg/m^3\n", h2, t2, hA2);

  // Control relay based on humidity comparison
  bool relayStatus = hA1 > hA2;
  if (relayStatus) {
    Serial.println("Fan ON");
    digitalWrite(RELAY_PIN, LOW); // Activate relay
  } else {
    Serial.println("Fan OFF");
    digitalWrite(RELAY_PIN, HIGH); // Deactivate relay
  }

  // Publish MQTT messages
  mqttClient.publish("home/sensors/indoor/humidity", String(h1).c_str());
  mqttClient.publish("home/sensors/indoor/temperature", String(t1).c_str());
  mqttClient.publish("home/sensors/indoor/absolute_humidity", String(hA1).c_str());
  mqttClient.publish("home/sensors/outdoor/humidity", String(h2).c_str());
  mqttClient.publish("home/sensors/outdoor/temperature", String(t2).c_str());
  mqttClient.publish("home/sensors/outdoor/absolute_humidity", String(hA2).c_str());
  mqttClient.publish("home/relay/status", relayStatus ? "ON" : "OFF");
}

float calculateAbsoluteHumidity(float relativeHumidity, float temperature) {
  double exponent = (7.5 * temperature) / (237.0 + temperature);
  double saturationVaporPressure = pow(10, exponent);
  double absoluteHumidity = 13.233 * relativeHumidity * (saturationVaporPressure / (273.16 + temperature));
  return absoluteHumidity;
}

void handleRoot() {
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();

  if (isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2)) {
    server.send(500, "text/plain", "Failed to read from DHT sensors");
    return;
  }

  float hA1 = calculateAbsoluteHumidity(h1, t1);
  float hA2 = calculateAbsoluteHumidity(h2, t2);

  String html = "<html><head><title>ESP8266 Sensor Data</title></head><body>";
  html += "<h1>ESP8266 Sensor Data</h1>";
  html += "<h2>Indoor</h2>";
  html += "<p>Temperature: " + String(t1) + " &deg;C</p>";
  html += "<p>Humidity: " + String(h1) + " %</p>";
  html += "<p>Absolute Humidity: " + String(hA1) + " g/m^3</p>";
  html += "<h2>Outdoor</h2>";
  html += "<p>Temperature: " + String(t2) + " &deg;C</p>";
  html += "<p>Humidity: " + String(h2) + " %</p>";
  html += "<p>Absolute Humidity: " + String(hA2) + " g/m^3</p>";

  if (hA1 > hA2) {
    html += "<p><strong>Fan Status: ON</strong></p>";
  } else {
    html += "<p><strong>Fan Status: OFF</strong></p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

