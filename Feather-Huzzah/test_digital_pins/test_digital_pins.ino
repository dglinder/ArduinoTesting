#define PINTIME 250

void setup() {
  Serial.begin(115200);
  delay(100);
}

int pinnum = 0;

void loop() {
  for (pinnum = 0; pinnum <= 16; pinnum++) {
    if (pinnum == 1) { continue; }
    if (pinnum == 3) { continue; }
    if (pinnum == 6) { continue; }
    if ((pinnum >= 7) && (pinnum <= 11)) { continue; }
    Serial.print("Pin: ");
    Serial.println(pinnum);
    pinMode(pinnum, OUTPUT);
    digitalWrite(pinnum, HIGH);
    delay(PINTIME);
    digitalWrite(pinnum, LOW);
    pinMode(pinnum, INPUT);
  }
}


