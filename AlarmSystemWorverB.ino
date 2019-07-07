#include <SPI.h>
#include <Wire.h>
#include <SSD1306.h> 
#include <Encoder.h>  
#include <RCSwitch.h>

 
#define DRAW_DELAY 118
#define D_NUM 47
#define ENC_A   32
#define ENC_B   33

int sekunde, minute, stunde, tag, wota, monat, jahr;

unsigned long currentTime, lastTime, ReceivedValue;
unsigned int progress, total;
boolean       rotary_switch = false;
int           rotary_event = 0, i;
long          positionENC = 0, newENC;
String        out;

SSD1306  display(0x3c, 21, 22);
Encoder  ENC(ENC_B, ENC_A);
RCSwitch mySwitch = RCSwitch();

void setup() {
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
  

  pinMode       (2, INPUT_PULLUP);         // rotary switch pin 
  rotary_switch = false;

  progress = 0;
  total = 100;

  delay(500);
  Serial.println("Starting up");
  BUZZ(250);


  

display.init();
display.setContrast(255);
display_top();
display.drawProgressBar(4, 29, 120, 8, 100 / (total / 100) );
display.display();


  
  // Interrupt initialisieren für RotarySwitch
  attachInterrupt(digitalPinToInterrupt(2), INT_RotarySwitch, CHANGE);
  // Interrupt initialisieren für RCSwitch
  mySwitch.enableReceive(digitalPinToInterrupt(15));
}

void loop() {

  if (mySwitch.available()) {
    
    display_top();
    display.drawProgressBar(4, 29, 120, 8, 100 / (total / 100) );

    ReceivedValue = mySwitch.getReceivedValue();
    out = String(ReceivedValue);
 
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(display.getWidth()/2, 38, out);
    
    display.display();
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedDelay() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );   
    
    mySwitch.resetAvailable();
  }


  // Wenn Rotary Switch
  if (rotary_switch){
    Serial.println(display.getWidth()/2); //... das Signal ausgegeben wurde, wird das Programm fortgeführt.
  
    progress = progress + 10;
    display_top();
    display.drawProgressBar(4, 29, 120, 8, progress / (total / 100) );
    display.display();
    
    BUZZ(2);
    rotary_switch = false;
    }
  // Wenn Rotary plus - minus
  if (!rotary_event == 0){
    progress = progress + rotary_event;
    display_top();
    display.drawProgressBar(4, 29, 120, 8, progress / (total / 100) );
    display.display();
    Serial.println(progress);
       BUZZ(2);
    rotary_event = 0;
    }
  // Rotary prüfen
  read_encoder();
}

// START  display_top()
void display_top() {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth()/2, 1, "alarm4u.de");
} // END  display_top()

// START  read_encoder()
void read_encoder() {
  newENC= ENC.read();
  if  (newENC != positionENC) {positionENC= newENC;}
  if  (positionENC > 3)   {
      ENC.write(0);
      positionENC = 0;
      rotary_event = 1;   }
  if (positionENC < -3)   {
      ENC.write(0);
      positionENC = 0;
      rotary_event = -1;  }  
} // END  read_encoder()

// START  INT_RotarySwitch()
void INT_RotarySwitch() {rotary_switch = true;}
// END    INT_RotarySwitch()

// START  LED(int LED, double zeit)
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
}// END   LED(int LED, double zeit) 


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
