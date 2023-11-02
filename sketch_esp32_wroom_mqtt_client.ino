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

// States
const char LISTENING = '0';
const char RESULT_POSITIVE = '1';
const char RESULT_NEGATIVE = '2';

char state = LISTENING;

// LEDS
const byte green_led_gpio = 13;
const byte red_led_gpio = 26;
const byte blue_led_gpio = 12;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32-wroom")) {
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
  change_LED_state();
}

void change_LED_state(){
  switch (state) {
    case LISTENING:
      digitalWrite(blue_led_gpio, HIGH);
      digitalWrite(red_led_gpio, LOW);
      digitalWrite(green_led_gpio, LOW);
      Serial.println("State changed to LISTENING");
    break;
    case RESULT_POSITIVE:
      digitalWrite(blue_led_gpio, LOW);
      digitalWrite(red_led_gpio, LOW);
      digitalWrite(green_led_gpio, HIGH);
      Serial.println("State changed to RESULT_POSITIVE");
    break;
    case RESULT_NEGATIVE:
      digitalWrite(blue_led_gpio, LOW);
      digitalWrite(red_led_gpio, HIGH);
      digitalWrite(green_led_gpio, LOW);
      Serial.println("State changed to RESULT_NEGATIVE");
    break;
    default:
      Serial.println("unknown state");
      digitalWrite(blue_led_gpio, LOW);
      digitalWrite(red_led_gpio, LOW);
      digitalWrite(green_led_gpio, LOW);
      Serial.println("Unknown state");
    break;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  state = (char) payload[0];
  Serial.println("Message received");
  change_LED_state();
}

unsigned long previousMillis = 0;
const long interval = 10000;
unsigned long currentMillis;

void loop() {
 if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /*
  if(state != LISTENING){

    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      state = LISTENING;
      digitalWrite(blue_led_gpio, HIGH);
      digitalWrite(red_led_gpio, LOW);
      digitalWrite(green_led_gpio, LOW);
    }

  }
  */
}


