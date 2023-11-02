# Biometrical authentication system using face recognition

This project was developed as part of the Network Embedded Systems course at DTU. The system comprises two interconnected modules communicating through a local server.

The first module involves an ESP32-CAM, which captures a user's facial image. This image is then transmitted via HTTP to the local server. The server processes the image by comparing it with a database of 'known' faces.

Upon analysis, the comparison results are published on an MQTT broker. Simultaneously, a second ESP32-WROOM module subscribes to the corresponding MQTT topic. It retrieves and displays the comparison outcome using external LEDs, providing a visual representation of the facial recognition results.

The project's architecture enables real-time communication and integration between the ESP32-CAM, server, MQTT broker, and ESP32-WROOM, facilitating facial recognition and the display of results through LED indicators.

## Build it yorself (tips&tricks)

For conducting face comparison no model was trained! Instead [face-recognition](https://pypi.org/project/face-recognition/) module was used. Despite how amazing it is after it's already succesfully installed, getting this beast to work eventually might be tricky, especially on Windows OS. [This](https://github.com/ageitgey/face_recognition/issues/175#issue-257710508) made the process so much easier.

For uplink communication http turned out to be the most convenient. Also webSocket was considered and tested, however eventually provided to much overhead just for sending photos. For downlink communication MQTT was used. Secure versions of both protocols should be considered during further development of the project.

[Eclipse Mosquitto](https://mosquitto.org/) was used as a MQTT Broker on a local machine. Running a local broker might come up with some challenges, so here is a gentle reminder for basic actions, which might save some frustration:

- if server and broker run on the same host, opereting within same private network usually saves some effort of overcoming built-in system security measures;
- if needed, change rules in the mosquitto.conf (basic: allow_anonymus true, listener 1883);
- for communnication between the broker and ESP32-WROOM, firewall rule has to be added (enabling incoming TCP packets on port 1883);
- if WROOM can't establish connection with the broker, pay special attention to [WiFi.mode(WIFI_STA);](https://github.com/knolleary/pubsubclient/issues/138)

## Authors

Adam Rogowski

Dmytro Kirshev
