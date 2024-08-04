

#define LIMIT_PIN A1      // ravin limit switch 
#define HOME_PIN A2       // ravin home switch 
#define RAVEN_DIR_PIN 32  // h-bridge limit and home switchs
#define RAVEN_PWM_PIN 5
#define NECK_DIR_PIN 23
#define NECK_PWM_PIN 2
#define RELAY_PIN A3        // Ohio motor for heart
//#define LIZARD_PWM_PIN 32
//#define LIZARD_DIR_PIN
#define AUDIO_1_PIN 39
#define AUDIO_2_PIN 41
#define AUDIO_3_PIN 43
#define AUDIO_4_PIN 45
#define AUDIO_5_PIN 47
#define AUDIO_6_PIN 49
#define AUDIO_7_PIN 51
#define AUDIO_8_PIN 53 

#define ANIMATE_T 1800000              // how often to animate . 1800000
#define CHANGEUP_T 5000               // how often to change up while animating 

int audio_pins[] = {39, 41, 43, 45, 47, 49, 51, 53};

#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *claw = AFMS.getMotor(1);
Adafruit_DCMotor *jaw = AFMS.getMotor(2);
Adafruit_DCMotor *lizard = AFMS.getMotor(3);

unsigned long now_t, prev_t, delta_t;      // our loop timing vars 
int neckPitch = 11;

unsigned long this_t, that_t, ravin_t;
int ravin_go = 0;                         //Is raven 0= not going, 1= going up, 2= going down
bool currRavinSw = false;                 // Is the switch currently on 

int state=0;

void setup() {
  Serial.begin(9600);
  
  pinMode(LIMIT_PIN, INPUT_PULLUP);
  pinMode(HOME_PIN, INPUT_PULLUP);
  pinMode(RAVEN_DIR_PIN, OUTPUT);
  pinMode(RAVEN_PWM_PIN, OUTPUT);
  pinMode(NECK_DIR_PIN, OUTPUT);
  pinMode(NECK_PWM_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  randomSeed(analogRead(7));
  
  pinMode(AUDIO_1_PIN, OUTPUT);
    pinMode(AUDIO_2_PIN, OUTPUT);
      pinMode(AUDIO_3_PIN, OUTPUT);
        pinMode(AUDIO_4_PIN, OUTPUT);
          pinMode(AUDIO_5_PIN, OUTPUT);
            pinMode(AUDIO_6_PIN, OUTPUT);
              pinMode(AUDIO_7_PIN, OUTPUT);
                pinMode(AUDIO_8_PIN, OUTPUT);

  
    
  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield ok");
  //analogWrite(NECK_PWM_PIN, 150);
 // digitalWrite(NECK_DIR_PIN, HIGH);
 // lizard->setSpeed(250);
 // claw->setSpeed(250); // 0 -> 255 
  jaw->setSpeed(100);
 // claw->run(BACKWARD); // forward, backward, release
  lizard->run(FORWARD);
 // digitalWrite(RELAY_PIN, HIGH);
  delay(4000);
  //digitalWrite(NECK_DIR_PIN, LOW);
 // motorNeck(11);
 //motorRaven(11);
  //claw->run(REVERSE); //RELEASE
  jaw->run(FORWARD);
  delay(2000);
  jaw->run(RELEASE);
  claw->run(RELEASE); //RELEASE
  digitalWrite(RELAY_PIN, LOW);
  lizard->run(RELEASE);
  analogWrite(NECK_PWM_PIN, 0);
 
  
 Serial.println(homeRavin());
 //motorRaven(11);
}

void loop() {
  now_t = millis();
  if(now_t < prev_t){delta_t = now_t + (4294967295 - prev_t); Serial.println("rollover");} //millis rollover check
  else{delta_t = now_t - prev_t;}
/*
 delay(20000);
  int randNum = random(1,9);
  Serial.println(randNum);
  digitalWrite(audio_pins[randNum -1], HIGH);
  delay(1);
  digitalWrite(audio_pins[randNum -1], LOW);
*/
  delay(1);
  if(state == 0){
    if(this_t > ANIMATE_T){
      limitRavin();
      playAudio();
      motorNeck(11);
      state = 1;
      this_t =0; 
    }
    else{this_t += delta_t;}
  }
  else if(state == 1){
    if(that_t > CHANGEUP_T){
      motorJaw(true);
      motorLizard(true);
      motorOhio(true);
      motorClaw(true);
      state = 2;
      that_t =0;
    }
    else{ that_t += delta_t;}
  }
  else if(state == 2){
    if(that_t > CHANGEUP_T){
      
      motorOhio(true);
      motorClaw(true);
      state = 3;
      that_t =0;
    }
    else{ that_t += delta_t;}
  }
  else if(state == 3){
    if(that_t > CHANGEUP_T){
      //motorLizard(false);
      //motorJaw(true);
      state = 4;
      that_t =0;
    }
    else{ that_t += delta_t;}
  }
  else if(state == 4){
     if(that_t > CHANGEUP_T){
      motorJaw(false);
      motorOhio(false);
      motorClaw(false);
      motorLizard(false);
      motorNeck(0);
      homeRavin();
      state = 0;
      that_t =0;
    }
    else{ that_t += delta_t;}
  }
/*
Serial.println(digitalRead(HOME_PIN));
  digitalWrite(NECK_DIR_PIN,HIGH);
  analogWrite(NECK_PWM_PIN, 160);
  digitalWrite(RELAY_PIN, HIGH);
  delay(5000);
  digitalWrite(NECK_DIR_PIN,LOW);
    digitalWrite(RELAY_PIN, LOW);

  delay(5000);
  */
  if(ravin_go > 0){
    
  }
 // if(digitalRead(LIMIT_PIN) == 0){Serial.println("got");}
 // if(ravenLimit()){Serial.println("limit!!");}
  prev_t = now_t;
}

void playAudio(){                               // play a random clip 
  int randNum = random(1,9);
  Serial.println(randNum);
  digitalWrite(audio_pins[randNum -1], HIGH);
  delay(1);
  digitalWrite(audio_pins[randNum -1], LOW);
}


bool ravenSwitch(){
  bool sw1 = !digitalRead(HOME_PIN);
  bool sw2 = !digitalRead(LIMIT_PIN);
  if(sw1 || sw2){return 1;}
  else{ return 0;}
}
bool ravenLimit(){
  bool sw2 = !digitalRead(LIMIT_PIN);
  return sw2;
}
bool ravenHome(){
  bool sw1 = !digitalRead(HOME_PIN);
  return sw1;
}

void motorRaven(int pitch){
  if(pitch == 11){
    digitalWrite(RAVEN_DIR_PIN, HIGH);
    analogWrite(RAVEN_PWM_PIN, 50);
    delay(500);
    analogWrite(RAVEN_PWM_PIN, 0);

  }
  else if(pitch == 0){
    
  }
}

int homeRavin(){                           // send the arm down to switch home
  int time_o = 0;
  digitalWrite(RAVEN_DIR_PIN, LOW);
  for(;time_o < 7000;time_o ++){
    currRavinSw = ravenHome();
    if(currRavinSw){                       // switch active bail 
      analogWrite(RAVEN_PWM_PIN, 0);
      Serial.println(time_o);
      return 1;
      }               
    else{ 
      analogWrite(RAVEN_PWM_PIN, 50);
      delay(1);
    }  
  }
  analogWrite(RAVEN_PWM_PIN, 0);
  return -1;                            // return switch fail! 
}

int limitRavin(){                           // send the arm down to switch limit
  int time_o = 0;
  digitalWrite(RAVEN_DIR_PIN, HIGH);
  for(;time_o < 11000;time_o ++){
    currRavinSw = ravenLimit();
    if(currRavinSw){                       // switch active bail 
      analogWrite(RAVEN_PWM_PIN, 0);
      Serial.println(time_o);
      return 1;
      }               
    else{ 
      analogWrite(RAVEN_PWM_PIN, 50);
      delay(1);
    }  
  }
  analogWrite(RAVEN_PWM_PIN, 0);
  return -1;                            // return switch fail! 
}


void motorJaw(bool goRun){
  if(goRun){
    jaw->setSpeed(200);
    jaw->run(FORWARD);
    
    //delay(2000);
    //jaw->run(RELEASE);
  }
  else if(!goRun){
    jaw->setSpeed(0);
    jaw->run(RELEASE);
  }
}

void motorNeck(int pitch){                // 1 to 10 pitch 1 = low , 10 = high , up.   if 0 go all the way down , 11 all the way up 
  
  if(pitch == 0){
    digitalWrite(NECK_DIR_PIN, LOW);
    analogWrite(NECK_PWM_PIN, 250);
   // delay(4000);
   // analogWrite(NECK_PWM_PIN, 0);

  }
  else if(pitch == 11){
    digitalWrite(NECK_DIR_PIN, HIGH);
    analogWrite(NECK_PWM_PIN, 250);
  //  delay(4000);
  //  analogWrite(NECK_PWM_PIN, 0);
  }
}

void motorLizard(bool goRun){
  if(goRun){
    lizard->setSpeed(250);
    lizard->run(FORWARD);
  }
  else if(!goRun){
    lizard->setSpeed(0);
    lizard->run(RELEASE);
  }
}

void motorOhio(bool goRun){
  if(goRun){
    digitalWrite(RELAY_PIN, HIGH);
  }
  else if(!goRun){
    digitalWrite(RELAY_PIN, LOW);
  }
}

void motorClaw(bool goRun){
  if(goRun){
    claw->setSpeed(250);
    claw->run(BACKWARD);                  // must run backwards
  }
  else if(!goRun){
    claw->setSpeed(0);
    claw->run(RELEASE);
  }
}
