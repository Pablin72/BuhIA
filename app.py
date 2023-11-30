import numpy as np
import cv2
from tensorflow.keras.models import load_model
from flask import Flask, jsonify

app = Flask(__name__)

# Cargar el modelo entrenado
modelo_reconocimiento_facial = load_model("modeloCNNv6.h5")

lista_nombres_personas = ['Sergio', 'Sebastian', 'Anthony', 'Mattew', 'Diego', 'Fernando', 'Desconocidos', 'Pablo']

def realizar_reconocimiento(imagen):
    # Preprocesamiento de la imagen
    img = cv2.resize(imagen, (150, 150))
    img = img / 255.0
    img = np.expand_dims(img, axis=0)

    # Realizar la predicción
    predictions = modelo_reconocimiento_facial.predict(img)
    predicted_label = np.argmax(predictions)
    nombre_persona_predicha = lista_nombres_personas[predicted_label]

    return nombre_persona_predicha

@app.route("/predict", methods=["GET"])
def predict_person():
    # Capturar la imagen desde la cámara
    capture = cv2.VideoCapture(0)
    ret, frame = capture.read()

    if ret:
        # Realizar el reconocimiento y la predicción
        nombre_persona_predicha = realizar_reconocimiento(frame)

        print(nombre_persona_predicha)

        # Liberar la cámara
        capture.release()

        return jsonify({"persona_predicha": nombre_persona_predicha})
    else:
        return jsonify({"persona_predicha": "Error en la captura de imagen"})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
