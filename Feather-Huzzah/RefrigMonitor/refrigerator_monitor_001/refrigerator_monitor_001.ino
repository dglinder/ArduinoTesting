#include <ESP8266WiFi.h>
#include <Ethernet.h>
#include <WiFiClientSecure.h>

const char* ssid     = "home714n1";
const char* password = "linderhome714";

const char* host = "www.adafruit.com";

EthernetClient client;

String AppPWD = "nyricnbdxzhvnmwj"; // From https://security.google.com/settings/security/apppasswords
String AppUID = "dan@linder.org";
String GMailAPI = "https://www.googleapis.com/auth/gmail.compose";

String smtp = "smtp.cox.net"; //SMTP server address
char smtp_arr[] = "smtp.cox.net";
String smtp_port_str = "587"; //smtp server port with TLS enabled (465 for SSL)
int smtp_port = 587;
String to_email_addr = "dan+refrigmon@linder.org"; // destination email address
String from_email_addr = "dan+refrigmon@linder.org"; //source email address

void waitforresponse() {
  long starttime = millis();
  while (!client.available()) {
    delay(1);
    if ((millis() - starttime) > 10000) {
      Serial.println("Timeout");
      break;
    }
  }

  while (client.available()) {
    char c = client.read();
    Serial.print(c);
    delay(1);
  } //end while
  Serial.println();
}

void sendemail() {
  //connect to SMTP email server.
  Serial.print("connecting SMTP... ");

  // if you get a connection, report back via serial:
  if (client.connect(smtp_arr, smtp_port)) {
    Serial.println("connected");
    waitforresponse();

    client.print("HELO "); // say hello
    client.println(from_email_addr);
    waitforresponse();

    // client.println("AUTH LOGIN"); // Authentication
    // waitforresponse();

    //    client.println(emaillogin); // user name encoded
    //    waitforresponse();

    //    client.println(password); // password encoded
    //    waitforresponse();


    client.print("MAIL FROM:<"); // identify sender
    client.print(from_email_addr);
    client.println(">");
    waitforresponse();

    client.print("RCPT TO: <"); // identify recipient
    client.print(to_email_addr);
    client.println(">");
    waitforresponse();

    client.println("DATA");
    waitforresponse();

    // start of email
    client.print("To: ");
    client.println(to_email_addr);
    client.print("From: ");
    client.println(from_email_addr);
    client.println("Subject: External IP Address");

    //    client.print("YOUR IP ADDRESS IS ");
    //    for(char j = 0; j < 17; j++)
    //    { client.print(externalIP[j]);
    //    }//endfor

    client.println("This is a test email.");
    client.println();
    client.println("."); // end of email
    waitforresponse();

    client.println("QUIT"); // terminate connection
    Serial.println("Email Complete");
  }//endif connect

  if (!client.connected())
  { Serial.println();
    Serial.println("Disconnected!!!!!!");
  }//endif

} //end sendemail

void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  if (0) { // Test connect to web server.
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    // We now create a URI for the request
    String url = "/testwifi/index.html";
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(500);

    // Read all the lines of the reply from server and print them to Serial
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  sendemail();

  Serial.println();
  Serial.println("closing connection");

  while (1) {
    delay(100); // Sit here forever...
  }
}
