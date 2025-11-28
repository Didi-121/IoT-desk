#include <DHT.h>
#include <ESP8266WiFi.h>

// ----------- CONFIG WI-FI -----------
const char* ssid = "Maria";
const char* password = "12345678";

const char* serverHost = "172.20.10.3"; // Tu PC
const int serverPort = 3000;

// --------- CONFIGURACIÓN DE PINES ---------
#define DHTPIN 14        // GPIO14 -> pin D5 en NodeMCU
#define DHTTYPE DHT11    // Tipo de sensor

#define RELAY_PIN 5      // GPIO5 -> pin D1 en NodeMCU (IN del relay)

// --------- UMBRALES DE TEMPERATURA ---------
float TEMP_ON  = 27.0;   // Enciende ventilador al llegar/pasar esta temp
float TEMP_OFF = 26.9;   // Apaga ventilador al bajar a esta temp o menos

bool fanOn = false;

DHT dht(DHTPIN, DHTTYPE);

// ------ ENVÍO JSON CADA 500 ms ------
unsigned long lastPostTime = 0;
const unsigned long interval = 500;   // 500 ms

// ========= SETUP =========
void setup() {
  Serial.begin(115200);
  Serial.println("Sistema: DHT11 + Relay + Ventilador");

  dht.begin();

  // Al inicio: ventilador apagado → pin en INPUT (flotante)
  // Así el relay ve IN ~5V y se queda OFF
  pinMode(RELAY_PIN, INPUT);

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
      ESP.restart();
    }
  }

  Serial.println("\nWiFi conectado ✅");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ========= LOOP PRINCIPAL =========
void loop() {
  float t = dht.readTemperature(); // Lectura en °C

  if (isnan(t)) {
    Serial.println("Error leyendo DHT");
    delay(2000);
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C");

  // ----- Lógica con histéresis -----
  // Relay: ACTIVO EN LOW
  // ON  -> pin como OUTPUT LOW
  // OFF -> pin como INPUT (flotante, el módulo lo lleva a ~5V)
  if (!fanOn && t >= TEMP_ON) {
    // Encender ventilador
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);    // activo en LOW → ON
    fanOn = true;
    Serial.println(">> Ventilador ENCENDIDO");
  }
  else if (fanOn && t <= TEMP_OFF) {
    // Apagar ventilador
    pinMode(RELAY_PIN, INPUT);       // dejamos el pin flotando → OFF
    fanOn = false;
    Serial.println(">> Ventilador APAGADO");
  }

  // ------ ENVÍO JSON CADA 500 ms ------
  if (millis() - lastPostTime >= interval) {
    lastPostTime = millis();
  
    WiFiClient client;
    if (!client.connect(serverHost, serverPort)) {
      Serial.println("❌ Error conectando al servidor");
      return;
    }
  
    int valorEntero = (int)t;
  
    String json = "{";
    json += "\"temperature\": " + String(valorEntero) + ",";
    json += "\"estado\": " + String(fanOn ? "true" : "false");
    json += "}";
  
    client.println("POST /iot/api/temperature/data HTTP/1.1");
    client.print("Host: ");
    client.println(serverHost);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(json.length());
    client.println();
    client.print(json);
  
    while (client.connected() && !client.available()) delay(1);
    while (client.available()) {
      Serial.println(client.readStringUntil('\n'));
    }
    client.stop();
  
    Serial.println("JSON enviado ✅ -> " + json + "\n");
  }

  // Un pequeño delay opcional para no spamear Serial demasiado
  delay(200);
}
