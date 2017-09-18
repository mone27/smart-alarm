#include <SdFat.h>
#include <Audio.h>

SdFat sd;
SdFile file;
const uint8_t SD_CHIP_SELECT = SS; //should be removed for saving RAM
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(200);
  if (!sd.begin(SD_CHIP_SELECT, SPI_EIGHTH_SPEED)) sd.initErrorHalt();
  if (file.open("PRINT00.TXT", O_READ)) {
    Serial.println("Opened PRINT00.TXT");
  }
  int16_t n;
  uint8_t buf[7];// nothing special about 7, just a lucky number.
  while ((n = file.read(buf, sizeof(buf))) > 0) {
    for (uint8_t i = 0; i < n; i++) Serial.write(buf[i]);
  }
  if(!file.close()){
    Serial.println("error closing file");
  }
  Serial.println("dajee");
  

  
  Audio.begin(88200, 100);
}

void loop() {
  int count = 0;

  // open wave file from sdcard
   if (!file.open("test.wav",O_READ)) {
    // if the file didn't open, print an error and stop
    Serial.println("error opening test.wav");
    while (true);
  }

  const int S = 1024; // Number of samples to read in block
  short buffer[S];

  Serial.print("Playing");
  // until the file is not finished
  while (file.available()) {
    // read from the file into buffer
    file.read(buffer, sizeof(buffer));

    // Prepare samples
    int volume = 1024;
    Audio.prepare(buffer, S, volume);
    // Feed samples to audio
    Audio.write(buffer, S);

    // Every 100 block print a '.'
    count++;
    if (count == 100) {
      Serial.print(".");
      count = 0;
    }
  }
  file.close();

  Serial.println("End of file. Thank you for listening!");
  while (true) ;
}


