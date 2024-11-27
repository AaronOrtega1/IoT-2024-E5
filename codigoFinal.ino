#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Constantes para la conexión WiFi y Ubidots
#define WIFISSID "IoT" // Tu SSID
#define PASSWORD "1t3s0IoT23"            // Tu contraseña
#define TOKEN "BBUS-Er2ldEiDcxgvBocY7CGN2O1mlKSYRF" // Token de Ubidots
#define MQTT_CLIENT_NAME "123456AbCde"      // Cliente MQTT
#define VARIABLE_LABEL_PESO "peso"          // Etiqueta de la variable del peso
#define VARIABLE_LABEL_MOV "hora_movimiento" // Etiqueta de la variable de movimiento
#define DEVICE_LABEL "mqtt-publish-esp32"   // Etiqueta del dispositivo en Ubidots

char mqttBroker[] = "industrial.api.ubidots.com";
char payload[200];
char topic[150];

// Espacio para valores
char str_peso[10];
char str_hora_mov[10];

// Pines y objetos para los sensores y actuadores
const int LOADCELL_DOUT_PIN = 5; // Pin DT del HX711
const int LOADCELL_SCK_PIN = 18; // Pin SCK del HX711
const int SERVO_PIN = 19;        // Pin para el servomotor
const int PIR_PIN = 23;          // Pin para el sensor de movimiento

HX711 scale;
Servo myServo;
float factor_de_calibracion = 470588.2; // Factor de calibración ajustado

WiFiClient ubidots;
PubSubClient client(ubidots);

// Configuración de NTP para obtener la hora real con ajuste de zona horaria de Guadalajara (-6 horas)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -21600, 60000); // -21600 segundos es UTC-6 horas

void callback(char *topic, byte *payload, unsigned int length) {
  // Callback para recibir mensajes de Ubidots, si es necesario
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected to Ubidots");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(factor_de_calibracion);
  scale.tare(); // Ajusta la tara

  myServo.attach(SERVO_PIN);
  pinMode(PIR_PIN, INPUT); // Configura el sensor de movimiento

  // Inicializar el cliente NTP para obtener la hora real
  timeClient.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  timeClient.update(); // Actualiza la hora desde el servidor NTP

  // Lectura del peso
  float peso = (scale.get_units(10)1000)-1; // Convertimos a gramos
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println(" g");

  // Enviar el peso a Ubidots
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "{\"%s\": {\"value\": %s}}", VARIABLE_LABEL_PESO, dtostrf(peso, 4, 2, str_peso));
  client.publish(topic, payload);

  // Verificar si el peso es menor que 0.01 kg (10 gramos)
  if (peso < 0.01) {
    myServo.write(90); // Abre el servomotor
    delay(10000);      // Mantén el servomotor abierto durante 10 segundos
    myServo.write(0);  // Cierra el servomotor
  }

  // Lectura del sensor de movimiento
  if (digitalRead(PIR_PIN) == HIGH) {
    // Obtener la hora actual en horas y minutos
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();

    // Formatear la hora en decimal, donde los minutos se representan como decimales
    float hora_decimal = hours + (minutes / 100.0);
    Serial.print("Movimiento detectado a las: ");
    Serial.println(hora_decimal);

    // Enviar la hora del movimiento en formato decimal a Ubidots
    sprintf(payload, "{\"%s\": {\"value\": %.2f}}", VARIABLE_LABEL_MOV, hora_decimal);
    client.publish(topic, payload);
  }

  client.loop();
  delay(1000);
}
