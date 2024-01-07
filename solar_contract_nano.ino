float temp,hum;
float light;
int State;
int percentage = 80;
int lab= 0; 
int office = 1;
String connectState = "NOT CONNECTED";
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

void read_datas();


#define TFT_CS     10
#define TFT_RST    9  
#define TFT_DC     8

#define BLACK   0x0000
#define RED     0x001F
#define BLUE    0xF800
#define GREEN   0x07E0
#define YELLOW  0x07FF
#define PURPLE  0xF81F
#define CYAN    0xFFE0  
#define WHITE   0xFFFF

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

int count;
double amp1;
#define SAMPLES 500   //Number of samples you want to take everytime you loop
#define ACS_Pin1 A0    //ACS712 data pin analong input

double amp2;
#define SAMPLES 500   //Number of samples you want to take everytime you loop
#define ACS_Pin2 A1    //ACS712 data pin analong input

float increasingTime1;
float increasingTime2;


float energy1;  float energyA; 
float High_peak1,Low_peak1;         //Variables to measure or calculate
float Amps_Peak_Peak1, Amps_RMS1;

float energy2;  float energyB; 
float High_peak2,Low_peak2;         //Variables to measure or calculate
float Amps_Peak_Peak2, Amps_RMS2;


void setup() {
  delay(5000);
  Serial.begin(9600); delay(100);
  Serial.println("Started...");
  pinMode(ACS_Pin1,INPUT);  
  pinMode(ACS_Pin2,INPUT); 
  delay(5000); 
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  welcomeMessage();
  delay(100);
  draw_buttons();
  delay(1000);
  print_labels();
}

void loop() {
    read_Amps1();
    read_Amps2();
    if (amp1 <= 0.17 && amp1 != 0) amp1 =0.0001; 
    if (amp2 <= 0.30 && amp2 != 0) amp2 =0.0001;                                         
    afterMathFunction1();
    afterMathFunction2();
//    Serial.println((String)amp1+"       "+amp2+"      ");                

    read_datas();
    
    statusCondition();
    count++;
    if (count > 10){
      count = 0;
      Serial.println((String)"Energys,"+energy1+","+energy2);
      delay(1);
      print_labels();
    }
    delay(500);
}

void statusCondition(){
  if (State == HIGH) {
      connectState = "CONNECTED";
    } else if (State == LOW) {
      connectState = "NOT CONNECTED";
    }
//    Serial.println(connectState);
}


void read_Amps1()            //read_Amps function calculate the difference between the high peak and low peak 
{                           //get peak to peak value
  int cnt1;            //Counter
  High_peak1 = 0;      //We first assume that our high peak is equal to 0 and low peak is 1024, yes inverted
  Low_peak1 = 1024;
  
      for(cnt1=0 ; cnt1<SAMPLES ; cnt1++)          //everytime a sample (module value) is taken it will go through test
      {
        float ACS_Value1 = analogRead(ACS_Pin1); //We read a single value from the module

        if(ACS_Value1 > High_peak1)                //If that value is higher than the high peak (at first is 0)
            {
              High_peak1 = ACS_Value1;            //The high peak will change from 0 to that value found
            }
        
        if(ACS_Value1 < Low_peak1)                //If that value is lower than the low peak (at first is 1024)
            {
              Low_peak1 = ACS_Value1;             //The low peak will change from 1024 to that value found
            }
      }                                        //We keep looping until we take all samples and at the end we will have the high/low peaks values
      
  Amps_Peak_Peak1 = High_peak1 - Low_peak1;      //Calculate the difference
   Amps_RMS1 = Amps_Peak_Peak1 *0.3536*0.06;   
  amp1 = Amps_RMS1 - 0.08;
}

void read_Amps2()            //read_Amps function calculate the difference between the high peak and low peak 
{                           //get peak to peak value
  int cnt2;            //Counter
  High_peak2 = 0;      //We first assume that our high peak is equal to 0 and low peak is 1024, yes inverted
  Low_peak2 = 1024;
  
      for(cnt2=0 ; cnt2<SAMPLES ; cnt2 ++)          //everytime a sample (module value) is taken it will go through test
      {
        float ACS_Value2 = analogRead(ACS_Pin2); //We read a single value from the module

        if(ACS_Value2 > High_peak2)                //If that value is higher than the high peak (at first is 0)
            {
              High_peak2 = ACS_Value2;            //The high peak will change from 0 to that value found
            }
        
        if(ACS_Value2 < Low_peak2)                //If that value is lower than the low peak (at first is 1024)
            {
              Low_peak2 = ACS_Value2;             //The low peak will change from 1024 to that value found
            }
      }                                        //We keep looping until we take all samples and at the end we will have the high/low peaks values
      
  Amps_Peak_Peak2 = High_peak2 - Low_peak2;      //Calculate the difference
   Amps_RMS2 = Amps_Peak_Peak2 *0.3536*0.06;   
  amp2 = Amps_RMS2 - 0.08;
}

void afterMathFunction1(){
 increasingTime1 = millis()/1000;
// Serial.print("        ");
// Serial.print(increasingTime);

 energyA = (amp1 * (increasingTime1 /3600));
 // energyJoule = energyA * voltage;
  energy1 = energy1 + energyA;

}


void afterMathFunction2(){
 increasingTime2 = millis()/1000;
// Serial.print("        ");
// Serial.print(increasingTime);

 energyB = (amp2 * (increasingTime2 /3600));
 // energyJoule = energyA * voltage;
  energy2 = energy2 + energyB;

}


void welcomeMessage() {
  tft.setTextSize(1);
  delay(1000); 
  tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(WHITE);
  tft.setCursor(5, 10);
  tft.println((String)"  WELCOME  ");
  delay(2000); 
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(5, 10);
    tft.setTextColor(YELLOW);
  tft.println((String)"WEB BASED INTELLIGENT");
  delay(2000);
    tft.setTextColor(YELLOW);
  tft.setCursor(5, 20);
  tft.println((String)"    SOLAR SYSTEM   ");
  delay(2000);
    tft.setTextColor(YELLOW);
  tft.setCursor(5, 40);
  tft.println((String)"DONE BY:   ");
  delay(2000); 
  tft.setCursor(5, 60);
  tft.println((String)"     G13,        ");
  delay(2000);
  tft.setCursor(5, 70);
  tft.println((String)"   2023/24        ");
  delay(2000);
  tft.setCursor(5, 90);
  tft.println((String)"SUPERVISED BY:  ");
  delay(2000);
  tft.setCursor(5, 110);
  tft.println((String)" ENGR. DR. ");
  delay(4000); 
  tft.fillScreen(ST7735_BLACK);
  
}

void print_labels(){
  tft.fillScreen(ST7735_BLACK);
  draw_buttons();
  if (percentage < 20){
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor(90,15);
    tft.println((String)percentage+"%");
  }
  else if (percentage > 70){
    tft.setTextSize(2);
    tft.setTextColor(GREEN);
    tft.setCursor(90,15);
    tft.println((String)percentage+"%");
  }
  else {
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.setCursor(90,15);
    tft.println((String)percentage+"%");
  }
  

  tft.setTextSize(1);
  //System label
  tft.setTextColor(BLUE);
  tft.setCursor(12,13);
  tft.println("INTELLIGENT");
  tft.setCursor(11,26);
  tft.println("SOLAR SYSTEM");

  if (connectState == "NOT CONNECTED"){
    tft.setTextColor(RED);
    tft.setCursor(13,43);
    tft.println((String)"   "+connectState);
  }
  else if (connectState == "CONNECTED"){
    tft.setTextColor(GREEN);
    tft.setCursor(10,43);
    tft.println((String)"    "+connectState);
  }
  tft.setTextSize(1);
  //Warning label
  tft.setTextColor(BLUE);
  tft.setCursor(10,60);
  tft.println((String)"TEMPERATURE: ");
  tft.setTextColor(YELLOW);
  tft.setCursor(82,60);
  tft.println((String)temp+"`C");

  tft.setTextColor(BLUE);
  tft.setCursor(10,70);
  tft.println((String)"HUMIDITY: ");
  tft.setTextColor(YELLOW);
  tft.setCursor(82,70);
  tft.println((String)hum+"%");
  
  tft.setTextColor(BLUE);
  tft.setCursor(10,80);
  tft.println((String)"LIGHT INTENSITY: ");
  tft.setTextColor(YELLOW);
  tft.setCursor(82,80);
  tft.println((String)light+"%");

  tft.setTextColor(BLUE);
  tft.setCursor(10,90);
  tft.println((String)"LAB_ENERGY: ");
  tft.setTextColor(YELLOW);
  tft.setCursor(82,90);
  tft.println((String)energy1+"WH");

  tft.setTextColor(BLUE);
  tft.setCursor(10,100);
  tft.println((String)"OFFICE_EY: ");
  tft.setTextColor(YELLOW);
  tft.setCursor(82,100);
  tft.println((String)energy2+"WH");

  tft.setTextColor(BLUE);
  tft.setCursor(10,110);
  tft.println((String)" OFFICE LABORATORY");
  if (office == HIGH){
    tft.setTextColor(GREEN);
    tft.setCursor(30,120);
    tft.println("ON");
  }
  else if (office == LOW){
    tft.setTextColor(RED);
    tft.setCursor(28,120);
    tft.println("OFF");
  }

  if (lab == HIGH){
    tft.setTextColor(GREEN);
    tft.setCursor(83,120);
    tft.println("ON");
  }
  else if (lab == LOW){
    tft.setTextColor(RED);
    tft.setCursor(80,120);
    tft.println("OFF");
  }
}

void draw_buttons(){

//  //Red button left of System Test label
//  tft.drawLine(11, 14, 11, 18, BLUE);
//  tft.drawLine(11, 14, 15, 14, BLUE);
//  tft.drawLine(15, 14, 15, 14, BLUE);
//  tft.drawLine(15, 14, 15, 18, BLUE);

  //Red and yellow buttons right of System Test label
//  tft.fillRect(99, 5, 13, 6, BLUE);
//  tft.fillRect(99, 33, 13, 6, YELLOW); 

  //Rectangle around left red button and system test label
  tft.drawLine(8,5,8,38, WHITE);  //|(POSOFHEIGHTXAXIS---,STARTHEIGHT|,ENDOFHEIGHT---,ENDHEIGHT|)
  tft.drawLine(87,5,87,38, WHITE); //|(POSOFHEIGHTXAXIS---,STARTHEIGHT,HOWLONG, ENDHEIGHT)
  tft.drawLine(8,5,87,5, WHITE);//-
  tft.drawLine(8,38,87,38, WHITE);//-

  //Rectangle around counter
  tft.drawLine(8,38,8,128, WHITE);//|LEFT
  tft.drawLine(8,129,124,129, WHITE);//-DOWN
  tft.drawLine(124,6,124,128, WHITE);//|RIGHT
  tft.drawLine(8,55,124,55, WHITE);//-UP
}

void read_datas() {
  static String accumulatedData;

  while (Serial.available() > 0) {
    char c = Serial.read();
//    Serial.print(c);  // Uncomment this line for debugging

    accumulatedData += c;

    // Check for the start and end markers
    if (accumulatedData.endsWith("START") && c == 'D') {
      // Process the start of a new data packet
      accumulatedData = "START_Datas,";
    } else if (accumulatedData.endsWith("_END")) {
      // Process the complete data packet
      accumulatedData.trim();
      accumulatedData.toLowerCase();
      // Uncomment this line for debugging
      // Serial.println(accumulatedData);

      // Parsing logic
      if (accumulatedData.indexOf("datas") > -1) {
        int pos[7];
        pos[0] = accumulatedData.indexOf(',');
        for (int i = 1; i < 7; ++i) {
          pos[i] = accumulatedData.indexOf(',', pos[i - 1] + 1);
        }

        temp = accumulatedData.substring(pos[0] + 1, pos[1]).toFloat();
        hum = accumulatedData.substring(pos[1] + 1, pos[2]).toFloat();
        float light0 = accumulatedData.substring(pos[2] + 1, pos[3]).toFloat();
        float percent = accumulatedData.substring(pos[3] + 1, pos[4]).toFloat();
        float office0 = accumulatedData.substring(pos[4] + 1, pos[5]).toFloat();
        float lab0 = accumulatedData.substring(pos[5] + 1, pos[6]).toFloat();
        float State0 = accumulatedData.substring(pos[6] + 1).toFloat();

        light = light0;
        percentage = round(percent);
        office = round(office0);
        lab = round(lab0);
        State = round(State0);
        
        Serial.println();
        Serial.print("Temp: ");
        Serial.print(temp);
        Serial.print(" Hum: ");
        Serial.print(hum);
        Serial.print(" Light: ");
        Serial.print(light);
        Serial.print(" Percentage: ");
        Serial.print(percentage);
        Serial.print(" Office: ");
        Serial.print(office);
        Serial.print(" Lab: ");
        Serial.print(lab);
        Serial.print(" State: ");
        Serial.println(State);
      } else {
        Serial.println("Invalid data received");
      }
      accumulatedData = "";  // Clear the accumulated data
    }
  }
}
//void roundVar(){
//  percentage = round(percent);
//  office = round(office0);
//  lab = round(lab0);
//  State = round(State0);
//}
//void read_datas() {
//  static String accumulatedData;
//
//  while (Serial.available() > 0) {
//    char c = Serial.read();
//    Serial.print(c);  // Uncomment this line for debugging
//
//    accumulatedData += c;
//
//    // Check for the start and end markers
//    if (accumulatedData.endsWith("START") && c == 'D') {
//      // Process the start of a new data packet
//      accumulatedData = "START_Datas,";
//    } else if (accumulatedData.endsWith("_END")) {
//      // Process the complete data packet
//      accumulatedData.trim();
//      accumulatedData.toLowerCase();
//      // Uncomment this line for debugging
//      // Serial.println(accumulatedData);
//
//      // Parsing logic
//      if (accumulatedData.indexOf("datas") > -1) {
//        // Your existing parsing logic here
//        int pos[7];
//        for (int i = 0; i < 7; ++i) {
//          pos[i] = accumulatedData.indexOf(',', pos[i - 1] + 1);
//        }
//
//        float temp = accumulatedData.substring(pos[0] + 1, pos[1]).toFloat();
//        float hum = accumulatedData.substring(pos[1] + 1, pos[2]).toFloat();
//        float light = accumulatedData.substring(pos[2] + 1, pos[3]).toFloat();
//        float percentage = accumulatedData.substring(pos[3] + 1, pos[4]).toFloat();
//        float office = accumulatedData.substring(pos[4] + 1, pos[5]).toFloat();
//        float lab = accumulatedData.substring(pos[5] + 1, pos[6]).toFloat();
//        float State = accumulatedData.substring(pos[6] + 1).toFloat();
//
//        Serial.print("Temp: ");
//        Serial.print(temp);
//        Serial.print(" Hum: ");
//        Serial.print(hum);
//        Serial.print(" Light: ");
//        Serial.print(light);
//        Serial.print(" Percentage: ");
//        Serial.print(percentage);
//        Serial.print(" Office: ");
//        Serial.print(office);
//        Serial.print(" Lab: ");
//        Serial.print(lab);
//        Serial.print(" State: ");
//        Serial.println(State);
//      } else {
//        Serial.println("Invalid data received");
//      }
//
//      accumulatedData = "";  // Clear the accumulated data
//    }
//  }
//}


//void read_datas() {
//  static String accumulatedData;
//
//  while (Serial.available() > 0) {
//    char c = Serial.read();
//    if (c == '\n') {
//      // End of line, process the accumulated data
//      accumulatedData.trim();
//      accumulatedData.toLowerCase(); // Convert to lowercase for case-insensitive comparison
//
//      Serial.println("Received data: " + accumulatedData);
//
//      if (accumulatedData.length() > 0 && accumulatedData.indexOf("datas") > -1) {
//        delay(10); // Add a small delay to allow the entire line to be received
//
//        // Adjust the format string to match the actual data format
//        int result = sscanf(accumulatedData.c_str(), "datas,%f,%f,%f,%f,%f,%f,%f", &temp, &hum, &light, &percentage, &office, &lab, &State);
//
//        if (result == 7) {
//          Serial.print("Temp: ");
//          Serial.print(temp);
//          Serial.print(" Hum: ");
//          Serial.print(hum);
//          Serial.print(" Light: ");
//          Serial.print(light);
//          Serial.print(" Percentage: ");
//          Serial.print(percentage);
//          Serial.print(" Office: ");
//          Serial.print(office);
//          Serial.print(" Lab: ");
//          Serial.print(lab);
//          Serial.print(" State: ");
//          Serial.println(State);
//        } else {
//          Serial.println("Parsing failed");
//        }
//
//        if (State == HIGH) {
//          connectState = "CONNECTED";
//        } else if (State == LOW) {
//          connectState = "NOT CONNECTED";
//        }
//
//        Serial.println(connectState);
//
//      } else {
//        Serial.println("Invalid data received");
//      }
//
//      accumulatedData = "";  // Clear the accumulated data
//    } else {
//      // Accumulate characters until a newline is received
//      accumulatedData += c;
//    }
//  }
//}







//void read_datas() {
//  while (Serial.available() > 0) {
//    String receivedData = Serial.readStringUntil('\n');
//    receivedData.trim(); // Remove leading and trailing whitespaces
//    Serial.println("Received data: " + receivedData);  // Print raw data for debugging
//
////    Serial.println(receivedData);
//    if (receivedData.indexOf("Datas") > -1) {
//      char energyString[50]; // Adjust the size based on your actual string length
//
//      // Use sscanf to parse the data
//      int result = sscanf(receivedData.c_str(), "Datas,%f,%f,%f,%f,%f,%f,%f", &temp, &hum, &light, &percentage, &office, &lab, &State);
//      Serial.println(result);
//      // Check if sscanf successfully parsed the data
//      if (result == 7) {
//        Serial.println((String)"Temp: "+temp+" Hum: "+hum+",  Light: "+light+", Percentage: "+percentage+" Office: "+office+", Lab: "+lab+", State: "+State);
//      } else {
////        Serial.println(receivedData);
//        Serial.println("Parsing failed");
////        break;
//      }
//
//      // Check the State and set connectState
//      if (State == HIGH) {
//        connectState = "CONNECTED";
//      } else if (State == LOW) {
//        connectState = "NOT CONNECTED";
//      }
//      
//      // Print connectState
//      Serial.println(connectState);
//    }
//  }
//}


//void read_datas(){
//  while (Serial.available() > 0) {
//    String receivedData = Serial.readStringUntil('\n');
//    if (receivedData.indexOf("Datas") > -1) {
////      Serial.println(receivedData);
//      char energyString[20]; // Adjust the size based on your actual string length
//      int result = sscanf(receivedData.c_str(), "Datas,%f,%f,%f,%f,%f,%f,%f", &temp, &hum, &light, &percentage, &office, &lab, &State);
//      
//      // Check if sscanf successfully parsed the data
//      if (result == 7) {
//        Serial.println((String)"Temp: "+temp+" Hum: "+hum+",  Light: "+light+", Percentage: "+percentage+" Office: "+office+", Lab: "+lab+", State: "+State);
//      } else {
//        Serial.println("Parsing failed");
//      }
//      if (State == HIGH){
//        connectState = "CONNECTED";
//      }
//      else if (State == LOW){
//        connectState = "NOT CONNECTED";
//      }
//       Serial.println(connectState);
//    }
//  }
//}
//////////////////////////////////////////////////////////////////////////////////////////////
