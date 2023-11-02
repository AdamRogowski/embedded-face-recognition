from flask import Flask, request
import cv2
import numpy as np
import face_recognition
import os
import random
from enum import Enum
import paho.mqtt.publish as mqttp


HOST_IP = "192.168.43.234"
HTTP_PORT = 8080


class State(Enum):
    LISTENING = "0"
    RESULT_POSITIVE = "1"
    RESULT_NEGATIVE = "2"


def publish_state_change(state):
    # Assuming that mqtt broker runs on the same host as flask_server, otherwise change hostname=MQTT_BROKER_IP
    # To start a mosquitto from localhost on Windows, run admin cmd -> cd C:\Program Files\mosquitto -> net start mosquitto/net stop mosquitto
    client_id = "flask_server"
    topic = "esp32/state"
    message = state.value
    publish = mqttp.single(topic, message, hostname=HOST_IP, client_id=client_id)


app = Flask(__name__)

known_faces_dir = "known_faces"
unknown_face_dir = "unknown_face"


@app.route("/upload", methods=["POST"])
def save_received_image():
    if request.method == "POST":
        print("Received a POST request")
        print(request.headers)

        # Check if the request contains binary data
        if request.data:
            print("Raw data received")

            # Convert the raw binary data to a NumPy array
            np_data = np.frombuffer(request.data, dtype=np.uint8)

            # Create an OpenCV image from the NumPy array
            image = cv2.imdecode(np_data, cv2.IMREAD_COLOR)

            if image is not None:
                # Process the image as needed
                random_number = random.randint(100000, 999999)
                filename = os.path.join(unknown_face_dir, f"image_{random_number}.jpg")
                cv2.imwrite(filename, image)
                publish_state_change(
                    State.RESULT_POSITIVE
                ) if compare_new_face() else publish_state_change(State.RESULT_NEGATIVE)
                remove_files_from_unknown_face_dir()
                return "Image received and saved successfully"
            else:
                print("Image conversion failed")
                return "Image conversion failed"
        else:
            print("No raw data received")
            return "No raw data received"
    return "Invalid request"


def compare_new_face():
    # Check if the "unknown_face" directory is empty
    if not os.listdir(unknown_face_dir):
        print("The 'unknown_face' directory is empty. No faces to compare.")
        return False

    for filename in os.listdir(unknown_face_dir):
        if (
            filename.endswith(".jpg")
            or filename.endswith(".jpeg")
            or filename.endswith(".png")
        ):
            first_unknown_image = face_recognition.load_image_file(
                os.path.join(unknown_face_dir, filename)
            )
        else:
            print("No image files found in the 'unknown_face' directory.")
            return False

    first_unknown_face_encodings = face_recognition.face_encodings(first_unknown_image)

    # Ensure there is at least one face in the first unknown image
    if len(first_unknown_face_encodings) > 0:
        first_unknown_face_encoding = first_unknown_face_encodings[0]
        # Compare the first unknown face encoding to the known faces
        results = face_recognition.compare_faces(
            known_faces, first_unknown_face_encoding
        )
        if True in results:
            print("New face has been recognized.")
            return True

    print("New face has NOT been recognized.")
    return False


def remove_files_from_unknown_face_dir():
    try:
        for filename in os.listdir(unknown_face_dir):
            file_path = os.path.join(unknown_face_dir, filename)
            if os.path.isfile(file_path):
                os.remove(file_path)
        print("'unknown_face' directory have been cleared")
    except Exception as e:
        print(f"Error while removing files: {e}")


if __name__ == "__main__":
    known_faces = []

    for filename in os.listdir(known_faces_dir):
        if (
            filename.endswith(".jpg")
            or filename.endswith(".jpeg")
            or filename.endswith(".png")
        ):
            image = face_recognition.load_image_file(
                os.path.join(known_faces_dir, filename)
            )

            face_encodings = face_recognition.face_encodings(image)

            # Ensure that there is at least one face in the image
            if len(face_encodings) > 0:
                known_faces.append(face_encodings[0])
            else:
                print(f"No face found in {filename}")

    publish_state_change(State.LISTENING)
    app.run(host=HOST_IP, port=HTTP_PORT)
