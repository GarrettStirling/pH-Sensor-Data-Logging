/* For Boron, 1.1.0  
SD Card--> CS (Green): A5, CLK (White): D13, MISO (Blue): D12, MOSI (Yellow): D11
Sentron sesnsor--> TX (Blue - from sensor): D10, RX (Y`ellow - from sensor): D9 */

#include "SdFat.h"
#include "SdCardLogHandlerRK.h"
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

String phcommand = "999!\r";  //Send to retreive pH data
byte phresponse[] = {0,0,0,0,0,0,0,0,0,0,0};  //array to store the response 
// String tempcommand = "777!\r"; 
// byte tempresponse[] = {0,0,0,0,0,0,0};  

const int cs = A5; //Chip select is in Pin A5
SdFat sd;
SdCardLogHandler<2048> logHandler(sd, cs, SPI_FULL_SPEED); //can use 'SD_SCK_HZ(4*MHZ)' if SPI_FULL_SPEED doesn't work
STARTUP(logHandler.withDesiredFileSize(1000000).withMaxFilesToKeep(10000)); // 1000000bytes * 10000files = 10 GB  before files are deleted and overwritten

void setup() {
  Cellular.off();
  Serial.begin(9600);    //For Serial.print
  Serial1.begin(115200); //For Sentron
  logHandler.setup();
}

void loop() {
  Serial.print("time: "); Serial.print(Time.format("%H:%M:%S")); Serial.println(", ");
  
  // To retrieve pH Data
  Serial1.println(phcommand);               // 1. Send initial command
  getphData();                              // 2. Receive Data from sesnsor
  double valpH = calcpH(phresponse);        // 3. Do Calculation & Display pH
	Serial.print("pH: "); Serial.println(valpH);

  // // To retrieve Temp Data
  //   Serial1.println(tempcommand);            // 1. Send initial command
  // Serial1.flush();
  // gettempData();                           // 2. Receive Data from sesnsor
  // double valtemp = calctemp(tempresponse); // 3. Do Calculation & Display pH
  // Serial.print("Temp (F): "); Serial.println(valtemp);
 
  // Log to SD Card
  logHandler.loop();
  double ms = millis() / (float)1000;
  Log.info("%f, %.3f", ms, valpH); // Log.info("%s, %d, %.3f, %.3f", Time.timeStr().c_str(), time_now, valtemp, valpH);
  Serial.println(" ");
  delay(1000);
}

//[[[[[[[[[[[[[[[[[[[  FOR PH DATA  ]]]]]]]]]]]]]]]]]]]
void getphData () {
  while(Serial1.available())  {
    for (int i=0; i < 11; i++) {    // Indexes 11 incoming Bytes
    phresponse[i] = Serial1.read();
    }
  }
}
double calcpH(unsigned char phpacket[]) {
  double A = phpacket[0]; // first byte
  double B = phpacket[1]; // second byte 
  double C = phpacket[2]; // third byte
  double pH = (A*4096 + B*64 + C) / (float)1000; //Formula from Datasheet
  Serial.print("pH Byte A: "); Serial.print(A); Serial.print(", ");
  Serial.print("Byte B: "); Serial.print(B); Serial.print(", ");
  Serial.print("Byte C: "); Serial.print(C); Serial.println(", ");
  return pH;
} 

// //[[[[[[[[[[[[[[[[[[[  FOR TEMP DATA  ]]]]]]]]]]]]]]]]]]]
// void gettempData () {
//   while(Serial1.available())  {
//     for (int i=0; i < 7; i++) {    // Indexes 7 incoming Bytes
//       tempresponse[i] = Serial1.read();
//     }
//   }
// }
// double calctemp(unsigned char temppacket[]) {
//   double A = temppacket[0]; // first byte
//   double B = temppacket[1]; // second byte 
//   double temp = (A*64 + B) / (float)10; //Formula from Datasheet
//   Serial.print("Temp Byte A: "); Serial.print(A); Serial.print(", ");
//   Serial.print("Byte B: "); Serial.println(B);
//   return temp;
// } 













/* OLD 'getdata' CODE:

void getData () {
  while(Serial1.available())  {
    int timeout = 0;                   //set a timeout counter
    while(Serial1.available() < 11 ) { // Wait for 11 byte response
      timeout++;
    if(timeout > 10){                  //if it takes too long there was probably an error
      while(Serial1.available()) Serial1.read();
      Serial.println("about to break");
      break;                           //exit and try again
    }
    delay(500);
    }
    for (int i=0; i < 11; i++) {    // Indexes 11 incoming Bytes
    response[i] = Serial1.read();
    }
  }
}

*/