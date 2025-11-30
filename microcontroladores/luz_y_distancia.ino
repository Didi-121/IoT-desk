#include <ESP8266WiFi.h>

// ----------- CONFIG WI-FI -----------
const char* ssid = "emilio";
const char* password = "12345678";

const char* serverHost = "172.20.10.3"; 
const int serverPort = 3000;

// ----------- ULTRASONICO ----------
#define TRIG_PIN D5
#define ECHO_PIN D6

// ----------- FOTORESISTENCIA ----------
const int ldrPin = A0;
int thresholdLuz = 300;   // Ajusta según tu ambiente

// ----------- LEDS ----------
#define RED_LED   D3
#define GREEN_LED D1
#define LED_TEST  D2
#define BLUE_LED  D7   // Indicador de oscuridad

// ----------- ESTADOS ANTERIORES ----------
bool lastOscuro = false;     // Detectar cambios luz→oscuro / oscuro→luz
String lastLedDist = "ninguno";

// =========================================================
// ENVÍO DE JSON
// =========================================================
void sendJSON(float dist, int luz, bool oscuro, String ledDist) {
  WiFiClient client;

  if (!client.connect(serverHost, serverPort)) {
    Serial.println("❌ Error conectando al servidor");
    return;
  }

  String json = "{";
  json += "\"distancia_cm\": " + String(dist, 1) + ",";
  json += "\"luz_raw\": " + String(luz) + ",";
  json += "\"oscuro\": " + String(oscuro ? "true" : "false") + ",";
  json += "\"led_distancia\": \"" + ledDist + "\",";
  json += "\"led_luz\": " + String(oscuro ? "true" : "false");
  json += "}";

  client.println("POST /data HTTP/1.1");
  client.print("Host: ");
  client.println(serverHost);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.print(json);

  Serial.println("📤 Evento enviado al servidor:");
  Serial.println(json);
  Serial.println();
  client.stop();
}

// =========================================================
// SETUP
// =========================================================
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(LED_TEST, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  pinMode(ldrPin, INPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH); // apagado (activo en LOW)
  digitalWrite(LED_TEST, LOW);

  Serial.println("\nConectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// =========================================================
// LOOP PRINCIPAL
// =========================================================
void loop() {

  // ----------- MEDIR DISTANCIA -----------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  float dist = (duration == 0) ? -1 : (duration * 0.034f) / 2.0f;

  // ----------- MEDIR LUZ -----------
  int luz = analogRead(ldrPin);

  bool oscuro = (luz < thresholdLuz);  // Valor bajo = oscuridad

  // LED azul indica oscuridad (activo en LOW)
  digitalWrite(BLUE_LED, oscuro ? HIGH : LOW);

  bool cambioLuz = (oscuro != lastOscuro);


  // ----------- LED DE DISTANCIA (ROJO/VERDE) -----------
  String ledDist = (dist >= 0 && dist <= 15) ? "rojo" : "verde";

  if (ledDist == "rojo") {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }

  bool cambioDist = (ledDist != lastLedDist);


  // =====================================================
  // ENVIAR EVENTO SI CAMBIÓ LUZ O DISTANCIA
  // =====================================================
  if (cambioLuz || cambioDist) {

    if (cambioLuz) {
      if (oscuro) Serial.println("🌑 EVENTO: Se hizo oscuro");
      else        Serial.println("☀️ EVENTO: Se hizo iluminado");
    }

    sendJSON(dist, luz, oscuro, ledDist);

    lastOscuro = oscuro;
    lastLedDist = ledDist;
  }

  delay(100);
}