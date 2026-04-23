#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int LED = 13;
const int BUZZ = 8;
const int LDR  = A0;

char buffer[64];
int bufIdx = 0;

void responderJSON(const String &cmd, const String &data) {
  Serial.print("{\"cmd\":\"");
  Serial.print(cmd);
  Serial.print("\",\"data\":");
  Serial.print(data);
  Serial.println("}");
}

void procesarComando(String cmd) {
  cmd.trim();

  if (cmd == "TEMP") {
    float t = dht.readTemperature();
    if (isnan(t)) {
      responderJSON("TEMP", "null");
    } else {
      responderJSON("TEMP", String(t, 1));
    }

  } else if (cmd == "HUM") {
    float h = dht.readHumidity();
    if (isnan(h)) {
      responderJSON("HUM", "null");
    } else {
      responderJSON("HUM", String(h, 1));
    }

  } else if (cmd == "LUZ") {
    int l = analogRead(LDR);
    responderJSON("LUZ", String(l));

  } else if (cmd.startsWith("LED")) {
    String val = cmd.substring(4); // LED:ON ó LED:OFF
    val.trim();
    if (val == "ON") {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }
    responderJSON("LED", "\"" + val + "\"");

  } else if (cmd == "ALL") {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int   l = analogRead(LDR);

    String d = "{";
    d += "\"temp\":"; d += (isnan(t) ? "null" : String(t, 1));
    d += ",\"hum\":"; d += (isnan(h) ? "null" : String(h, 1));
    d += ",\"luz\":"; d += String(l);
    d += "}";

    responderJSON("ALL", d);

  } else {
    responderJSON("ERR", "\"Comando desconocido: " + cmd + "\"");
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED, OUTPUT);
  pinMode(BUZZ, OUTPUT);

  Serial.println("{\"status\":\"ready\",\"version\":\"1.0\"}");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      if (bufIdx > 0) {
        buffer[bufIdx] = '\0';
        procesarComando(String(buffer));
        bufIdx = 0;
      }
    } else {
      if (bufIdx < 63) {
        buffer[bufIdx++] = c;
      } else {
        // buffer lleno, reiniciar
        bufIdx = 0;
      }
    }
  }

  // Aquí podrías hacer otras tareas en paralelo si quieres
}