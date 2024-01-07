#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *SSID = "ESP32";
const char *PWD = "ESP32_PWD";

IPAddress localIp(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

void setState();
void handleRequest();
void handleOn();
void handleOff();
void handleToggle();
void sendHTML();
void sendJSON();
String buildHTML();

const uint8_t PIN = 37;
bool state = LOW;

void off()
{
  state = LOW;
  setState();
}

void on()
{
  state = HIGH;
  setState();
}

void setState()
{
  digitalWrite(PIN, state);
}

void setup()
{
  Serial.begin(115200);

  off();

  WiFi.softAP(SSID, PWD);
  WiFi.softAPConfig(localIp, gateway, subnet);
  delay(10);

  server.on("/", handleRequest);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/toggle", handleToggle);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

void handleRequest()
{
  sendHTML();
}

void handleOn()
{
  on();
  sendJSON();
}

void handleOff()
{
  off();
  sendJSON();
}

void handleToggle() {
  if(state == LOW) {
    on();
  }
  else {
    off();
  }
  sendJSON();
}

void sendJSON()
{
  String textState = state == LOW ? "false" : "true";
  String response = "{ \"state\": ";
  response += textState + " }";
  server.send(200, "application/json", response);
}

void sendHTML()
{
  server.send(200, "text/html", buildHTML());
}

String getStateTextButton() {
  return state == LOW ? "Désactivée" : "Activée";
}

String buildHTML()
{
  String html = "<!DOCTYPE html>";
html += "<html lang=\"en\">";
html += "<head>";
html += "    <meta charset=\"UTF-8\">";
html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
html += "    <title>Prises connectées</title>";
html += "</head>";
html += "<body>";
html += "    <label for=\"device_1\">Prise n°1 (Guirlande extérieure) </label>";
html += "    <button id=\"device_1\" onclick=\"activer(1)\">" + getStateTextButton() + "</button>";
html += "    <script type=\"application/javascript\">";
html += "        const device_1 = document.getElementById(\"device_1\");";
html += "        const activer = async (name) => {";
html += "            await fetch(\"http://192.168.0.1/toggle\")";
html += "                .then(response => response.json())";
html += "                .then(data => {";
html += "                    if(data.state) {";
html += "                        device_1.textContent = \"Activée\";";
html += "                    }";
html += "                    else {";
html += "                        device_1.textContent = \"Désactivée\";";
html += "                    }";
html += "                })";
html += "        }";
html += "    </script>";
html += "</body>";
html += "</html>";
  return html;
}