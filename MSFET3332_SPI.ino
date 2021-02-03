
#include <SPI.h>

const int cs = 53;


void setup() {
  Serial.begin(9600);
  pinMode(cs, OUTPUT);
  // start the SPI library:
  SPI.begin();
}

void loop() {
  //define variable
  double avg_pH = 0;  
  double Data_pH;
  for(int a=0;a<5;a++) {
        Data_pH = readADC();  //Serial.print("probe 3: "); Serial.println(Data_pH);
        avg_pH = avg_pH + Data_pH;
        //delay(10);
   }
    //double pH_mV = avg_pH/5*5000/4095;      // convert the data to mV                  
  double pH_mV = Data_pH/5*5000/4095;
  
  // // display data to serial monitor
  Serial.print("pH:  ");
  Serial.println(pH_mV);
  delay(1000); //Delay 1 second
}

//Read from ADC:
unsigned int readADC() 
{
  byte inByte = 0;           // incoming byte from the SPI
  unsigned int result = 0;   // result to return

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); 
  digitalWrite(cs, LOW); 
  
  // send a value of 0 to read the first byte returned:
  result = SPI.transfer(0x00); Serial.print("probe 1: "); Serial.println(result, BIN);
 
  // shift the first byte left, then get the second byte:
  result = result << 8;
  inByte = SPI.transfer(0x00); Serial.print("probe 2: "); Serial.println(inByte, BIN);
  
  // combine the byte you just got with the previous one:
  result = result | inByte;
  result = result >> 0;
  result = result & 0b0000111111111111;

  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  // return the result:
  return (result);
}
