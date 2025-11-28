#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>

// ----------- Config wifi
const char* ssid = "emilio";
const char* password = "12345678";
const char* serverHost = "172.20.10.5"; 
const int serverPort = 3000;

// --- Actuadores
LiquidCrystal lcd(5, 4, 14, 12, 13, 16); // Display
const int POT_PIN = A0;
// Potenciometro
const int BTN_PIN = 2;        // Boton
const int BUZZER_PIN = 0;
// Buzzer

// --- Constantes logicas 
const unsigned long ONE_SECOND = 1000;

// --- Variables
unsigned long lastTick = 0;
long totalSeconds = 0;
int selectedMinutes = 0;
int selectedSeconds = 0;

int lastBtnState = HIGH;
// Estados del Programa
enum State {
  SET_MINUTES, // 1 - Seleccionar Minutos
  SET_SECONDS, // 2 - Seleccionar Segundos
  RUNNING,     // 3 - Cuenta regresiva
  PAUSED,      // 4 - Pausa
  FINISHED     // 5 - Finalizado 
};
State currentState = SET_MINUTES;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicialización del LCD 
  lcd.begin(16, 2);
  delay(50);
  // Doble inicio para estabilidad y evitar errores con el microcontrolador
  lcd.begin(16, 2);
  
  lcd.clear();
  lcd.print("Conectando WiFi...");

  // Conexión WiFi
  WiFi.begin(ssid, password);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++intentos > 20) {
      break; // Continuar aunque falle la conexión
    }
  }
  Serial.println("\nWiFi conectado");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("Sistema encendido");
  delay(1000);
  lcd.clear();
  pinMode(BTN_PIN, INPUT_PULLUP);
  
  // Configurar Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Asegurar silencio al inicio
  noTone(BUZZER_PIN);
}

void loop() {
  // El manejador del botón revisa cada click 
  handleButton();
  switch (currentState) {
    case SET_MINUTES:
      logicSetMinutes();
      break;
    case SET_SECONDS:
      logicSetSeconds();
      break;
      
    case RUNNING:
      logicRunning();
      break;
    case PAUSED:
      logicPaused();
      break;
      
    case FINISHED:
      logicFinished();
      break;
  }
}

// ---------------- Funciones para el manejo de estados 

void logicSetMinutes() {
  int val = analogRead(POT_PIN);
  // Mapeo con zona muerta (+- 3)
  int rawMinutes = map(val, 0, 1024, -3, 63);
  selectedMinutes = constrain(rawMinutes, 0, 60);

  // Interfaz
  lcd.setCursor(0, 0);
  lcd.print("Set Minutes:    ");
  
  lcd.setCursor(6, 1);
  if (selectedMinutes < 10) lcd.print("0");
  lcd.print(selectedMinutes);
  lcd.print("      ");
}

void logicSetSeconds() {
  int val = analogRead(POT_PIN);
  // Mapeo con zona muerta (+- 3)
  int rawSeconds = map(val, 0, 1024, -3, 62);
  selectedSeconds = constrain(rawSeconds, 0, 59);

  // Interfaz
  lcd.setCursor(0, 0);
  lcd.print("Set Seconds:    ");
  
  lcd.setCursor(6, 1);
  if (selectedSeconds < 10) lcd.print("0");
  lcd.print(selectedSeconds);
  lcd.print("      ");
}

void logicRunning() {
  // Revisar si paso 1 segundo
  if (millis() - lastTick >= ONE_SECOND) {
    lastTick = millis();
    if (totalSeconds > 0) {
      totalSeconds--;
    } else {
      
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Descansa!");
      tone(BUZZER_PIN, 1000);
      // frecuencia de 1000Hz  
      delay(3000); 
      
      noTone(BUZZER_PIN);
      digitalWrite(BUZZER_PIN, LOW);
      // Seguridad extra

      // Cambio de estado
      currentState = FINISHED;
      
      // Enviar estado: FINISHED
      sendButtonState(currentState);

      lcd.clear(); 
    }
  }

  if (currentState == RUNNING) {
    lcd.setCursor(5, 0); 
    printFormattedTime(totalSeconds);
    
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}

void logicPaused() {
  lcd.setCursor(0, 0);
  lcd.print("  --PAUSA--   ");
  
  lcd.setCursor(5, 1);
  printFormattedTime(totalSeconds);
}

void logicFinished() {
  lcd.setCursor(2, 0);
  lcd.print("Timepo de descansar");
  
  lcd.setCursor(0, 1);
  lcd.print(" Presiona de nuevo  ");
}

void handleButton() {
  int reading = digitalRead(BTN_PIN);
  // Detectar pulsación 
  if (reading == LOW && lastBtnState == HIGH) {
    delay(50);
    if (digitalRead(BTN_PIN) == LOW) {
      transitionState();
    }
  }
  lastBtnState = reading;
}

void transitionState() {
  lcd.clear(); // Siempre limpiar al cambiar
  
  switch (currentState) {
    case SET_MINUTES:
      currentState = SET_SECONDS;
      // Enviar estado: SET_SECONDS
      sendButtonState(currentState);
      
      // Esperar a que suelte el botón
      while(digitalRead(BTN_PIN) == LOW); 
      break;
    case SET_SECONDS:
      // Calcular tiempo total e iniciar
      totalSeconds = (selectedMinutes * 60) + selectedSeconds;
      lastTick = millis(); 
      currentState = RUNNING;
      
      // Enviar datos del timer
      sendTimerData(selectedMinutes, selectedSeconds);
      
      // Enviar estado: RUNNING
      sendButtonState(currentState);
      break;
      
    case RUNNING:
      currentState = PAUSED;
      // Enviar estado: PAUSED
      sendButtonState(currentState);
      break;
    case PAUSED:
      currentState = RUNNING;
      lastTick = millis();
      // Resincronizar para no saltar tiempo
      
      // Enviar estado: RUNNING
      sendButtonState(currentState);
      break;
    case FINISHED:
      // Reiniciar todo
      selectedMinutes = 0;
      selectedSeconds = 0;
      currentState = SET_MINUTES;
      // Enviar estado: SET_MINUTES
      sendButtonState(currentState);
      break;
  }
}

void printFormattedTime(long val) {
  int m = val / 60;
  int s = val % 60;

  if (m < 10) lcd.print("0");
  lcd.print(m);
  lcd.print(":");
  if (s < 10) lcd.print("0");
  lcd.print(s);
}

// Función auxiliar para enviar datos del temporizador
void sendTimerData(int minutes, int seconds) {
  String json = "{";
  json += "\"minutes\": " + String(minutes) + ",";
  json += "\"seconds\": " + String(seconds);
  json += "}";
  sendPostRequest("/iot/api/timer/data", json);
}

// Función auxiliar para enviar estado del botón
void sendButtonState(int state) {
  String json = "{\"state\": " + String(state) + "}";
  sendPostRequest("/iot/api/button/data", json);
}

// Enviar POST al servidor
void sendPostRequest(String endpoint, String json) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    if (client.connect(serverHost, serverPort)) {
      client.println("POST " + endpoint + " HTTP/1.1");
      client.print("Host: ");
      client.println(serverHost);
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(json.length());
      client.println();
      client.print(json);

      // Espera para asegurar envío
      unsigned long timeout = millis();
      while (client.connected() && !client.available()) {
        if (millis() - timeout > 1000) break;
        delay(1);
      }
      client.stop();
      Serial.println("Enviado a " + endpoint + ": " + json);
    } else {
      Serial.println("Error conectando al servidor");
    }
  }
}