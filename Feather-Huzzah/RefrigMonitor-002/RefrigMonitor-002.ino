#include <SPI.h>
#include <ESP8266WiFi.h>

// WiFi settings
char ssid[] = "home714n1";          //  your network SSID (name)
char pass[] = "linderhome714";   // your network password

// sendemail() settings
String to_email_addr = "dan+refrigmon@linder.org"; // destination email address
String to_sms_addr   = "4029687576@messaging.sprintpcs.com"; // destination text pager address
String from_email_addr = "dan+refrigmon@linder.org"; //source email address
String emaillogin = "ZGFuK3JlZnJpZ21vbkBsaW5kZXIub3Jn"; // base64 encoded "dan+refrigmon@linder.org"
String emailpwd   = "MTIzNG5ld3M="; // base64 encoded "1234news"
char servername[] = "mail.smtp2go.com"; // remote server (name or IP address) we will connect to
int port = 2525;

// waitforresponse() settings
#define SERVER_WAIT_TIME 10000 // Wait 10 seconds for a response.
#define LINE_WIDTH 120 // Rough line width maximum.
int width = 0; // Roughly track cursor position for line wrapping.

// gettemp() settings
#define NTEMPSENSORS 1    // Number of temp sensors
#define NDOORSENSORS 2    // Number of door sensors
#define NTEMPREADINGS 5   // Number of temp readings to keep
#define READINGPAUSE 5    // How often to log a temp reading (seconds)
#define DOOROPENTIME 90   // How long we'll allow a door to be left open
#define DOOROPEN 1        // Value to return when door open
#define DOORCLOSED 0       // Value to return when door closed
#define ALERTPAUSE 600    // Don't send more alerts for this many seconds.
#define DOOR_0_GPIO 12
#define DOOR_1_GPIO 13
#define DOOR_2_GPIO 14

int   TempCursor[NTEMPSENSORS]; // What reading do we update next?
int   TempValues[NTEMPSENSORS][NTEMPREADINGS];
int   DoorOpen[NDOORSENSORS]; // Time in millis() when door was opened, or zero for closed.
float TempHigh[NTEMPSENSORS];
int   DoorTime[NDOORSENSORS]; // Time in seconds door should close by
unsigned long NextAlert = 0UL;

int nextreading = 0;
int status = WL_IDLE_STATUS;
int lysine = 3;

#define DEBUG 1
#define NOMAIL 1  // Set to 1 to stop all emails.

#ifdef DEBUG
#define DEBUG_PRINTL(x)  Serial.println (x)
#define DEBUG_PRINT(x)  Serial.print (x)
#else
#define DEBUG_PRINTL(x)
#define DEBUG_PRINT(x)
#endif
WiFiClient client;

float tempRawToC(int t)
{
  // Convert the raw reading into degrees F
  return (( ((float)t / 1024.0 * 1000.0) - 500.0) / 10.0);
}
float tempCtoF(float t)
{
  return (t * 1.8) + 32.0;
}
float readtemp(int sensor)
{
  // Read temperature at "sensor" and return the Celcius value.
#ifdef DEBUG
  int temp = analogRead(A0);
  DEBUG_PRINT("DEBUG: Temp reading: ");
  DEBUG_PRINT(sensor);
  DEBUG_PRINT(": ");
  DEBUG_PRINT(tempRawToC(temp));
  DEBUG_PRINT(" (");
  DEBUG_PRINT(temp);
  DEBUG_PRINT(")");
  DEBUG_PRINTL();
  delay(100);
#endif
  return tempRawToC(analogRead(A0));
}
int doorstat(int sensor)
{
  // Get status of the door open/close sensor.
  int foo = 0;
  DEBUG_PRINT("Reading from switch ");
  switch (sensor)
  {
    case 0:
      // First door sensor.
      DEBUG_PRINT(0);
      foo = digitalRead(DOOR_0_GPIO);
      break;
    case 1:
      // First door sensor.
      DEBUG_PRINT(1);
      foo = digitalRead(DOOR_1_GPIO);
      break;
    case 2:
      // First door sensor.
      DEBUG_PRINT(2);
      foo = digitalRead(DOOR_2_GPIO);
      break;
    default:
      // Expand this case statement if more than three sensors are needed
      Serial.print   ("ERROR: Called with more than ");
      Serial.print   (NDOORSENSORS);
      Serial.println (" door sensors. Fix the case statement or reduce sensors.");
  }
  DEBUG_PRINT(" value: ");
  DEBUG_PRINT(foo);
  DEBUG_PRINT(" == ");
  if (foo == 0)
  {
    // reed switch open, door open
    DEBUG_PRINTL(DOOROPEN);
    return DOOROPEN;
  }
  else
  {
    DEBUG_PRINTL(DOORCLOSED);
    return DOORCLOSED;
  }
}
void sensors_setup()
{
  // Setup any information necessary
  TempHigh[0] = 7.2;  // 44.9F
  TempHigh[1] = 1.0;  // 33.8F
  DoorTime[0] = 90.0; // seconds
  DoorTime[1] = 90.0; // seconds
}
void gettemp_setup()
{
  // Initialize the stored temperature values.
  for (int s = 0; s < NTEMPSENSORS; s++)
  {
    TempCursor[s] = 0; // Point to first reading for each sensor.
    for (int r = 0; r < NTEMPREADINGS; r++)
    {
      TempValues[s][r] = readtemp(s);
    }
  }
}
void getdoor_setup()
{
  for (int s = 0; s < NDOORSENSORS; s++)
  {
    DoorOpen[s] = DOORCLOSED;
  }
}
void settemp(int sensor, float temp)
{
  // Store the latest temp reading and update the sensors average temp
  int cursor = TempCursor[sensor];
  TempValues[sensor][cursor] = temp;
  TempCursor[sensor]++;
  if (TempCursor[sensor] >= NTEMPSENSORS)
  {
    TempCursor[sensor] = 0;
  }
}
void wifi_setup()
{
  Serial.println("Attempting to connect to WiFi network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Connecting.");

  status = WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
}
void waitforresponse()
{
  // Wait a bit for the connection to become available.
  long starttime = millis();
  while (!client.available())
  {
    delay(1);
    if ((millis() - starttime) > SERVER_WAIT_TIME)
    {
      Serial.println("ERROR: Timeout waiting for response.");
      while (1)
      {
        delay(1); // So the ESP watchdog timer doesn't kick in.
      }
    }
  }

  // Print whatever the server sends back to us.
  while (client.available())
  {
    char c = client.read();
    if ( (c == '\n') || (c == '\r') )
    {
      width = 0;
      delay(25);
    }
    if (width++ > LINE_WIDTH)
    {
      Serial.println();
      width = 0;
    }
    Serial.print(c);
    delay(5);
  } //end while
  Serial.println();
}
int sendemail(String send_to, char subject[], char mailbody[])
{
  // if you get a connection, report back via serial:
#if NOMAIL > 0
  DEBUG_PRINTL("DEBUG: NOT sending email in DEBUG mode.");
#else
  DEBUG_PRINTL("");
  DEBUG_PRINTL("Starting connection...");
  if (client.connect(servername, port))
  {
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
    client.print(send_to);
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
    client.print("Subject: ");
    client.println(subject);
    client.println(mailbody);
    client.println("."); // end of email
    waitforresponse();

    client.println("QUIT"); // terminate connection
    DEBUG_PRINTL("Email Complete");
    waitforresponse();

  }
  else
  {
    Serial.println("ERROR: Sendemail() unable to get connected...");
    while (1)
    {
      delay(10);
    }
  }
#endif
}
float tempAverage(int sensor)
{
  // Return the average of the current readings.
  float foo = 0;
  for (int r = 0; r < NTEMPREADINGS; r++)
  {
    foo += TempValues[sensor][r];
  }
  foo /= NTEMPREADINGS;
  // DEBUG DEBUG DEBUG
  foo = TempHigh[sensor];
  return foo;
}
int tempAlarm(int sensor)
{
  // Return 1/TRUE if the temp for a sensor is critical
  if (tempAverage(sensor) > TempHigh[sensor])
  {
    return 1;
  }
  return 0;
}

void setup()
{
  Serial.begin(115200);

  wifi_setup();

  sensors_setup();
  gettemp_setup();
  getdoor_setup();

}

void loop()
{
  delay(50);  // Let the ESP8266 do it's stuff in the background.
  if (millis() > nextreading)
  {
    // Time to take another reading.
    DEBUG_PRINT("Reading time: ");
    DEBUG_PRINT(millis());
    DEBUG_PRINTL("");
    for (int s = 0; s < NTEMPSENSORS; s++)
    {
      float foo = readtemp(s);
      delay(10);
      settemp(s, foo);  // Retrieve and save temp readings for this time.
    }
  }

  for (int s = 0; s < NDOORSENSORS; s++)
  {
    // Check each door and set their open time
    if (doorstat(s) == DOOROPEN)
    {
      // The door is open.
      if ((millis() > DoorTime[s]) && (DoorTime[s] > 0))
      {
        // Time to alert if door time is NOT zero and we're past the delay time.
        NextAlert = millis() + (ALERTPAUSE * 1000);
        char sub[100];
        sprintf(sub, "Door %s is open for more than %i seconds!", s, DOOROPENTIME);
        DEBUG_PRINTL ("Sending email alarm!");
        sendemail(to_email_addr, sub, sub);
      }
    }
    else
    {
      // Door is shut, reset open time.
      DoorTime[s] = 0; //(0UL - 1UL);
    }
  }

  // Check all sensors before compleeting loop.
  DEBUG_PRINTL ("Checking sensors/doors before looping.");
  for (int s = 0; s < NTEMPSENSORS; s++)
  {
    // Check TEMPERATURE of all probes.
    if (tempAlarm(s))
    {
      DEBUG_PRINTL ("TEMP ALARM FOUND!");
      // There is a temperature problem
      if (millis() > NextAlert)
      {
        // Time to alert.
        NextAlert = millis() + (ALERTPAUSE * 1000);
        char foo[7];
        char thresh[7];
        char sub[100];
        dtostrf(tempAverage(s), 4, 2, foo);
        dtostrf(TempHigh[s], 4, 2, thresh);
        sprintf(sub, "Temp on sensor %i: %s above %s!", s, foo, thresh);
        DEBUG_PRINTL ("Sending email alarm!");
        sendemail(to_email_addr, sub, sub);
      }
    }
  }

  for (int s = 0; s < NDOORSENSORS; s++)
  {
    // Check all DOORS
    if (doorstat(s) && (DoorTime[s] > millis()))
    {
      // The door has been open for a long time, alert on it.
      DEBUG_PRINTL ("DOOR ALARM FOUND!");
      if (millis() > NextAlert)
      {
        NextAlert = millis() + (ALERTPAUSE * 1000);
        char sub[100];
        sprintf(sub, "Door open: %i", s);
        DEBUG_PRINTL ("Sending email alarm!");
        sendemail(to_email_addr, sub, sub);
      }
    }
  }


#if NOMAIL <= 0
  // Only hang here if we are sending email.
  Serial.println("STOPPING IN FOREVER LOOP.");
  while (1)
  {
    delay(1);
  }
#endif
  delay(5000);
}


