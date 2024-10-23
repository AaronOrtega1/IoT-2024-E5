#include <ESP32Servo.h>  // Usamos la librería ESP32Servo

Servo myServo;  // Crea un objeto Servo
int servoPin = 15;  // El pin GPIO al que conectas el servomotor

void setup() {
  Serial.begin(115200);  // Inicializa el monitor serial
  myServo.attach(servoPin);  // Adjunta el servomotor al pin GPIO 15
}

void loop() {
  // Movimiento de 0° a 180°
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle);  // Mueve el servomotor al ángulo especificado
    delay(15);  // Pausa para permitir el movimiento
    if (angle == 180) {
      Serial.println("Ángulo: 180° - Servomotor completamente abierto");
    }
  }

  // Movimiento de 180° a 0°
  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle);  // Mueve el servomotor al ángulo especificado
    delay(15);  // Pausa para permitir el movimiento
    if (angle == 0) {
      Serial.println("Ángulo: 0° - Servomotor completamente cerrado");
    }
  }
}

