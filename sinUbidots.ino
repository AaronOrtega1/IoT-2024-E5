#include <WiFi.h> // Incluye esta solo si mantienes el ESP32 conectado al WiFi
#include "HX711.h"
#include "ESP32Servo.h"

// Pines para el HX711
const int LOADCELL_DOUT_PIN = 5;  // Pin DT del HX711
const int LOADCELL_SCK_PIN = 18;  // Pin SCK del HX711

// Pines para el Servomotor y el sensor de movimiento
const int SERVO_PIN = 19;         // Pin donde está conectado el servomotor
const int PIR_PIN = 23;           // Pin donde está conectado el sensor de movimiento

// Objetos para el HX711 y el Servomotor
HX711 scale;
Servo myServo;

float factor_de_calibracion = 470588.2; // Ajusta este valor según el cálculo

void setup() {
  Serial.begin(115200);

  // Configura el HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(factor_de_calibracion); // Aplica el factor de calibración
  scale.tare();                           // Ajusta la tara para que el valor inicial sea 0

  // Configura el servomotor
  myServo.attach(SERVO_PIN);

  // Configura el sensor de movimiento
  pinMode(PIR_PIN, INPUT);

  Serial.println("Configuración completada. Esperando lecturas...");
}

void loop() {
  // Lee el peso del plato
  float peso = (scale.get_units(10) * 1000)*-1; // Convierte a gramos
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println(" g");

  // Verifica si el peso es menor a 0.01 gramos
  if (peso < 1) {
    Serial.println("Peso cero detectado. Abriendo servomotor...");
    myServo.write(90); // Abre el servomotor
    delay(10000);      // Espera 10 segundos
    myServo.write(0);  // Cierra el servomotor
  }

  // Lee el estado del sensor de movimiento
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Movimiento detectado");
    // Imprime la hora en que se detectó el movimiento
    Serial.print("Hora de detección: ");
    Serial.println(millis() / 1000); // Hora en segundos desde el inicio
  }

  delay(1000); // Lee cada segundo
}
