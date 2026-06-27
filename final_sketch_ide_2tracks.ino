#include <BluetoothSerial.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

BluetoothSerial SerialBT;

HardwareSerial dfSerial(1);
DFRobotDFPlayerMini player;

// STRAIGHT TRACK
#define STRAIGHT_START 25
#define STRAIGHT_END   26

// CURVED TRACK
#define CURVED_START   14
#define CURVED_END     27

unsigned long straightStartTime = 0;
unsigned long curvedStartTime = 0;

bool straightRunning = false;
bool curvedRunning = false;

// ================= AUDIO =================

void playDigit(char d)
{
  switch (d)
  {
    case '0': player.play(10); break;
    case '1': player.play(16); break;
    case '2': player.play(9);  break;
    case '3': player.play(7);  break;
    case '4': player.play(14); break;
    case '5': player.play(13); break;
    case '6': player.play(4);  break;
    case '7': player.play(3);  break;
    case '8': player.play(12); break;
    case '9': player.play(15); break;
  }

  delay(700);
}

void speakNumber(float value)
{
  char buffer[10];

  dtostrf(value, 0, 2, buffer);

  for (int i = 0; buffer[i] != '\0'; i++)
  {
    if (buffer[i] == '.')
    {
      player.play(1);     // point
      delay(700);
    }
    else
    {
      playDigit(buffer[i]);
    }
  }

  player.play(2);         // seconds
  delay(1000);
}

void announceStraight(float t)
{
  player.play(5);         // straight
  delay(1500);

  player.play(8);         // time
  delay(1200);

  speakNumber(t);
}

void announceCurved(float t)
{
  player.play(11);        // curved
  delay(1500);

  player.play(8);         // time
  delay(1200);

  speakNumber(t);
}

// ================= SETUP =================

void setup()
{
  Serial.begin(115200);

  SerialBT.begin("ESP32_Robot");

  pinMode(STRAIGHT_START, INPUT);
  pinMode(STRAIGHT_END, INPUT);

  pinMode(CURVED_START, INPUT);
  pinMode(CURVED_END, INPUT);

  // ESP32 RX=19  TX=18
  dfSerial.begin(9600, SERIAL_8N1, 19, 18);

  Serial.println("Starting DFPlayer...");

  if (!player.begin(dfSerial))
  {
    Serial.println("DFPlayer NOT FOUND");
    SerialBT.println("DFPlayer NOT FOUND");

    while (true);
  }

  player.volume(30);

  Serial.println("System Ready");
  SerialBT.println("System Ready");

  player.play(6);   // systemready

  delay(2000);
}

// ================= LOOP =================

void loop()
{
  // ---------- STRAIGHT START ----------

  if (!straightRunning &&
      digitalRead(STRAIGHT_START) == LOW)
  {
    straightStartTime = millis();
    straightRunning = true;

    Serial.println("Straight Started");
    SerialBT.println("Straight Started");

    delay(300);
  }

  // ---------- STRAIGHT END ----------

  if (straightRunning &&
      digitalRead(STRAIGHT_END) == LOW)
  {
    float straightTime =
      (millis() - straightStartTime) / 1000.0;

    Serial.print("Straight Time = ");
    Serial.println(straightTime, 2);

    SerialBT.print("STRAIGHT TIME = ");
    SerialBT.println(straightTime, 2);

    announceStraight(straightTime);

    straightRunning = false;

    delay(1000);
  }

  // ---------- CURVED START ----------

  if (!curvedRunning &&
      digitalRead(CURVED_START) == LOW)
  {
    curvedStartTime = millis();
    curvedRunning = true;

    Serial.println("Curved Started");
    SerialBT.println("Curved Started");

    delay(300);
  }

  // ---------- CURVED END ----------

  if (curvedRunning &&
      digitalRead(CURVED_END) == LOW)
  {
    float curvedTime =
      (millis() - curvedStartTime) / 1000.0;

    Serial.print("Curved Time = ");
    Serial.println(curvedTime, 2);

    SerialBT.print("CURVED TIME = ");
    SerialBT.println(curvedTime, 2);

    announceCurved(curvedTime);

    curvedRunning = false;

    delay(1000);
  }
}