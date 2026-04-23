#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const float TEMPON  = 30.0;
const float TEMPOFF = 28.0;
const float HUMON   = 80.0;
const float HUMOFF  = 75.0;

const int LEDTEMP = 9;
const int LEDHUM  = 10;
const int BUZZER  = 8;

bool alertaTemp = false;
bool alertaHum  = false;

void setup() {
  dht.begin();
  pinMode(LEDTEMP, OUTPUT);
  pinMode(LEDHUM, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);
  Serial.println("Tiempos,TempC,Hum,ATemp,AHum");
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error sensor no conectado");
    delay(2000);
    return;
  }

  // Histéresis temperatura
  if (!alertaTemp && t >= TEMPON) {
    alertaTemp = true;
  }
  if (alertaTemp && t <= TEMPOFF) {
    alertaTemp = false;
  }

  // Histéresis humedad
  if (!alertaHum && h >= HUMON) {
    alertaHum = true;
  }
  if (alertaHum && h <= HUMOFF) {
    alertaHum = false;
  }

  digitalWrite(LEDTEMP, alertaTemp ? HIGH : LOW);
  digitalWrite(LEDHUM,  alertaHum  ? HIGH : LOW);
  digitalWrite(BUZZER,  (alertaTemp || alertaHum) ? HIGH : LOW);

  Serial.print(millis() / 1000);
  Serial.print(",");
  Serial.print(t, 1);
  Serial.print(",");
  Serial.print(h, 1);
  Serial.print(",");
  Serial.print(alertaTemp);
  Serial.print(",");
  Serial.println(alertaHum);

  delay(2000);  // DHT11 necesita ~1s mínimo entre lecturas
}