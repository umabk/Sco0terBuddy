#include <SPI.h>
#include <Adafruit_WINC1500.h>
#include <Twitter.h>
#include "pitches.h"

#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2

// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);

#define ADC_ref 3.3
#define zero_x 1.65
#define zero_y 1.65
#define zero_z 1.65
#define sensitivity_x 0.3
#define sensitivity_y 0.3
#define sensitivity_z 0.3

char ssid[] = "CalVisitor"; //  your network SSID (name)
char pass[] = "1888309wifi";

int status = WL_IDLE_STATUS;
Twitter twitter("709432830523125760-ogYOQKxI8vh3sQzZK9YkF2OdgJaLSLn");

int greenPin = 11;
int bluePin = 10;
int redPin = 5;

char msgUp[] = "Is there a scooter escalator for this hill? Cause there should be…";
char msgDown[] = "This scooter's got a need for speed!";
char msgBump[] = "I didn't know it was #pothole season so early on the roads this year...";
char msgIdleSad[] = "SAD SCOOTER ALERT RIGHT HERE";
char msgIdleAngry[] = "I swear, if people are using bikes instead of scooters, I will pop one of their dumb, goofy tires.";

String upSlope[8] = { "The incline is mine!",
"Can't wait to get to the top of this hill! I am too tired for this today :/",
"If I only could, I'd be scootering up that hill. If I only could, I'd be scootering up that hill.", 
"As I go up the hill I huff and I puff, but it is will be all worth it when I start speeding down that hill :)",
"No longer will the mountains around me limit my adventures, this next incline is mine.",
"I was always told that a scooter like me would never be able to go up that hill #Wrong #Hatersgonnahate", 
"In my nightmares, I go up hills like that…",
"Geez! It ain’t easy out here for a scooter with inclines like these."};

String downSlope[8] = {"The hills are alive...with my scooting!",
"Up the hills and down the hills, merrily I scoot along.",
"Today is the day that I conquer the hills of this town.",
"The world knows no greater force than a scooter sauntering up and down these rolling hills.",
"This scooter's got a need for speed!",
"This decline is mine!", 
"Ups and downs! Highs and lows! This scooter knows no bounds.",
"From the highest mountain to the lowest valley, my two wheels will traverse the world entire."};

String bumpy[8] = {"OW! THESE ROADS ARE TOO BUMPY!",
  "Oh boy! Rough roads around town today!!!",
"I didn't know it was #pothole season so early on the roads this year...",
"My favorite ice cream might be Rocky Road, but this street might be too much for this scooter.",
"My two wheels are really shaking after that bumpy road.",
"I sure am gonna be sore after riding over all these bumpy roads today.",
"Hopefully I don't get a flat going over all these bumpy roads.",
"OUCH OUCH OUCH. No more rocky roads like this after today!"};

String sad[8] = {"Get over with this! I want to move! :(",
"Is that a cobweb on my handlebar??? Probably since I haven’t left the house in days…",
"My wheels do not spin. My scooter body has scant touched the open road. I am not a scooter in this state.",
"The days are long and the night never comes soon enough when a scooter such as me sits idle…",
":( SAD SCOOTER ALERT RIGHT HERE", "I long for the day when I will return to the streets, when I will ride once again…",
"Do you know what makes a scooter sad? Just sitting around, that’s what.",
"Will I never ride again? Will I sit here forever lonely in my corner of this room? Does the sun still rise each morning?"
};

String angry[8] = {"I’d rather be going up a huge, steep hill than just sitting around like this! And I  H A T E steep hills.",
"I swear, if people are using bikes instead of scooters, I will pop one of their dumb, goofy tires.",
"Scooterbuddy, more like scooterANGRY! #Angry #Amiright",
"THIS SCOOTER IS ABOUT TO GO STRAIGHT UP #KANYE ON THIS TWITTER FEED IF HE HAS TO SIT IN THIS ROOM ONE MORE DAY",
"What do scooters hate more than anything in the world? Not being able to scoot around town, that’s what  >__<",
"Wow, really hope my owner is out of shape with all the scootering around town they’re NOT doing these days.",
"I may be a blue scooter, but if I had a spirit color, right now it would be red. Like, angry red.",
"My therapist told me I should calm down more when I get angry, but how can’t I be this mad when I haven’t moved in weeks?!?"
};

// notes in the melody:
int melodyHappy[] = {
  NOTE_C8, NOTE_G8, NOTE_E8, R, NOTE_C8, NOTE_G8, NOTE_C8, NOTE_G8
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsHappy[] = {
  8, 4, 4,  8,  8,  8, 4, 4
};

int melodySad[] = {
  NOTE_E8, NOTE_DS8, NOTE_C8, R, NOTE_AS7, NOTE_A7
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsSad[] = {
  2, 2, 1,  4, 2, 1
};

int value_x;
int value_y;
int value_z;
int old_x = 504;
int old_y = 497;
int old_z = 629;
unsigned int count_up = 0;
unsigned int count_down = 0;
unsigned int count_bump = 0;
unsigned int count_idle = 0;
unsigned int idleTrack = 0;
unsigned int tone_up = 0;
unsigned int tone_down = 0;

unsigned long lastTime = 0;
unsigned long lastTune = 0;

float xv;
float yv;
float zv;

float angle_x;
float angle_y;
float angle_z;

void setColor(int red, int green, int blue)
{
#ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
#endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
void loopyEyes()
{
  for (int i = 0; i < 10; i++) {
    setColor(255, 0, 0);  // red
    delay(200);
    setColor(0, 255, 0);  // green
    delay(200);
    setColor(0, 0, 255);  // blue
    delay(200);
    setColor(255, 255, 0);  // yellow
    delay(200);
    setColor(255, 0, 255);  // purple
    delay(200);
    setColor(0, 255, 255);  // aqua
    delay(200);
    setColor(0, 0, 0);
  }
}

void goEyes()
{
  for (int i = 0; i < 5; i++) {
    setColor(0, 0, 255);  // blue
    delay(500);
    setColor(0, 0, 0);  // blink
    delay(500);
  }
}

void idleEyes()
{
  for (int i = 0; i < 5; i++) {
    setColor(255, 0, 0);  // blue
    delay(500);
    setColor(0, 0, 0);  // blink
    delay(500);
  }
}

void tweetUphill(int tweetIndex) {
  Serial.println("connecting ...");
  if (twitter.post(upSlope[tweetIndex].c_str())) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}

void tweetDownhill(int tweetIndex) {
  Serial.println("connecting ...");
  if (twitter.post(downSlope[tweetIndex].c_str())) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}

void tweetBumpy(int tweetIndex) {
  Serial.println("connecting ...");
  if (twitter.post(bumpy[tweetIndex].c_str())) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}


void tweetIdleSad(int tweetIndex) {
  Serial.println("connecting ...");
  if (twitter.post(sad[tweetIndex].c_str())) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}

void tweetIdleAngry(int tweetIndex) {
  Serial.println("connecting ...");
  if (twitter.post(angry[tweetIndex].c_str())) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
}

void happyTune() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsHappy[thisNote];
    tone(9, melodyHappy[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(9);
  }
}

void sadTune() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsSad[thisNote];
    tone(9, melodySad[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(9);
  }
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  delay(1000);
#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif
  //  WiFi.begin(ssid, pass);
  // or you can use DHCP for automatic IP address configuration.
  // WiFi.begin(mac);
  Serial.begin(9600);
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    uint8_t timeout = 10;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }
  Serial.println("Connected to wifi");

}
int tweetIndex = 0;
int numTweets = 8;
void loop() {

  //Adafruit_WINC1500Client client = server.available();   // listen for incoming clients

  value_x = analogRead(A3);
  value_y = analogRead(A2);
  value_z = analogRead(A1);

  xv = (value_x / 1024.0 * ADC_ref - zero_x) / sensitivity_x;
  yv = (value_y / 1024.0 * ADC_ref - zero_y) / sensitivity_y;
  zv = (value_z / 1024.0 * ADC_ref - zero_z) / sensitivity_z;

  angle_x = atan2(-yv, -zv) * 57.2957795 + 180;

  angle_y = atan2(-xv, -zv) * 57.2957795 + 180;
  angle_z = atan2(-yv, -xv) * 57.2957795 + 180;

  if (angle_x > 270 && angle_x < 335) {
    count_up++;
    tone_up++;
  }
  if (angle_x > 5 && angle_x < 90) {
    count_down++;
    tone_down++;
  }


  if (abs(old_z - value_z) > 10)
    count_bump++;

  if (abs(old_x - value_x) < 5 && abs(old_y - value_y) < 5 && abs(old_z - value_z) < 5)
    count_idle++;

  if (tweetIndex == numTweets)
    tweetIndex = 0;

  if (tone_up > 8 && (millis() - lastTune) > 10000) {
    sadTune();
    goEyes();
    tone_up = 0;
    lastTune = millis();
  }
  if (tone_down > 8 && (millis() - lastTune) > 10000) {
    happyTune();
    goEyes();
    tone_down = 0;
    lastTune = millis();
  }

  if (count_up > 20 && (millis() - lastTime) > 60000)
  {
    tweetUphill(tweetIndex);
    count_up = 0;
    lastTime = millis();
    idleTrack = 0;
  }

  if (count_down > 20 && (millis() - lastTime) > 60000)
  {
    tweetDownhill(tweetIndex);
    count_down = 0;
    lastTime = millis();
    idleTrack = 0;
  }

  if (count_bump > 20 && (millis() - lastTime) > 5000)
  {
    loopyEyes();
    tweetBumpy(tweetIndex);
    count_bump = 0;
    lastTime = millis();
    idleTrack = 0;
  }

  if (count_idle > 20 && idleTrack == 0 && (millis() - lastTime) > 10000)
  {
    idleEyes();
    tweetIdleSad(tweetIndex);
    count_idle = 0;
    lastTime = millis();
    idleTrack = 1;
  }

  if (count_idle > 1000 && idleTrack == 1 && (millis() - lastTime) > 60000)
  {
    idleEyes();
    tweetIdleAngry(tweetIndex);
    count_idle = 0;
    lastTime = millis();
    idleTrack = 0;
  }

  old_x = value_x;
  old_y = value_y;
  old_z = value_z;

  delay(500);
  tweetIndex = tweetIndex + 1;
}

