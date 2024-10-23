// Definir el pin del sensor PIR
int pirPin = 4;  // OUT del PIR conectado al pin digital 4 de la ESP32
int pirState = LOW;  // Inicializar el estado del PIR a LOW (sin movimiento)
int val = 0;  // Variable para almacenar el valor de lectura del PIR

void setup() {
  Serial.begin(115200);  // Iniciar comunicación serial
  pinMode(pirPin, INPUT);  // Configurar el pin del PIR como entrada
}

void loop() {
  // Leer el estado del sensor PIR
  val = digitalRead(pirPin);  

  // Si el sensor detecta movimiento
  if (val == HIGH) {  
    if (pirState == LOW) {
      Serial.println("Movimiento detectado!");
      pirState = HIGH;  // Actualizar el estado del PIR
    }
  } else {
    if (pirState == HIGH) {
      Serial.println("Movimiento detenido.");
      pirState = LOW;  // Actualizar el estado del PIR
    }
  }
}
