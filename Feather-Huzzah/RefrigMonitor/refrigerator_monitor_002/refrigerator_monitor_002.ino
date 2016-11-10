#include <ESP8266WiFi.h>
#include <Ethernet.h>
#include <WiFiClientSecure.h>
 
const char* ssid     = "home714n1";
const char* password = "linderhome714";

const char* AppPWD = "nyricnbdxzhvnmwj"; // From https://security.google.com/settings/security/apppasswords
const char* AppUID = "dan@linder.org";
const char* GMailAPI = "https://www.googleapis.com/auth/gmail.compose";
const char* host = "www.googleapis.com";
#define HOST_URL "/auth/gmail.compose"
#define HOST_PORT 443

IPAddress my_dns(8, 8, 8, 8);

String smtp = "mail.smtp2go.com"; //SMTP server address
//char smtp_arr[] = "216.22.15.249"; //"mail.smtp2go.com";
byte smtp_arr[] = { 216, 22, 15, 249 }; // "mail.smtp2go.com"
String smtp_port_str = "2525"; //smtp server port with TLS enabled (465 for SSL)
int smtp_port = 2525;
String to_email_addr = "dan+refrigmon@linder.org"; // destination email address
String from_email_addr = "dan+refrigmon@linder.org"; //source email address
// Encode at: http://base64-encoder-online.waraxe.us/
String emaillogin = "ZGFuK3JlZnJpZ21vbkBsaW5kZXIub3Jn"; // base64 encoded "dan+refrigmon@linder.org"
String emailpwd   = "MTIzNG5ld3M="; // base64 encoded "1234news"

EthernetClient client;

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
  Serial.print("connecting to ");
  Serial.print(smtp);
  Serial.print(" on port ");
  Serial.print(smtp_port);
  Serial.print(": ");

  // if you get a connection, report back via serial:
  int conn_stat = client.connect(smtp_arr, 2525);
  if (conn_stat >= 1) {
    Serial.println("connected.");
    waitforresponse();

    client.print("HELO "); // say hello
    client.println(from_email_addr);
    waitforresponse();

    client.println("AUTH LOGIN"); // Authentication
    waitforresponse();

    client.println(emaillogin); // user name encoded
    waitforresponse();

    client.println(password); // password encoded
    waitforresponse();

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
    client.println("Subject: Test email");

    //    client.print("YOUR IP ADDRESS IS ");
    //    for(char j = 0; j < 17; j++)
    //    { client.print(externalIP[j]);
    //    }//endfor

    client.print("This is a test email - ");
    client.println(millis());
    client.println("."); // end of email
    waitforresponse();

    client.println("QUIT"); // terminate connection
    Serial.println("Email Complete");
  }//endif connect
  else {
    Serial.print("FAILED, exit code: ");
    Serial.println(conn_stat);
  }

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
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("DNS address: ");
  Serial.println(WiFi.dnsIP());
  Serial.println("++++++++++++++++++++++++++++");
  WiFi.printDiag(Serial);
  Serial.println("++++++++++++++++++++++++++++");

}

int value = 0;

void loop() {
//  delay(5000);
  ++value;

  if (0) { // Test connect to web server.
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = HOST_PORT;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    // We now create a URI for the request
    String url = HOST_URL;
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

  if (1) { // Try via SMTP
    sendemail();
  }
  Serial.println();
  Serial.println("closing connection");

  while (1) {
    delay(100); // Sit here forever...
  }
}
