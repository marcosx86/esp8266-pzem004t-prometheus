
#include "ESP8266WebServer.h"
#include "ESP8266Wifi.h"
#include "PZEM004Tv30.h"
#include <SoftwareSerial.h>

// ===== WIFI CONFIG =====
const char* ssid = "REDACTED";
const char* password = "REDACTED";

// ===== PZEM SERIAL =====
// D1 = GPIO5 (RX), D2 = GPIO4 (TX)
SoftwareSerial pzemSWSerial(4, 5f);
PZEM004Tv30 pzem(pzemSWSerial);

// ===== WEB SERVER =====
ESP8266WebServer server(80);

// ===== METRICS HANDLER =====
void handleMetrics() {
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power   = pzem.power();
  float energy  = pzem.energy();
  float freq    = pzem.frequency();
  float pf      = pzem.pf();

  String metrics = "";

  metrics += "# HELP pzem_voltage Volts\n";
  metrics += "# TYPE pzem_voltage gauge\n";
  metrics += "pzem_voltage " + String(voltage) + "\n";

  metrics += "# HELP pzem_current Amps\n";
  metrics += "# TYPE pzem_current gauge\n";
  metrics += "pzem_current " + String(current) + "\n";

  metrics += "# HELP pzem_power Watts\n";
  metrics += "# TYPE pzem_power gauge\n";
  metrics += "pzem_power " + String(power) + "\n";

  metrics += "# HELP pzem_energy kWh\n";
  metrics += "# TYPE pzem_energy counter\n";
  metrics += "pzem_energy " + String(energy) + "\n";

  metrics += "# HELP pzem_frequency Hz\n";
  metrics += "# TYPE pzem_frequency gauge\n";
  metrics += "pzem_frequency " + String(freq) + "\n";

  metrics += "# HELP pzem_power_factor Ratio\n";
  metrics += "# TYPE pzem_power_factor gauge\n";
  metrics += "pzem_power_factor " + String(pf) + "\n";

  server.send(200, "text/plain", metrics);
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  pzemSWSerial.begin(9600);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Routes
  server.on("/metrics", handleMetrics);
  server.begin();

  Serial.println("HTTP server started");
}

// the loop function runs over and over again forever
void loop() {
  server.handleClient();
}
