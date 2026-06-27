#include <BluetoothSerial.h>
/*
  BluetoothSerial.h Library

  Purpose:
  - Enables Bluetooth Classic (Serial Port Profile - SPP) communication
    on the ESP32.
  - Allows wireless serial communication between the ESP32 and
    smartphones, laptops, or other Bluetooth-enabled devices.

  Features:
  - Send and receive data wirelessly.
  - Works like the Serial Monitor but over Bluetooth.
  - Supports text, numbers, and commands.

  Common Functions:
  - begin("Device_Name") : Starts Bluetooth with a device name.
  - available()          : Checks if data is available.
  - read()               : Reads incoming data.
  - write()              : Sends raw data.
  - print()/println()    : Sends text or values.

  Note:
  - BluetoothSerial.h is supported only on ESP32 boards.
  - Used for Bluetooth Classic (SPP), not Bluetooth Low Energy (BLE).
*/
#include <HardwareSerial.h>
/*
  HardwareSerial.h Library

  Purpose:
  - Provides access to the ESP32's hardware UART (serial) ports.
  - Used for serial communication with external devices such as
    GPS modules, GSM modules, Bluetooth modules, barcode scanners,
    and other microcontrollers.

  ESP32 UARTs:
  - Serial  -> UART0 (USB programming and Serial Monitor)
  - Serial1 -> UART1
  - Serial2 -> UART2

  Common Functions:
  - begin()      : Starts serial communication.
  - available()  : Checks if data has been received.
  - read()       : Reads one byte of incoming data.
  - write()      : Sends raw data.
  - print() / println() : Sends text or values.

  Note:
  - Not required for digital sensors like IR obstacle sensors.
  - Use it only when communicating with UART-based devices.
*/
#include <DFRobotDFPlayerMini.h>
/*
  DFRobotDFPlayerMini.h Library

  Purpose:
  - Controls the DFPlayer Mini MP3 module using UART serial communication.
  - Allows the ESP32 to play audio files stored on a microSD card.

  Features:
  - Play, pause, stop, next, and previous tracks.
  - Set volume and equalizer (EQ).
  - Play specific tracks or folders.
  - Check the player's status and receive error messages.

  Common Functions:
  - begin()           : Initializes the DFPlayer Mini.
  - play(trackNo)     : Plays the specified track.
  - pause()           : Pauses playback.
  - stop()            : Stops playback.
  - next()            : Plays the next track.
  - previous()        : Plays the previous track.
  - volume(level)     : Sets the volume (0–30).

  Note:
  - Requires UART communication (HardwareSerial.h).
  - Audio files must be stored on a properly formatted microSD card.
  - Commonly used in voice assistants, talking robots, alarms, and music playback projects.
*/

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
