# Biometrical authentication system using face recognition

This project was developed as part of the Network Embedded Systems course at DTU. The system comprises two interconnected modules communicating through a local server.

The first module involves an ESP32-CAM, which captures a user's facial image. This image is then transmitted via HTTP to the local server. The server processes the image by comparing it with a database of 'known' faces.

Upon analysis, the comparison results are published on an MQTT broker. Simultaneously, a second ESP32-WROOM module subscribes to the corresponding MQTT topic. It retrieves and displays the comparison outcome using external LEDs, providing a visual representation of the facial recognition results.

The project's architecture enables real-time communication and integration between the ESP32-CAM, server, MQTT broker, and ESP32-WROOM, facilitating facial recognition and the display of results through LED indicators.

## Build it yorself (tips&tricks)

Facial Comparison:

For conducting face comparisons, the project utilized the [face-recognition](https://pypi.org/project/face-recognition/) module, negating the need for training a specific model. While the module is impressive, setting it up might be a challenge, particularly on Windows OS. To streamline the installation process, I found this [guide](https://github.com/ageitgey/face_recognition/issues/175#issue-257710508) immensely helpful.

Communication Protocols:

Uplink communication predominantly relied on HTTP due to its convenience. Although WebSocket was considered and tested, its use resulted in excessive overhead for simply sending photos.

For downlink communication, MQTT was the chosen protocol. Note that using secure versions of both HTTP and MQTT should be contemplated for the project's future development.

MQTT Broker Setup:

The project employed [Eclipse Mosquitto](https://mosquitto.org/) as the MQTT Broker on a local machine. Running a local broker may present challenges. Here are some essential actions that could prevent unnecessary frustration:

- When the server and broker operate on the same host within a private network, it often eases efforts in bypassing inherent system security measures.
- Consider modifying rules in the (`mosquitto.conf`) file (e.g., setting (`allow_anonymous true`) and defining the listener as (`1883`)).
- For communication between the broker and ESP32-WROOM, add a firewall rule allowing incoming TCP packets on port (`1883`).
- If the ESP32-WROOM struggles to connect with the broker, pay special attention to the configuration, especially regarding [WiFi.mode(WIFI_STA);](https://github.com/knolleary/pubsubclient/issues/138)

## Authors

Adam Rogowski

Dmytro Kirshev
