#include <ESP8266WiFi.h>

// =======================
// CONFIG WI-FI
// =======================
const char* ssid = "emilio";
const char* password = "12345678";

const char* serverHost = "172.20.10.3"; 
const int serverPort = 3000;

// =======================
//      LDR + LED NORMAL
// =======================
const int ldrPin = A0;
const int led = D1;
int thresholdValue = 100;
bool lastOscuro = false;

// =======================
//      LED RGB
// =======================
#define RED_LED   D4
#define GREEN_LED D2
#define BLUE_LED  D3

String lastDistState = "none";

// =======================
//     ULTRASONICO
// =======================
#define TRIG D5
#define ECHO D6


// =======================
//   WiFi + HTTP JSON
// =======================
void sendToServer(String endpoint, String json) {
  WiFiClient client;

  if (!client.connect(serverHost, serverPort)) {
    Serial.println("❌ Error conectando al servidor");
    return;
  }

  client.println("POST " + endpoint + " HTTP/1.1");
  client.print("Host: ");
  client.println(serverHost);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.print(json);

  Serial.println("📤 Enviado a " + endpoint + ": " + json);
  client.stop();
}


// =======================
//         SETUP
// =======================
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(ldrPin, INPUT);
  pinMode(led, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.println("=== SISTEMA INICIADO ===");

  Serial.println("\nConectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}



// =======================
//          LOOP
// =======================
void loop() {

  // =======================
  // LDR
  // =======================
  int luz = analogRead(ldrPin);
  bool oscuro = (luz < thresholdValue);

  Serial.print("Luz: ");
  Serial.println(luz);

  if (oscuro) digitalWrite(led, HIGH);
  else digitalWrite(led, LOW);

  // Si cambia el estado de luz → enviar al servidor
  if (oscuro != lastOscuro) {
    String jsonLuz = "{";
    jsonLuz += "\"value\": " + String(luz) + ",";
    jsonLuz += "\"state\": " + String(oscuro ? 1 : 0);
    jsonLuz += "}";
    sendToServer("/iot/api/light/data", jsonLuz);
    lastOscuro = oscuro;
  }


  // ============================
  // Ultrasonico + LED RGB
  // ============================
  digitalWrite(TRIG, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  Serial.print("Distancia: ");
  Serial.println(distance);

  String distState = (distance <= 5 && duration != 0) ? "cerca" : "seguro";
  int stateValue = (distState == "cerca") ? 1 : 0;

//  if (duration == 0 || distance <= 0) {
//    digitalWrite(RED_LED, LOW);
//    digitalWrite(GREEN_LED, LOW);
//    Serial.println("RGB APAGADO");
//  } else if
  if (distState == "cerca") {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("RGB: ROJO (muy cerca)");
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("RGB: VERDE (distancia segura)");
  }

  // Si cambia el estado de distancia → enviar al servidor
  if (distState != lastDistState) {
    String jsonDist = "{";
    jsonDist += "\"value\": " + String(distance, 1) + ",";
    jsonDist += "\"state\": " + String(stateValue);
    jsonDist += "}";
    sendToServer("/iot/api/distance/data", jsonDist);
    lastDistState = distState;
  }

  Serial.println("-------------------------");
  delay(300);
}
