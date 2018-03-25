#include <Arduino.h>
#include "ArduinoJson.h"
#include "SdFat.h"
#include <Audio.h>
#include <DueTimer.h>
#include <URTouch.h>
#include <UTFT.h>
#include <memorysaver.h>
#include <DS3231.h>
#include <utils.h>
#include <DueTimer.h>
//#include "Logging.h"
#define WD 240
#define HG 320

//important remove unsued string literals!!!!!


// inizialize UTFT objects
UTFT    disp(ILI9325D_8,38,39,40,41);
URTouch  Touch(3,4,5,6,7);
DS3231 rtc(SDA, SCL);
SdFat sd;

const uint8_t SD_CHIP_SELECT = SS;

// external font

extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

volatile bool firstTime_updTime = true;//for the fisrt time that update_time() is called by the interrupt
volatile bool play_alarm = false;

volatile Time time_alarm;
void home();
void update_time();



 void setup(/* arguments */) {
    Serial.begin(9600);
    Serial << "starting .." << endl;
    if (!sd.begin(SD_CHIP_SELECT, SPI_EIGHTH_SPEED)) sd.initErrorHalt();
   time_alarm.min = 22;   time_alarm.hour = 06;
    // start rtc
    rtc.begin();
   
    //setting up display
    disp.InitLCD(0);
    Touch.InitTouch(0);
    Touch.setPrecision(PREC_MEDIUM);
    //setting up audio
     Audio.begin(88200, 100);
    

  disp.clrScr();
  disp.setFont(BigFont);
  disp.setColor(VGA_GREEN);
  disp.print(":",WD/2-3,HG/2-30);
  Timer3.attachInterrupt(update_time).setFrequency(1).start();

 }
 
void update_time()
{
  //called by timer interrupt every second
  static uint8_t sec;
  if(firstTime_updTime)
  {
    Time t;
    t=rtc.getTime();
    disp.setFont(SevenSegNumFont);
    disp.printNumI(t.hour, WD/2-78, HG/2-50, 2, ' ');
    disp.printNumI(t.min, WD/2+10, HG/2-50, 2, '0');
    disp.setFont(BigFont);
    disp.printNumI(t.sec, WD/2-16, HG/2+35, 2, '0');
    firstTime_updTime = false;
     sec = t.sec ;

  }
  if(sec == 60) //every   minute
  {
    static Time t;
    t = rtc.getTime(); //sync with rtc
    disp.setFont(SevenSegNumFont);
    disp.printNumI(t.min, WD/2+10, HG/2-50, 2, '0');
    Serial << "min :" << t.min << endl << "alarm min : "
    << time_alarm.min << "h : " << time_alarm.hour  << endl;
    
    sec = t.sec;
    //check for alarm
    if(t.min == time_alarm.min && t.hour == time_alarm.hour){
      play_alarm = true;
      Serial << "play alarm !!!!";
      
    }
    // update temperature
    Serial << "temp: " << rtc.getTemp() << endl;
    disp.setFont(BigFont);
    disp.printNumF(rtc.getTemp(),1 ,3 , 2 ,'.', 2);
    
    
      
      if(t.min == 00) //every hour
        {
          disp.setFont(SevenSegNumFont);
          disp.printNumI(t.hour, WD/2-78, HG/2-50, 2, ' ');
        }
    }

  disp.setFont(BigFont);
  disp.printNumI( sec++, WD/2-16, HG/2+35, 2, '0');

}
bool Alarm() {
  SdFile file;

  // open wave file from sdcard
   if (!file.open("test.wav",O_READ)) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening test.wav");
    while (true);
  }

  const int S = 1024; // Number of samples to read in block
  short buffer[S];

 
  // until the file is not finished
  while (file.available()) {
    // read from the file into buffer
    file.read(buffer, sizeof(buffer));

    // Prepare samples
    int volume = 1024;
    Audio.prepare(buffer, S, volume);
    // Feed samples to audio
    Audio.write(buffer, S);

    // check fro display data to shut down alarm
    if (Touch.dataAvailable()) {return true;}
  }
  file.close();
  return false;
}
void loop() {
  if ( play_alarm == true){
    play_alarm = false;
   bool alarm;
    do{
     alarm = Alarm();
    }while ( alarm == false );
    Serial << "stopped alarm";
 
  }
  if( Serial.available() > 0){
    SerialFlush();
    Serial << "setting up alarm... " << " insert hour of alarm and any non numerical charater at end" << endl;
    wait_for_input();
    time_alarm.hour = Serial.parseInt();
    SerialFlush();
    Serial << "inserted value : " << time_alarm.hour << endl; 

    
    Serial << " insert time of alarm and any non numerical charater at end" << endl;// copy and paste !!!!!! :(
    wait_for_input();
    time_alarm.min = Serial.parseInt();
    Serial << "inserted value : " << time_alarm.hour << endl;

    Serial << "alarm set up for : " << time_alarm.hour <<":" << time_alarm.min << endl;

      SerialFlush();
      
  }
  delay(1000);
}
void wait_for_input(){
  uint8_t count = 0;
  while(Serial.available() == 0){
    if(count == 30){
      Serial << '.';
      count = 0;
      }
      count ++;
      delay(100);
    }
   
}



//old code cut from setup

/*
    ifstream settings("set");
    if(!settings.is_open()) Log.Error("cant open sets file");

    String* JsonSet = new String("");
    int c;
    while ((c = settings.get()) >= 0) JsonSet += (char)c;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(JsonSet);
    */
    //char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    //
    // Step 1: Reserve memory space
    //
    //StaticJsonBuffer<200> jsonBuffer;

    //
    // Step 2: Deserialize the JSON string
    //
    //JsonObject& root = jsonBuffer.parseObject(json);






    //Log.Debug("inizializaton done!");

