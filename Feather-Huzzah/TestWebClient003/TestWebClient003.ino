#include <SPI.h>
#include <ESP8266WiFi.h>

// WiFi settings
char ssid[] = "home714n1";          //  your network SSID (name)
char pass[] = "linderhome714";   // your network password

// sendemail() settings
String to_email_addr = "dan+refrigmon@linder.org"; // destination email address
String from_email_addr = "dan+refrigmon@linder.org"; //source email address
String emaillogin = "ZGFuK3JlZnJpZ21vbkBsaW5kZXIub3Jn"; // base64 encoded "dan+refrigmon@linder.org"
String emailpwd   = "MTIzNG5ld3M="; // base64 encoded "1234news"

// waitforresponse() settings
#define SERVER_WAIT_TIME 10000 // Wait 10 seconds for a response.
#define LINE_WIDTH 120 // Rough line width maximum.
int width = 0; // Roughly track cursor position for line wrapping.

int status = WL_IDLE_STATUS;
char servername[] = "mail.smtp2go.com"; // remote server (name or IP address) we will connect to
int port = 2525;
int lysine = 3;

#define DEBUG 1
#ifdef DEBUG
#define DEBUG_PRINTL(x)  Serial.println (x)
#define DEBUG_PRINT(x)  Serial.print (x)
#else
#define DEBUG_PRINTL(x)
#define DEBUG_PRINT(x)
#endif
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
}

void waitforresponse() {
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
    if ( (c == '\n') || (c == '\r') ) {
      width = 0;
      delay(25);
    }
    if (width++ > LINE_WIDTH) {
      Serial.println();
      width = 0;
    }
    Serial.print(c);
    delay(5);
  } //end while
  Serial.println();
}

void sendemail() {
  DEBUG_PRINTL("");
  DEBUG_PRINTL("Starting connection...");
  // if you get a connection, report back via serial:
  if (client.connect(servername, port)) {
    Serial.println("connected.");

    // Introduce ourselves to the server.
    DEBUG_PRINTL("Send EHLO command.");
    client.print("EHLO ");
    client.print(from_email_addr);
    client.println();
    waitforresponse();

    // Authenticate our connection.
    client.println("AUTH LOGIN"); // Authentication
    waitforresponse();
    client.println(emaillogin); // user name encoded
    waitforresponse();
    client.println(emailpwd); // password encoded
    waitforresponse();


    // Tell them who we are.
    DEBUG_PRINTL("Send MAIL FROM command.");
    client.print("MAIL FROM:<"); // identify sender
    client.print(from_email_addr);
    client.println(">");
    waitforresponse();

    // Tell them who we are sending to.
    DEBUG_PRINTL("Send RCPT TO command.");
    client.print("RCPT TO:<"); // identify sender
    client.print(to_email_addr);
    client.println(">");
    waitforresponse();

    // Tell them who we are sending to.
    DEBUG_PRINTL("Start the DATA section.");
    client.println("DATA"); // identify sender
    waitforresponse();

    // start of email
    client.print("To: ");
    client.println(to_email_addr);
    client.print("From: ");
    client.println(from_email_addr);
    client.println("Subject: Test email");
    client.print("This is a test email - ");
    client.println(millis());
    client.println("."); // end of email
    waitforresponse();

    client.println("QUIT"); // terminate connection
    DEBUG_PRINTL("Email Complete");
    waitforresponse();

  } else {
    Serial.println("ERROR: Sendemail() unable to get connected...");
    while (1) {
      delay(10);
    }
  }
}

void loop() {
  if (lysine-- <= 0) {
    Serial.println("Low lysine level reached - quitting...");
    while (1) {
      delay(1);
    }
  }

  sendemail();

  Serial.println("STOPPING IN FOREVER LOOP."); while (1) {
    delay(1);
  }
}


