#define PINTIME 1000

char *ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println ("Cycle the LED through the range before starting.");
  for (int pinnum = 0; pinnum <= 1024; pinnum++) {
    analogWrite(16, pinnum);
    delay(2);
  }
  for (int pinnum = 1024; pinnum >= 0; pinnum--) {
    analogWrite(16, pinnum);
    delay(2);
  }
  Serial.println ("LED test done.");
  delay(PINTIME);
}

int tempA;
float tempA_C;
float tempA_F;
char tmp[10];

void loop() {
  while (1) {
    tempA = analogRead(A0);
    analogWrite(16, tempA);
    Serial.print ("Millis: ");
    Serial.print (millis());
    Serial.print (" --- ");
    Serial.print ("Value: ");
    Serial.print (tempA);

    tempA_C = (( ((float)tempA / 1024 * 1000) - 500) / 10);
    Serial.print (" --- ");
    Serial.print ("Temp C: ");
    dtostrf(tempA_C, 5, 2, tmp);
    Serial.print (tmp);

    tempA_F = (tempA_C * 1.8) + 32;
    Serial.print (" --- ");
    Serial.print ("Temp F: ");
    dtostrf(tempA_F, 5, 2, tmp);
    Serial.print (tmp);

    Serial.println();
    delay(250);
  }
}
