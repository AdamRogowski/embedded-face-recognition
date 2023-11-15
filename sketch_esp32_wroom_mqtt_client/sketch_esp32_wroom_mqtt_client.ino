#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ssid";
const char* password = "pwd";

// MQTT Broker
IPAddress _mqtt_broker(192, 168, 43, 234);
const char *mqtt_broker = "192.168.43.234";
const char *topic = "esp32/state";
const int mqtt_port = 1883;
const char *client_id = "ESP32-wroom";

WiFiClient espClient;
PubSubClient client(espClient);

enum State {
  LISTENING,
  RESULT_POSITIVE,
  RESULT_NEGATIVE,
  UNKNOWN
};

State currentState = LISTENING;

unsigned long stateChangeTime = 0;
const unsigned long stateChangeDelay = 5000;

// Function to check if it's time to return to default state (LISTENING)
void check_return_to_LISTENING() {
  if ((currentState != LISTENING) && (millis() - stateChangeTime >= stateChangeDelay)) {
    currentState = LISTENING;
    update_LEDs();
  }
}

// LEDS
const byte green_led_gpio = 13;
const byte red_led_gpio = 26;
const byte blue_led_gpio = 12;

void update_LEDs() {
  digitalWrite(blue_led_gpio, currentState == LISTENING);
  digitalWrite(green_led_gpio, currentState == RESULT_POSITIVE);
  digitalWrite(red_led_gpio, currentState == RESULT_NEGATIVE);
  // Turn off all LEDs if the state is UNKNOWN
  if (currentState == UNKNOWN) {
    digitalWrite(blue_led_gpio, LOW);
    digitalWrite(green_led_gpio, LOW);
    digitalWrite(red_led_gpio, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32-wroom")) {
      client.subscribe(topic);
      Serial.println("Connected to broker, subscribed the topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received");
  switch ((char) payload[0]) {
    case '0':
      currentState = LISTENING;
      Serial.println("State changed to LISTENING");
      break;
    case '1':
      currentState = RESULT_POSITIVE;
      Serial.println("State changed to RESULT_POSITIVE");
      stateChangeTime = millis(); // Record the time of state change
      break;
    case '2':
      currentState = RESULT_NEGATIVE;
      Serial.println("State changed to RESULT_NEGATIVE");
      stateChangeTime = millis(); // Record the time of state change
      break;
    default:
      currentState = UNKNOWN;
      Serial.println("Unknown state");
      stateChangeTime = millis(); // Record the time of state change
      break;
  }
  update_LEDs();
}

void setup() {
  Serial.begin(115200);

  pinMode(green_led_gpio, OUTPUT);
  pinMode(red_led_gpio, OUTPUT);
  pinMode(blue_led_gpio, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(_mqtt_broker, mqtt_port);
  client.setCallback(callback);
  WiFi.mode(WIFI_STA);
  if(client.connect("ESP32-wroom")){
    client.subscribe(topic);
    Serial.println("Connected to broker, subscribed the topic");
  } 
  update_LEDs();
}

void loop() {
 if (!client.connected()) {
    reconnect();
  }
  client.loop();
  check_return_to_LISTENING();
}


