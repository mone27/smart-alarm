//#include <UTFT_Buttons.h>
#include <Arduino.h>
#include<Audio.h>
#include <SD.h>
#include <SPI.h>
#include <DueTimer.h>
#include <URTouch.h>
#include <UTouchCD.h>
#include <string.h>
#include <UTFT.h>
#include <memorysaver.h>
#include <DS1307.h>
#define WD 240
#define HG 320

UTFT    disp(ILI9325D_8,38,39,40,41);
URTouch  touch(3,4,5,6,7);
//UTFT_Buttons butt(&disp, &touch);
DS1307 rtc(SDA, SCL);

extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
volatile bool firstTime_updTime = true;//for the fisrt time that update_time() is called by the interrupt
volatile bool play_alarm = true;

volatile Time t_al(16, 33);
void home();
void update_time();
bool playAlarm();
void check_alarm(Time t);


void setup() {
  Serial.begin(9600);
   // start rtc
  rtc.begin();
  //rtc run mode
  rtc.halt(false);
  //setting up display
  disp.InitLCD(0);
  touch.InitTouch(0);
  touch.setPrecision(PREC_MEDIUM);
  //setting up audio
//  Audio.begin(88200, 100);
  //inizializating sd card
  Serial.print("Initializing SD card...");
  if (!SD.begin(8)) {
    Serial.println(" failed!");
//    while(true);
  }
  Serial.println(" done.");




  home();




}
void home()
{

  disp.clrScr();
  disp.setFont(BigFont);
  disp.setColor(VGA_GREEN);
  disp.print(":",WD/2-3,HG/2-30);
  Timer3.attachInterrupt(update_time).setFrequency(1).start();
    while(true)//infinite loop
  {
    while( play_alarm)
    {
      playAlarm();
    }
    if(touch.dataAvailable()) play_alarm=true;
  }


}
/*void serialEvent(){
  Serial.print(Serial.read());
 Serial.println("insert the hour of the alarm");

uint8_t hour = Serial.parseInt();
Serial.print(hour, DEC);
t_al.hour=hour;
Serial.println("ok now insert the minutes for the alarm ");
uint8_t min = Serial.parseInt();
t_al.min= min;
Serial.print(min, DEC);
Serial.println("alarm set ");
}*/
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
    sec = t.sec;
    check_alarm(t);
      if(t.min == 00) //every hour
        {
          disp.printNumI(t.hour, WD/2-78, HG/2-50, 2, ' ');
        }
    }

  disp.setFont(BigFont);
  disp.printNumI( sec++, WD/2-16, HG/2+35, 2, '0');

}
void check_alarm(Time t)
{
  if(t_al.hour == t.hour && t_al.min == t.min)
  {
    play_alarm= true;
    Serial.print("alarm starting");
  }

}
bool playAlarm()
{
  //setting up audio
  Audio.begin(88200, 100);
  int count = 0;
  disp.setFont(BigFont);
  disp.print("playing..",0,0);
  // open wave file from sdcard
  File myFile = SD.open("test.wav");
  if (!myFile) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening test.wav");
    return false;
  }

  const int S = 1024; // Number of samples to read in block
  short buffer[S];

  Serial.print("Playing");
  // until the file is not finished
  while (myFile.available()) {
    // read from the file into buffer
    myFile.read(buffer, sizeof(buffer));

    // Prepare samples
    int volume = 1024;
    Audio.prepare(buffer, S, volume);
    // Feed samples to audio
    Audio.write(buffer, S);
    if(touch.dataAvailable())
    {
      myFile.close();//close the file
      touch.read(); // clear the touch buffer
      play_alarm= false; //set play alarm to false to stop playing the alarm in the main loop
      disp.print("         ",0,0);
      return true; //return
    }
   }
  myFile.close();
  return true;
}
void menu(){


}
void music_player(){

}
int keyboard(int start){

}
void loop() {
  // put your main code here, to run repeatedly:

}
//uint32_t next_time=0;
  //uint8_t sec=60, min=60;
  // del String h,m, s;
  /*while(true)
  {
    if ( millis() >= next_time)
    {
       next_time= millis() + 1000;
       //should write update time function!!!!!!!!!!!!!!
       t = rtc.getTime();
       if(min == 60){//every hour
        min=0;
        disp.setFont(SevenSegNumFont);
       // h = String(t.hour);
       disp.printNumI(t.hour, WD/2-78, HG/2-50, 2, '0');
       }

       // h = String(t.hour);
       //disp.printNumI(t.hour, WD/2-78, HG/2-50, 2, '0');//50 i s the size of a char with seven seg  font
                                                    //16 the size in big font
                                                    //to center a 5 char string
                                                    //seven seg 35x50
                                                    //

      if(sec==60){//every minute
        disp.setFont(SevenSegNumFont);
        disp.printNumI(t.min, WD/2+10, HG/2-50, 2, '0');
        min++;
        sec = 0;
        /*if(rtc.getTimeStr(FORMAT_SHORT) == "16:20"){//check for alarm
          for(uint8_t c=0; c <=6 ; c++){
            for(uint8_t i=0; i < 120 ; i++)
              {
                analogWrite(DAC0, waveformsTable[i]);
                delayMicroseconds(19);//for 440 hz

              }
              digitalWrite(DAC0, LOW);
          }
        }*/
        /*}
        disp.setFont(BigFont);
      disp.printNumI(t.sec, WD/2-16, HG/2+35, 2, '0');
      sec++;


      }//if next time
      */





    //}//while true
    /*if(int pressed = butt.checkButtons() != -1)
    {
      if (pressed == menu )
      {
        disp.print("sorry work in progress.....",LEFT,0);
      }
    }*/
