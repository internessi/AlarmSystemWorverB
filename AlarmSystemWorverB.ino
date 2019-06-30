#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

unsigned long currentTime, lastTime;
boolean       encA, encB, lastAB = false, rotary_switch = false;
int           rotary_event_1 = 0;

void setup() {
  delay(500);
  Serial.begin(115200);
  


//
//  LED(1,1000);      // rot
//  LED(2,1000);      // grün
//  LED(3,1000);      // blau
//  LED(4,1000);      // orange/gelb
//  LED(5,1000);      // cyan
//  LED(6,1000);      // magenta
//  LED(7,1000);      // weiss
//  BUZZ(250);
//
  pinMode       (25, OUTPUT);       // ROT
  pinMode       (26, OUTPUT);       // GREEN
  pinMode       (27, OUTPUT);       // BLAU
  pinMode       (4, OUTPUT);        // BUZZER
  LED_OFF();
  BUZZ_OFF();
  
  pinMode       (32, INPUT_PULLUP); // rotary pin as input with internal pullups
  pinMode       (33, INPUT_PULLUP); // rotary pin as input with internal pullups
  pinMode       (2, INPUT_PULLUP);         // rotary switch pin 
  rotary_switch = false;


  Serial.println("Starting up");
  BUZZ(250);

  attachInterrupt(digitalPinToInterrupt(2), INT_RotarySwitch, CHANGE);
}

void loop() {

  // Wenn Rotary Switch
  if (rotary_switch){
    Serial.println("Switch betaetigt"); //... das Signal ausgegeben wurde, wird das Programm fortgeführt.
    BUZZ(250);
    rotary_switch = false;
  }


  // Wenn Rotary plus - minus
  if (!rotary_event_1 == 0){
      Serial.println(rotary_event_1);
         BUZZ(30);
      rotary_event_1 = 0;
  }


  ROTARY();
}



void INT_RotarySwitch() {rotary_switch = true;}

void ROTARY() {
  currentTime = millis();
  if(currentTime >= (lastTime + 10))
  {
    encA = digitalRead(32);
    encB = digitalRead(33);
    if ((!encA) && (lastAB)){if (encB){rotary_event_1 = rotary_event_1 + 1;}else{rotary_event_1 = rotary_event_1 - 1;}}
    lastAB = encA;
    lastTime = currentTime;
  }
}

int LED(int LED, double zeit){
  if (LED == 1){
    digitalWrite(25, LOW);  
    }
  if (LED == 2){
    digitalWrite(26, LOW);    
    }
  if (LED == 3){
    digitalWrite(27, LOW);  
    }
  if (LED == 4){
    digitalWrite(25, LOW);   
    digitalWrite(26, LOW);
    }   
  if (LED == 5){
    digitalWrite(27, LOW);   
    digitalWrite(26, LOW);
    }   
  if (LED == 6){
    digitalWrite(25, LOW);   
    digitalWrite(27, LOW);
    }       
  if (LED == 7){
    digitalWrite(25, LOW);   
    digitalWrite(26, LOW);
    digitalWrite(27, LOW);
    }        
  delay(zeit+10); 
  digitalWrite(25, HIGH);   
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
}


void LED_OFF() {
  digitalWrite(25, HIGH);   
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
}

int BUZZ(double zeit){
  digitalWrite  (4, HIGH); 
  delay(zeit); 
  BUZZ_OFF();
}
void BUZZ_OFF() {
  digitalWrite  (4, LOW);
}
