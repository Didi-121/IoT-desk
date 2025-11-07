#include <ESP8266WiFi.h>
#include "DHT.h"

// ----------- CONFIG WI-FI -----------
const char* ssid = "emilio";
const char* password = "12345678";

const char* serverHost = "172.20.10.5"; // Tu PC
const int serverPort = 3000;

// ----------- ULTRASONICO ----------
#define trigPin D5
#define echoPin D6
#define buzzerPin D2
long duration;
float distance;

// ----------- FOTORESISTENCIA ----------
const int ldrPin = A0;
const int ledLuz = D1;

// ----------- DHT11 ----------
#define DHTPIN 14  // D5 físico → GPIO14
#define DHTTYPE DHT11
#define ledTemp D6
DHT dht(DHTPIN, DHTTYPE);

// ----------- SENSOR PIR ----------
const int PIR = D3;
const int ledPIR = D7; // Puedes cambiar si lo necesitas

// ----------- POST TIMING ----------
unsigned long lastPostTime = 0;
unsigned long interval = 500; // medio segundo

void setup() {
  Serial.begin(115200);

  // Pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(ledLuz, OUTPUT);
  pinMode(ledTemp, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(ledPIR, OUTPUT);

  dht.begin();

  // WIFI
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++intentos > 20) {
      Serial.println("\nNo se pudo conectar, reiniciando...");
      delay(2000);
      ESP.restart();
    }
  }

  Serial.println("\nWiFi conectado ✅");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // ------ ULTRASONICO ------
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000UL);
  if (duration == 0) distance = -1;
  else distance = (duration * 0.034) / 2;

  if (distance > 0 && distance <= 10) tone(buzzerPin, 3000);
  else noTone(buzzerPin);

  // ------ FOTORESISTENCIA ------
  int luz = analogRead(ldrPin);
  if (luz < 100) digitalWrite(ledLuz, HIGH);
  else digitalWrite(ledLuz, LOW);

  // ------ DHT11 ------
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  if (!isnan(temperatura) && !isnan(humedad)) {
    digitalWrite(ledTemp, temperatura >= 24 ? HIGH : LOW);
  }

  // ------ PIR ------
  int movimiento = digitalRead(PIR);
  digitalWrite(ledPIR, movimiento);

  // ------ ENVÍO JSON CADA 500 ms ------
  if (millis() - lastPostTime >= interval) {
    lastPostTime = millis();

    WiFiClient client;
    if (!client.connect(serverHost, serverPort)) {
     Serial.println("Error conectando al servidor");
      return;
    }

    String json = "{";
//    json += "\"ultrasonico_cm\": " + String(distance, 2) + ",";
    json += "\"luz_raw\": " + String(luz);
//    json += "\"temperatura_c\": " + String(temperatura, 1) + ",";
//    json += "\"humedad_pct\": " + String(humedad, 1) + ",";
//    json += "\"movimiento\": " + String(movimiento);
json += "}";

    client.println("POST /data HTTP/1.1");
    client.print("Host: ");
    client.println(serverHost);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println();
    client.print(json);

    while (client.connected() && !client.available()) delay(1);
    while (client.available()) Serial.println(client.readStringUntil('\n'));
    client.stop();

    Serial.println("JSON enviado ✅ -> " + json + "\n");
  }
}
