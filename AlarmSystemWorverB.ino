#include <SPI.h>
#include <Wire.h>
#include <SSD1306.h> 
#include <Encoder.h>  
#include <RCSwitch.h>
#include <FS.h>
#include <SPIFFS.h>

#define DRAW_DELAY 118
#define D_NUM 47
#define ENC_A   32
#define ENC_B   33
#define FORMAT_SPIFFS_IF_FAILED true

int           sekunde, minute, stunde, tag, wota, monat, jahr;
unsigned long currentTime, lastTime, ReceivedValue;
unsigned long sID [100];    //;TYPE;ZONE;BIT;PRO;TIME
int           sTYPE [100], sZONE [100], sBIT [100], sPRO [100], sTIME [100];
unsigned int  progress, total;
boolean       rotary_switch = false;
int           rotary_event = 0, i;
long          positionENC = 0, newENC;
String        out, in;
char          puffer[30];

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
  BUZZ(20);

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  delay(100);
  if(!SPIFFS.exists("/sensoren.csv")){
    writeFile(SPIFFS, "/sensoren.csv", "ID;TYPE;ZONE;BIT;PRO;TIME\r\n");
    Serial.println("write");
  }

  read_to_sram(SPIFFS, "/sensoren.csv");

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

    write_new_sensor();


    mySwitch.resetAvailable();

    readFile(SPIFFS, "/sensoren.csv");
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

void read_to_sram(fs::FS &fs, const char * path){
    Serial.printf("read_to_sram: %s\r\n", path);
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed");
        return;
    }
    Serial.println("- reading:");
    
    i= file.readBytesUntil('\n', puffer, sizeof(puffer));
      puffer[i] = 0;
      in = puffer;
      Serial.println(in);
    int x = 0;
    while(file.available()){
      i= file.readBytesUntil(';', puffer, sizeof(puffer));
        puffer[i] = 0;
        sID[x] = atol(puffer);
        int y = 0;
        for (y; y < x; y++) {
            if (sID[x] == sID[y]) {
               y=998;
               sID[x] = 0;           
            }
        }
       
      i= file.readBytesUntil(';', puffer, sizeof(puffer));
        puffer[i] = 0;
        sTYPE[x] = atoi(puffer);
      i= file.readBytesUntil(';', puffer, sizeof(puffer));
        puffer[i] = 0;
        sZONE[x] = atoi(puffer);
      i= file.readBytesUntil(';', puffer, sizeof(puffer));
        puffer[i] = 0;
        sBIT[x] = atoi(puffer);
      i= file.readBytesUntil(';', puffer, sizeof(puffer));
        puffer[i] = 0;
        sPRO[x] = atoi(puffer);
      i= file.readBytesUntil('\n', puffer, sizeof(puffer));
        puffer[i] = 0;
        sTIME[x] = atoi(puffer);

      
      Serial.print(sID[x], DEC);
      Serial.print(";"); 
      Serial.print(sTYPE[x], DEC);
      Serial.print(";"); 
      Serial.print(sZONE[x], DEC);
      Serial.print(";"); 
      Serial.print(sBIT[x], DEC);
      Serial.print(";"); 
      Serial.print(sPRO[x], DEC);
      Serial.print(";"); 
      Serial.print(sTIME[x], DEC);
      Serial.print(";"); 
      Serial.print(x, DEC);
      Serial.print(";"); 
      Serial.println(y, DEC);

      if (y < 999){
        x++;
      }
    }







    
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

void appendFile(fs::FS &fs, const char * path, String text){
    Serial.printf("Appending to file: %s\r\n", path);
    const char * message = text.c_str();
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- frite failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void delete_write(){
  if(SPIFFS.exists("/sensoren.csv")){
    deleteFile(SPIFFS, "/sensoren.csv");
  }
  delay(100);
  if(!SPIFFS.exists("/sensoren.csv")){
    writeFile(SPIFFS, "/sensoren.csv", "ID;TYPE;ZONE;BIT;PRO;TIME\r\n");
  }
}

void write_new_sensor(){
    out = String(mySwitch.getReceivedValue()) + ";" + String(progress) + ";" + String(1) + ";" + String(mySwitch.getReceivedBitlength()) + ";" + String(mySwitch.getReceivedProtocol()) + ";" + String(mySwitch.getReceivedDelay()) + "\r\n";
    appendFile(SPIFFS, "/sensoren.csv",out); // ID;TYPE;ZONE;BIT;PRO;TIME\r\n
}
