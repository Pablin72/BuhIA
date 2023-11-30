#include <GL/freeglut.h>
#include <cmath>
#include <iostream> // Incluye la biblioteca iostream
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#define CURL_STATICLIB
#include <curl/curl.h>

void llamarApi();
void mouseMoveCallback(int x, int y);
bool bienvenido = false;
const float PI = 3.14159265359f;
float movAlas = 0.0f;
float movPico = 0.0f;
//Variables globales para obtener la posición del mouse
float mouseX = 0.0f;
float mouseY = 0.0f;
// Se incluye la biblioteca mutex para la sincronización de hilos
std::mutex responseMutex;
// Variable para almacenar el nombre predicho
std::string personaPredicha = "";
// Variable para almacenar la respuesta HTTP de la API
std::string response_data;
// Declarar una variable para controlar el estado de la interfaz
enum InterfaceState {
	STATE_START,
	STATE_FACIAL_RECOGNITION,
	STATE_VOICE_RECOGNITION,
};

// Declarar una variable para controlar el estado de los ojos
enum EyeState {
	EYE_STATE_NORMAL,
	EYE_STATE_ANIMATING,
};

EyeState eyeState = EYE_STATE_NORMAL; // Estado inicial de los ojos
int animationStartTime = 0; // Tiempo de inicio de la animación
const int animationDuration = 5000; // Duración de la animación en milisegundos (5 segundos)

InterfaceState currentState = STATE_START; // Estado inicial

bool isPredicting = false; // Estado para rastrear si está en modo de predicción

void drawLine(int x1, int y1, int x2, int y2, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void drawSquare(int x, int y, int size, float r, float g, float b) {
	glColor3f(0.48f, 0.24f, 0.16f);
	glBegin(GL_QUADS);
	glVertex2i(225, 75);
	glVertex2i(225, 425);
	glVertex2i(775, 425);
	glVertex2i(775, 75);
	glEnd();
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glVertex2i(x3, y3);
	glEnd();
}

void drawCircle(float centerX, float centerY, float radius, int numSegments, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(centerX, centerY);
	for (int i = 0; i <= numSegments; ++i) {
		float angle = 2.0f * PI * static_cast<float>(i) / static_cast<float>(numSegments);
		float x = centerX + radius * std::cos(angle);
		float y = centerY + radius * std::sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

//Botones de reconocimiento
void recognitionButtonCallback(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {  // Si se presiona un botón del mouse
		if (button == GLUT_LEFT_BUTTON) {  // Si es el botón izquierdo del mouse

			// Cambio de estado según la posición del clic
			if (currentState == STATE_START) {
				if (x >= 15 && x <= 215 && y >= 250 && y <= 300) {
					currentState = STATE_FACIAL_RECOGNITION;
				}
				else if (x >= 790 && x <= 995 && y >= 250 && y <= 300) {
					currentState = STATE_VOICE_RECOGNITION;
				}
				else if (x >= 850 && x <= 950 && y >= 450 && y <= 500) {
					currentState = STATE_START;
					isPredicting = false;
				}
			}
			else if (currentState == STATE_FACIAL_RECOGNITION) {
				if (x >= 100 && x <= 200 && y >= 450 && y <= 500) {
					isPredicting = true;
					isPredicting = true;
					eyeState = EYE_STATE_NORMAL;
					animationStartTime = glutGet(GLUT_ELAPSED_TIME);
				}
				else if (x >= 850 && x <= 950 && y >= 450 && y <= 500) {
					currentState = STATE_START;
					std::cout << "Aplasto Volver";
					currentState == STATE_START;
				}

			}
			else if (currentState == STATE_VOICE_RECOGNITION) {
				if (x >= 100 && x <= 200 && y >= 450 && y <= 500) {
					isPredicting = true;
					std::cout << "Aplasto Predecir";
				}
				else if (x >= 850 && x <= 950 && y >= 450 && y <= 500) {
					currentState = STATE_START;
					std::cout << "Aplasto Volver";
					currentState == STATE_START;
				}

			}
		}
	}
	glutPostRedisplay(); // Solicita un redibujado para actualizar la pantalla
}


void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

//Función para que los ojos sigan el movimiento del mouse
void mouseMoveCallback(int x, int y) {
	// Calcula la distancia desde el cursor hasta el centro de los círculos de ojos medianos
	float deltaX = x - 625.0f;
	float deltaY = y - 175.0f;
	float distancia = std::sqrt(deltaX * deltaX + deltaY * deltaY);

	// Calcula el vector de dirección normalizado desde el centro de los círculos de ojos medianos hasta el cursor
	float dirX = deltaX / distancia;
	float dirY = deltaY / distancia;

	// Calcula las nuevas posiciones de los círculos de ojos pequeños en función del vector de dirección
	const float distanciaOjo = 30.0f; // Distancia desde el centro del círculo de ojo mediano hasta el círculo de ojo pequeño
	mouseX = 625.0f + dirX * distanciaOjo;
	mouseY = 175.0f + dirY * distanciaOjo;

	glutPostRedisplay(); // Solicita un redibujado para actualizar la pantalla
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Orejas
	drawTriangle(775, 25, 700, 75, 775, 75, 0.48f, 0.24f, 0.16f); // oreja derecha
	drawTriangle(225, 25, 225, 75, 300, 75, 0.48f, 0.24f, 0.16f); // oreja izquierda

	// Cuerpo
	drawSquare(225, 75, 550, 0.48f, 0.24f, 0.16f);

	// Pico superior
	drawTriangle(500, 200, 425, 250, 500, 250 + static_cast<int>(movPico), 0.76f, 0.66f, 0.34f);
	drawLine(500, 200, 425, 250, 0.0f, 0.0f, 0.0f);
	drawLine(425, 250, 500, 250 + static_cast<int>(movPico), 0.0f, 0.0f, 0.0f);
	drawLine(500, 250 + static_cast<int>(movPico), 500, 200, 0.0f, 0.0f, 0.0f);

	// Pico inferior
	drawTriangle(500, 250, 450, 250, 475, 275 - static_cast<int>(movPico), 0.76f, 0.66f, 0.34f);
	drawLine(500, 250, 450, 250, 0.0f, 0.0f, 0.0f);
	drawLine(450, 250, 475, 275 - static_cast<int>(movPico), 0.0f, 0.0f, 0.0f);
	drawLine(475, 275 - static_cast<int>(movPico), 500, 250, 0.0f, 0.0f, 0.0f);

	// Cuerpo inferior
	drawTriangle(225, 425, 500, 600, 775, 425, 0.48f, 0.24f, 0.16f);

	// Alas
	float wingOffset = 10.0f * std::sin(movAlas);
	drawTriangle(775 + static_cast<int>(wingOffset), 250, 775, 400, 875, 400, 0.2f, 0.1f, 0.0f);
	drawTriangle(225 - static_cast<int>(wingOffset), 250, 125, 400, 225, 400, 0.2f, 0.1f, 0.0f);




	// Detalles del cuerpo
	drawLine(225, 225, 275, 225, 0.0f, 0.0f, 0.0f); // línea superior izquierda
	drawLine(275, 225, 500, 350, 0.0f, 0.0f, 0.0f); // línea superior central
	drawLine(500, 350, 725, 225, 0.0f, 0.0f, 0.0f); // línea superior derecha
	drawLine(725, 225, 775, 225, 0.0f, 0.0f, 0.0f); // línea superior extrema derecha

	drawLine(225, 300, 275, 300, 0.0f, 0.0f, 0.0f); // línea media izquierda
	drawLine(275, 300, 500, 425, 0.0f, 0.0f, 0.0f); // línea media central
	drawLine(500, 425, 725, 300, 0.0f, 0.0f, 0.0f); // línea media derecha
	drawLine(725, 300, 775, 300, 0.0f, 0.0f, 0.0f); // línea media extrema derecha

	drawLine(375, 525, 500, 425, 0.0f, 0.0f, 0.0f); // línea inferior izquierda
	drawLine(500, 425, 625, 525, 0.0f, 0.0f, 0.0f); // línea inferior derecha

	// Ojos
	drawCircle(625, 175, 75, 150, 0.98f, 0.98f, 0.96f); // ojo derecho grande
	drawCircle(625, 175, 50, 150, 0.0f, 0.0f, 0.0f);   // ojo derecho mediano
	drawCircle(mouseX, mouseY, 20, 150, 0.98f, 0.98f, 0.96f);  // ojo derecho pequeño

	drawCircle(375, 175, 75, 150, 0.98f, 0.98f, 0.96f); // ojo izquierdo grande
	drawCircle(375, 175, 50, 150, 0.0f, 0.0f, 0.0f);   // ojo izquierdo mediano
	drawCircle(mouseX - 250, mouseY, 20, 150, 0.98f, 0.98f, 0.96f); // ojo izquierdo pequeño


	//Movimiento de alas
	movAlas = 0.01f;
	if (movAlas >= 0.000001f) {
		movAlas;
	}

	if (currentState == STATE_START) {
		// Dibuja los botones "Reconocimiento Facial" y "Reconocimiento de Voz"
		glColor3f(0.5f, 0.5f, 0.5f); // Color de los botones
		glRecti(15, 250, 215, 300); // Botón Reconocimiento Facial
		glRecti(790, 250, 995, 300); // Botón Reconocimiento de Voz

		// Dibuja los textos de los botones
		glColor3f(0.0f, 0.0f, 0.0f); // Color del texto
		glRasterPos2i(20, 280); // Posición del texto
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Reconocimiento Facial"));
		glRasterPos2i(795, 280); // Posición del texto
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Reconocimiento de Voz"));
	}
	else if (currentState == STATE_FACIAL_RECOGNITION) {
		// Dibuja el título "Reconocimiento Facial"
		glColor3f(0.0f, 0.0f, 0.0f); // Color del título
		glRasterPos2i(350, 50); // Posición del título
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, reinterpret_cast<const unsigned char*>("Reconocimiento Facial"));

		if (bienvenido) {

			// Dibuja el título "Reconocimiento Facial"
			glColor3f(0.0f, 0.0f, 0.0f); // Color del título
			glRasterPos2i(10, 200); // Posición del título
			const char* personaPredichaC = personaPredicha.c_str();
			std::string mensajeBienvenida = "Bienvenido " + personaPredicha + "!!!";
			const char* mensajeBienvenidaC = mensajeBienvenida.c_str();

			// Utiliza glutBitmapString para mostrar la cadena
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, reinterpret_cast<const unsigned char*>(mensajeBienvenidaC));

		}

		if (isPredicting) {
			// Dibuja los botones "Predecir" y "Volver"
			glColor3f(0.5f, 0.5f, 0.5f); // Color de los botones
			glRecti(100, 450, 200, 500); // Botón Predecir
			glRecti(850, 450, 950, 500); // Botón Volver

			// Dibuja los textos de los botones
			glColor3f(0.0f, 0.0f, 0.0f); // Color del texto
			glRasterPos2i(110, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Predecir"));
			glRasterPos2i(860, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Volver"));

			// Cambia el estado de la animación
			eyeState = EYE_STATE_ANIMATING;

			// Calcula el tiempo transcurrido desde el inicio de la animación
			int currentTime = glutGet(GLUT_ELAPSED_TIME);
			int elapsedTime = currentTime - animationStartTime;


			if (elapsedTime < animationDuration) {
				// Durante la animación: agrandar y rotar los ojos, cambiar el color
				float scaleFactor = static_cast<float>(elapsedTime) / animationDuration;
				float rotationAngle = 360.0f * scaleFactor;

				// Dibuja los ojos con animación
				drawCircle(625, 175, 50 + 20 * scaleFactor, 150, 0.0f, 0.0f, 0.98f); // ojo derecho pequeño
				drawCircle(375, 175, 50 + 20 * scaleFactor, 150, 0.0f, 0.0f, 0.98f); // ojo izquierdo pequeño



			}
			else if (elapsedTime == animationDuration) {
				llamarApi();
				

				glutPostRedisplay();

				// Espera durante unos segundos antes de volver al estado normal
				std::this_thread::sleep_for(std::chrono::seconds(2)); // Espera 2 segundos

				glutPostRedisplay();

			}
			else {
				// Animación finalizada: vuelve al estado normal
				// Realiza la solicitud a la API y procesa la respuesta

				eyeState = EYE_STATE_NORMAL;
				elapsedTime = 0;
			}


		}
		else {
			// Dibuja los botones "Predecir" y "Volver"
			glColor3f(0.5f, 0.5f, 0.5f); // Color de los botones
			glRecti(100, 450, 200, 500); // Botón Predecir
			glRecti(850, 450, 950, 500); // Botón Volver

			// Dibuja los textos de los botones
			glColor3f(0.0f, 0.0f, 0.0f); // Color del texto
			glRasterPos2i(110, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Predecir"));
			glRasterPos2i(860, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Volver"));
			eyeState == EYE_STATE_NORMAL;
		}
	}
	else if (currentState == STATE_VOICE_RECOGNITION) {
		// Dibuja el título "Reconocimiento Facial"
		glColor3f(0.0f, 0.0f, 0.0f); // Color del título
		glRasterPos2i(350, 50); // Posición del título
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, reinterpret_cast<const unsigned char*>("Reconocimiento de Voz"));

		if (isPredicting) {
			// Dibuja los elementos de predicción facial
			// Dibuja los botones "Predecir" y "Volver"
			glColor3f(0.5f, 0.5f, 0.5f); // Color de los botones
			glRecti(100, 450, 200, 500); // Botón Predecir
			glRecti(850, 450, 950, 500); // Botón Volver

			// Dibuja los textos de los botones
			glColor3f(0.0f, 0.0f, 0.0f); // Color del texto
			glRasterPos2i(110, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Predecir"));
			glRasterPos2i(860, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Volver"));
		}
		else {
			// Dibuja los botones "Predecir" y "Volver"
			glColor3f(0.5f, 0.5f, 0.5f); // Color de los botones
			glRecti(100, 450, 200, 500); // Botón Predecir
			glRecti(850, 450, 950, 500); // Botón Volver

			// Dibuja los textos de los botones
			glColor3f(0.0f, 0.0f, 0.0f); // Color del texto
			glRasterPos2i(110, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Predecir"));
			glRasterPos2i(860, 485); // Posición del texto
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<const unsigned char*>("Volver"));
		}
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

// Función de devolución de llamada utilizada por cURL para manejar la respuesta HTTP
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
	size_t total_size = size * nmemb;
	{
		// Se utiliza un bloqueo de mutex para asegurar que la escritura en 'output' sea segura en entornos multiproceso
		std::lock_guard<std::mutex> lock(responseMutex);
		output->append(static_cast<char*>(contents), total_size);
	}
	return total_size;
}



// Función para realizar la llamada a la API
void llamarApi() {
	std::string api_url = "http://localhost:5000/predict";
	CURL* curl = curl_easy_init();

	if (curl) {
		CURLcode res;

		// Configurar la URL y la función de devolución de llamada para la respuesta
		curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

		// Realizar la solicitud HTTP
		res = curl_easy_perform(curl);

		if (res == CURLE_OK) {
			// Buscar la cadena "persona_predicha": " en la respuesta JSON
			std::string buscar = "\"persona_predicha\": \"";
			size_t pos = response_data.find(buscar);
			if (pos != std::string::npos) {
				pos += buscar.length();
				size_t endPos = response_data.find("\"", pos);
				if (endPos != std::string::npos) {
					// Extraer el valor de "persona_predicha" y almacenarlo en la variable
					personaPredicha = response_data.substr(pos, endPos - pos);
					std::cout << "Persona predicha: " << personaPredicha << std::endl;
					bienvenido = true;
				}
			}
		}
		else {
			std::cerr << "Error en la solicitud al API: " << curl_easy_strerror(res) << std::endl;
		}

		// Liberar recursos de cURL
		curl_easy_cleanup(curl);
	}
	else {
		std::cerr << "Error al inicializar cURL." << std::endl;
	}

}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1000, 600);
	glutCreateWindow("Buho animado");

	glClearColor(0.89f, 0.79f, 0.62f, 1.0f);

	glutDisplayFunc(display);
	glutMouseFunc(recognitionButtonCallback);
	glutPassiveMotionFunc(mouseMoveCallback); //Reconocer el movimiento del mouse

	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}