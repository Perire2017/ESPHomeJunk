#include <SPI.h>
#include <Ethernet2.h>
#include <ArduinoHA.h>
#include <Wire.h>
#include "DFRobot_AHT20.h"
#include <Adafruit_NeoPixel.h>

/* ================== USER CONFIG ================== */

#define W5500_CS 17

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x23, 0x50 };

IPAddress mqttServer(192, 168, 1, 116);
// MQTT credentials (if enabled)
#define BROKER_USERNAME ""
#define BROKER_PASSWORD ""

const char* clientID = "rp2350-aht30";

/* ================== NEOPIXEL ================== */

#define NEOPIXEL_PIN   25
#define NEOPIXEL_COUNT 1

Adafruit_NeoPixel pixel(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* ================== HARDWARE ================== */

#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1

EthernetClient ethClient;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(ethClient, device);

HASensor tempSensor("HouseTemp");
HASensor humiditySensor("HouseHumidity");

DFRobot_AHT20 aht;

/* ================== STATE ================== */

bool ethernetConfigured = false;
bool mqttOK = false;
int  ahtPresent = false;

unsigned long lastEthTry = 0;

/* ================== MQTT ================== */

void reconnectMQTT() {
  if (mqtt.isConnected()) return;

  if (mqtt.begin(mqttServer, BROKER_USERNAME, BROKER_PASSWORD)) {
    mqtt.setKeepAlive(30);
    mqttOK = true;
  } else {
    mqttOK = false;
  }
}

/* ================== ETHERNET ================== */

void checkEthernetIP() {
  static unsigned long lastCheck = 0;
  const unsigned long CHECK_INTERVAL = 2000;

  if (millis() - lastCheck < CHECK_INTERVAL) return;
  lastCheck = millis();

  IPAddress ip = Ethernet.localIP();
  bool hasIP = !(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);

  if (hasIP) {
    ethernetConfigured = true;
  } else {
    ethernetConfigured = false;
    mqttOK = false;   // force MQTT reconnect
  }
}


/* ================== LED HELPERS ================== */

void blinkStatus(unsigned long now, uint8_t r, uint8_t g, uint8_t b) {
  static unsigned long lastBlink = 0;
  static bool ledOn = false;

  if (now - lastBlink >= 500) {
    lastBlink = now;
    ledOn = !ledOn;
    pixel.setPixelColor(0, ledOn ? pixel.Color(r, g, b) : 0);
    pixel.show();
  }
}

void setSolidColor(uint8_t r, uint8_t g, uint8_t b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
}

/* ================== SETUP ================== */

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(W5500_CS, OUTPUT);
  digitalWrite(W5500_CS, HIGH);

  SPI.begin();

  pixel.begin();
  pixel.clear();
  pixel.show();

  Wire.setSDA(I2C_SDA_PIN);
  Wire.setSCL(I2C_SCL_PIN);
  Wire.begin();
  Wire.setClock(100000);

  ahtPresent = aht.begin();
  if (ahtPresent != 0) {
    Serial.println("AHT sensor not detected");
  } else {
    Serial.println("AHT sensor detected");
    aht.startMeasurementReady();
    delay(180);
  }
  Ethernet.init(W5500_CS);

  Serial.println("Starting Ethernet...");
  if (Ethernet.begin(mac) != 0) {
    ethernetConfigured = true;
    Serial.print("Ethernet IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    ethernetConfigured = false;
    Serial.println("DHCP failed");
  }

}

/* ================== LOOP ================== */

void loop() {
  unsigned long now = millis();

  /* ---------- ETHERNET ---------- */
  checkEthernetIP();

  /* ---------- MQTT LOOP (5s) ---------- */
  static unsigned long lastMQTTLoop = 0;
  const unsigned long MQTT_LOOP_INTERVAL = 5000;

  if (now - lastMQTTLoop >= MQTT_LOOP_INTERVAL) {
    lastMQTTLoop = now;

    if (mqtt.isConnected()) {
      mqtt.loop();
      mqttOK = true;
    } else {
      mqttOK = false;
      reconnectMQTT();
    }
  }

  /* ---------- STATUS FLAGS ---------- */
  bool ethernetOK = ethernetConfigured;
  bool sensorOK   = (ahtPresent == 0);

  /* ---------- LED STATUS (PRIORITY) ---------- */
  if (!ethernetOK) {
    blinkStatus(now, 0, 0, 50);          // 🔵 Ethernet down
  }
  else if (!mqttOK) {
    blinkStatus(now, 50, 50, 0);         // 🟡 MQTT down
  }
  else if (!sensorOK) {
    blinkStatus(now, 50, 0, 50);         // 🟣 Sensor error
  }
  else {
    setSolidColor(0, 50, 0);             // 🟢 All OK
  }

  /* ---------- SENSOR READ (1s) ---------- */
static bool measurementStarted = false;
static unsigned long measurementStartTime = 0;

if (sensorOK) {

  if (!measurementStarted) {
    aht.startMeasurementReady(true);
    measurementStarted = true;
    measurementStartTime = now;
  }

  if (measurementStarted && (now - measurementStartTime >= 80)) {
    float tempF = aht.getTemperature_F() - 17.3;
    float rh = aht.getHumidity_RH();
    static char tempBuf[10];
    dtostrf(tempF, 0, 2, tempBuf);
    static char humidityBuf[10];
    dtostrf(rh, 0, 2, humidityBuf);
    tempSensor.setValue(tempBuf);
    humiditySensor.setValue(humidityBuf);
    Serial.print("Temp F: ");
    Serial.println(tempBuf);

    measurementStarted = false; // allow next cycle
  }
}
mqtt.loop();
delay(5000);
}

