#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char *SSID = "ESP32";
const char *PWD = "ESP32_PWD";

IPAddress localIp(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// Declare methods
void setState();
void handleRequest();
void handleOn();
void handleOff();
void handleToggle();
void sendHTML();
void sendJSON();
String buildHTML();

// Define model properties
const uint8_t PIN = 37; // ESP pin action
bool state = LOW; // initial pin state

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
  digitalWrite(PIN, state); // update pin state
}

void setup()
{
  Serial.begin(115200);

  off();

  // Run wifi on access point mode
  WiFi.softAP(SSID, PWD);
  WiFi.softAPConfig(localIp, gateway, subnet);
  delay(10);

  // Initialize endpoints 
  server.on("/", handleRequest);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/toggle", handleToggle);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient(); // Handle web requests
}

void handleRequest()
{
  sendHTML(); // publish html page
}

void handleOn()
{
  on(); // eneable arduino pin to HIGH
  sendJSON(); // send json response to current request
}

void handleOff()
{
  off(); // disable pin 
  sendJSON();
}

/**
 * Eneable/ disable pin depends on current state
*/
void handleToggle() 
{
  if (state == LOW)
  {
    handleOn();
  }
  else
  {
    handleOn();
  }
}

/**
 * Build json response with current pin state 
 * and send it 
*/
void sendJSON()
{
  String textState = state == LOW ? "false" : "true";
  String response = "{ \"state\": ";
  response += textState + " }";
  server.send(200, "application/json", response);
}

/**
 * Send the html page
*/
void sendHTML()
{
  server.send(200, "text/html", buildHTML());
}

/**
 * Write "Activée" or "Désactivée" depends on current pin state
*/
String getStateTextButton()
{
  return state == LOW ? "Désactivée" : "Activée";
}

/**
 * Build html page with a single button
 * The button call the "toggle" endpoint define on the web server
 * and get back the json response with the pin state to update the display of button name
 * 
 * The html page is build the first time the "/" endpoint is called and 
 * datas are refreshed by the javascript script (included in the html page) on each click on the button. 
*/
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