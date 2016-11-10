#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "home714n1";          //  your network SSID (name)
char pass[] = "linderhome714";   // your network password

#define SERVER_WAIT_TIME 10000 // Wait 10 seconds for a response.

int status = WL_IDLE_STATUS;
char servername[] = "google.com"; // remote server (name or IP address) we will connect to
int port = 80;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println("Attempting to connect to WiFi network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Connecting.");

  status = WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");

  Serial.println("");
  Serial.println("Starting connection...");
  // if you get a connection, report back via serial:
  if (client.connect(servername, port)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();

    // Wait a bit for the connection to become available.
    long starttime = millis();
    while (!client.available()) {
      delay(1);
      if ((millis() - starttime) > SERVER_WAIT_TIME) {
        Serial.println("ERROR: Timeout waiting for response.");
        while (1) {
          delay(1); // So the ESP watchdog timer doesn't kick in.
        }
      }
    }

    // Print whatever the server sends back to us.
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      delay(1);
    } //end while
    Serial.println();

  }
}

void loop() {

}

