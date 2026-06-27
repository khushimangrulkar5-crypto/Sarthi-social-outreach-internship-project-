#define IR_SENSOR 4   // GPIO connected to IR sensor OUT pin

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR, INPUT);
}

void loop() {
  int state = digitalRead(IR_SENSOR);

  if (state == LOW) {
    Serial.println("Object Detected");
  } else {
    Serial.println("No Object Detected");
  }

  delay(200);
}