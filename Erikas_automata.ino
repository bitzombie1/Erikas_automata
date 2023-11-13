#define LIMIT_PIN A1
#define RAVEN_DIR_PIN 47
#define RAVIN_PWM_PIN 5
#define NECK_DIR_PIN 33
#define NECK_PWM_PIN 2
#define RELAY_PIN A2

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *claw = AFMS.getMotor(1);
Adafruit_DCMotor *jaw = AFMS.getMotor(1);

unsigned long now_t, prev_t, delta_t;      // our loop timing vars 

void setup() {
  Serial.begin(9600);
  
  pinMode(LIMIT_PIN, INPUT_PULLUP);
  pinMode(RAVEN_DIR_PIN, OUTPUT);
  pinMode(RAVIN_PWM_PIN, OUTPUT);
  pinMode(NECK_DIR_PIN, OUTPUT);
  pinMode(NECK_PWM_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  musicPlayer.setVolume(20,20);
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));

    
  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield ok");
  claw->setSpeed(100); // 0 -> 255 
  jaw->setSpeed(100);
  claw->run(FORWARD); // forward, backward, release
  claw->run(RELEASE);
  jaw->run(FORWARD);
  jaw->run(RELEASE);
}

void loop() {
  now_t = micros();
  if(now_t < prev_t){delta_t = now_t + (4294967295 - prev_t); Serial.println("rollover");} //millis rollover check
  else{delta_t = now_t - prev_t;}

  //musicPlayer.startPlayingFile("/track001.mp3")


  
  prev_t = now_t;
}

void motorFunc(){
  
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
