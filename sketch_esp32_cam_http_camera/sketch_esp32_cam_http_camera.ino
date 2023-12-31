#include "WiFi.h"
#include "esp_camera.h"
#include <HTTPClient.h>

const char* ssid = "ssid";
const char* password = "pwd";

//const char* serverAddress = "192.168.0.163";
//const char* serverAddress = "192.168.43.234";
//const uint16_t serverPort = 2137;

const char* serverAddress = "http://192.168.43.234:8080/upload";
HTTPClient http;

// Define the pin for the button
//const int YELLOW_LED_PIN = 16;

// ledPin refers to ESP32-CAM GPIO 4 (flashlight)
const int FLASH_PIN = 4;
const int LED_PIN = 33;

const int TRIG_PIN = 12;
const int ECHO_PIN = 13;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
const int CAPTURE_DISTANCE = 90;

long duration;
float distanceCm;


#define STATE_WAIT_PRESS   0
#define STATE_INIT_PRESS   1
#define STATE_SENDING      2

int state = STATE_WAIT_PRESS;

unsigned long previousMillis = 0;

const long interval = 8000;


// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


void setupCamera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 1;
  
    // Init Camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
}

void connectToWiFi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}


void sendPhoto() {

  // Capture a new photo for the next cycle
  camera_fb_t* new_fb = esp_camera_fb_get();
  if (new_fb) {
    esp_camera_fb_return(new_fb);
    Serial.println("fb pre cleared");
  }

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  digitalWrite(FLASH_PIN, HIGH);
  delayMicroseconds(1000);
  digitalWrite(FLASH_PIN, LOW);

  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/octet-stream");
  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
        Serial.println("Image sent successfully");
  } else {
      Serial.print("Error sending image: ");
      Serial.println(httpResponseCode);
  }
  http.end();

  esp_camera_fb_return(fb);

}


void setup(){
  Serial.begin(57600);
  
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  

  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_PIN, HIGH);
  setupCamera();
  connectToWiFi();
  
}

unsigned long currentMillis;


void loop() {

  currentMillis = millis();
  
  // Clears the TRIG_Pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIG_Pin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  if(distanceCm < CAPTURE_DISTANCE){
    digitalWrite(LED_PIN, LOW);
    Serial.println("Within capture distance");

    if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sendPhoto();
  }

  } 
  else digitalWrite(LED_PIN, HIGH);

  

  /*
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sendPhoto();
  }
  */

  /*
  if (digitalRead(BUTTON_PIN) == LOW){
    switch (state) {
      case STATE_WAIT_PRESS:
      Serial.println("Button pressed, init");
        state = STATE_INIT_PRESS;
        break;
      case STATE_INIT_PRESS:
        delay(100);
        break;
      default:
        break;
    }
  }

  switch (state) {
    case STATE_INIT_PRESS:
      if (digitalRead(BUTTON_PIN) != LOW) {
        state = STATE_SENDING;
      }
      break;
    case STATE_SENDING:
      sendPhoto();
      state = STATE_WAIT_PRESS;
      
      break;
    default:
      //Serial.println("Waiting for button press");
      break;
  }
  */
  
}
