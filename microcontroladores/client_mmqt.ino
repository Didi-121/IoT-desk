#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ----------- WiFi -----------
const char* ssid = "";
const char* password = "";

// ----------- MQTT -----------
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic_publish = "IoTTC1004B/sensor";   // envío del potenciómetro
const char* topic_subscribe = "IoTTC1004B/color";  // recibir color del LED

WiFiClient espClient;
PubSubClient client(espClient);

// ----------- Hardware -----------
const int potPin = A0;
const int pinR = D4;
const int pinG = D5;
const int pinB = D6;

unsigned long lastMsg = 0;
unsigned long interval = 1000;

String ultimoColor = "off";

// ========================== LED ==========================

void apagarLED() {
  digitalWrite(pinR, HIGH);
  digitalWrite(pinG, HIGH);
  digitalWrite(pinB, HIGH);
}
void aplicarColor(String color) {

  Serial.print("Aplicando color: ");
  Serial.println(color);

  if (color.equals("red")) {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, HIGH);
  }
  else if (color.equals("green")) {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, HIGH);
  }
  else if (color.equals("blue")) {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, LOW);
  }
  else if (color.equals("yellow")) {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, HIGH);
  }
  else if (color.equals("cyan")) {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, LOW);
  }
  else if (color.equals("magenta")) {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, LOW);
  }
  else if (color.equals("white")) {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, LOW);
  }
  else if (color.equals("off")) {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, HIGH);
  }
}



// ========================== WiFi ==========================

void setup_wifi() {
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}


// ========================== MQTT CALLBACK ==========================

void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  msg.trim();

  Serial.print("\nMensaje MQTT recibido -> ");
  Serial.println(msg);

  if (String(topic) == topic_subscribe) {
    ultimoColor = msg;
    aplicarColor(ultimoColor);  
  }
}


// ========================== MQTT RECONNECT ==========================

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT... ");

    if (client.connect("NodeMCU_Alumno")) {
      Serial.println("Conectado!");
      client.subscribe(topic_subscribe);
    } else {
      Serial.print("Error (rc=");
      Serial.print(client.state());
      Serial.println("). Reintentando...");
      delay(2000);
    }
  }
}


// ========================== SETUP ==========================

void setup() {
  Serial.begin(115200);

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // apagar LED al inicio
  digitalWrite(pinR, HIGH);
  digitalWrite(pinG, HIGH);
  digitalWrite(pinB, HIGH);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


// ========================== LOOP ==========================

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();

  if (now - lastMsg >= interval) {
    lastMsg = now;

    int valor = analogRead(potPin);
    String mensaje = String(valor);

    Serial.print("Enviando potenciómetro: ");
    Serial.println(mensaje);

    client.publish(topic_publish, mensaje.c_str());


    aplicarColor(ultimoColor);
  }
}