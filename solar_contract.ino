#define labLoad 5
#define officeLoad 18
float energy1, energy2;
float State = 1;
int countdown = 0;
int result1, result2, result3;
float light;
int light_send;
float hum;
float temp;
float result;
float percentage;
float energyReading1;
float energyReading2;
String weather = "";
String advice = "";
int RTemp;
int Rhum;
float lab;
float office;
float light_send_tft;
int Renergy_reading;
int Rlight;
int count = 0;
//String Com = "";

//#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* sendUrl = "https://intelligentsolar.onrender.com/send_data"; // Replace with your Flask server's IP and port for sending data.
const char* receiveUrl = "https://intelligentsolar.onrender.com/get_real_time_data"; // Replace with your Flask server's IP and port for receiving data.
const char* deleteUrl = "https://intelligentsolar.onrender.com/delete_all_data"; // Replace with your Flask server's IP and port for receiving data.


#define DHTPIN 2          // What digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);


// Replace with your network credentials
const char* WIFI_SSID = "Airtel_4G_SMARTBOX_810C";
const char* WIFI_PASS = "516DACB5";

void setup() {
  pinMode (labLoad, OUTPUT);
  pinMode (officeLoad, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  dht.begin();
  delay(100);
  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  int con = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    State = 1;
    delay(1000);
    Serial.println("Connecting...");
    con++;
    if (con == 20){
      con = 0;
      State = 0;
      Serial.println("NOT Connected to WiFi");
      break;
    }
  }
    if (State == 1) {
      State = 1; Serial.println("Connected to WiFi");
    }
     delay(10000);
//     Serial.println("Sending delete request");
//     sendDeleteRequest();
//     Serial.println("Delete request successful");
//     delay(1000);
  }

void loop() {
  delay(10);
//  Serial.println((String)"State: "+State);
  readLight();
  readDHT();

  readEnergy();
  delay(10);
  batteryPercentage();
  delay(10);
  
  sendRequest();
  delay(50);
  receiveRequest();
  delay(10);
  sendToNano();
  switchLoads();
//  delay(1000);

}

void sendToNano(){
  // Send data with start and end delimiters
  Serial.print("START_Datas,");
  Serial.print(temp);
  Serial.print(",");
  Serial.print(hum);
  Serial.print(",");
  Serial.print(light_send_tft);
  Serial.print(",");
  Serial.print(percentage);
  Serial.print(",");
  Serial.print(office);
  Serial.print(",");
  Serial.print(lab);
  Serial.print(",");
  Serial.print(State);
  Serial.println("_END");
}

void sendRequest() {
  if (State == 0) {
    delay(1000); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }
  if (WiFi.status() == WL_CONNECTED) {
    State = 1;
    HTTPClient http;

    // Create a JSON payload with two values to send.
    DynamicJsonDocument sendJson(512); // Adjust the size as needed.
    sendJson["temp"] = temp;
    sendJson["hum"] = hum;
    sendJson["light"] = light_send;
    sendJson["percentage"] = percentage;
    sendJson["Energy_reading"] = energyReading1;
    sendJson["Energy_reading2"] = energyReading2;

    String sendPayload;
    serializeJson(sendJson, sendPayload);

    http.begin(sendUrl);
//    http.setTimeout(10000); // Set timeout to 10 seconds (default is 5000 milliseconds)
    http.addHeader("Content-Type", "application/json");

    int sendResponseCode = http.POST(sendPayload);

    if (sendResponseCode > 0) {
      String sendResponse = http.getString();
      Serial.print("Server response: ");
      Serial.println(sendResponse);  // Print the server response
    } else {
      Serial.println("Data send request failed. response code: " + String(sendResponseCode));
    }

    http.end();  // Close the connection
  }
  else if (WiFi.status() != WL_CONNECTED) {
    State = 0;
    }
}


void receiveRequest() {
  if (State == 0) {
    delay(1000); 
    WiFi.begin(WIFI_SSID, WIFI_PASS); 
  }
  if (WiFi.status() == WL_CONNECTED) {
    State = 1;
    HTTPClient http;

    http.begin(receiveUrl);

    int receiveResponseCode = http.GET();

    if (receiveResponseCode > 0) {
      String receiveResponse = http.getString();
      // Parse the JSON response with two values.
      DynamicJsonDocument receiveJson(512); // Adjust the size as needed.
      deserializeJson(receiveJson, receiveResponse);

      result = receiveJson["result"];
      office = receiveJson["office"];
      lab = receiveJson["lab"];
//      Serial.println((String)"Received value1: "+receiveResponse);
      Serial.println();
      }
    else{
      Serial.println((String)"Data receive request failed. Response code: "+receiveResponseCode);
      }

    http.end();
  }
    else if (WiFi.status() != WL_CONNECTED) {
    State = 0;
    }
}


void switchLoads(){
  digitalWrite (labLoad, lab);
  digitalWrite (officeLoad, office);
}

void readEnergy(){
  while (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    if (receivedData.indexOf("Energys") > -1) {
//      Serial.println(receivedData);
      char energyString[20]; // Adjust the size based on your actual string length
      int result = sscanf(receivedData.c_str(), "Energys,%f,%f", &energy1, &energy2);
      // Check if sscanf successfully parsed the data
      if (result == 2) {
        Serial.println((String)"Energy1: "+energy1+" Energy2: "+energy2);
      } else {
        Serial.println("Parsing failed");
      }
    }
  }
//  energyReading1 = energy1; //random(0,50);
  energyReading1 = round(energy1 * 100) / 100.0;  // Round to 2 decimal places

//  energyReading2 = energy2; //random(0,50);
  energyReading2 = round(energy2 * 100) / 100.0;  // Round to 2 decimal places
}

void readLight(){
  light = (4095-(analogRead(34))); //random(2800, 4095); //(
  if (light == 0) {
    light = 5;
  }
  float light_sen = map(light,0, 4095, 0, 50);
  light_send = round(light_sen);  // Round to 2 decimal places
  light_send_tft = round((light / 4095) * 100);
}

void readDHT(){
  float temperature = dht.readTemperature(); //random(33,45); //
  temp = round(temperature * 100) / 100.0;  // Round to 2 decimal places

  float humidity = dht.readHumidity(); // //
  hum = round(humidity * 100) / 100.0;  // Round to 2 decimal places
  if (temp == 0 || hum == 0){
    temp = 27;
    hum = 45;
  }
}


void sendDeleteRequest() {
  if (State == 0) {
    delay(1000); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }
  if (WiFi.status() == WL_CONNECTED) {
    State = 1;
    HTTPClient http;

    // Create a JSON payload with two values to send.
    DynamicJsonDocument sendJson(512); // Adjust the size as needed.
    String sendPayload;
    serializeJson(sendJson, sendPayload);

    http.begin(deleteUrl);
//    http.setTimeout(10000); // Set timeout to 10 seconds (default is 5000 milliseconds)
    http.addHeader("Content-Type", "application/json");

    int sendResponseCode = http.POST(sendPayload);

    if (sendResponseCode > 0) {
      String sendResponse = http.getString();
      Serial.print("Server response: ");
      Serial.println(sendResponse);  // Print the server response
    } else {
      Serial.println("Data send request failed. response code: " + String(sendResponseCode));
    }

    http.end();  // Close the connection
  }
  else if (WiFi.status() != WL_CONNECTED) {
    State = 0;
}
}

void batteryPercentage(){
  percentage = map(analogRead(32),2000,3300,0,100); //
  if (percentage < 0) percentage = -1;
  percentage = round(percentage * 100) / 100.0;  // Round to 2 decimal places
}

//void overAllConditions(){
//  if ((light<1000) && (temp<33) && (hum>70)){
//    result = 1;
//  }
//  else if ((light>2000) && (temp>32 && temp<36) && (hum>70)){
//    result = 2;
//  }
//  else if ((light<500) && (temp>32 && temp<36) && (hum>55 && hum<70)){
//    result = 1;
//  }
//  else if ((light<500) && (temp>35) && (hum<55)){
//    result = 2;
//  }
//  else if ((light>500 && light<1000) && (temp<28) && (hum>85)){
//    result = 1;
//  }
//  else if ((light>500 && light<1000) && (temp>35) && (hum<55)){
//    result = 2;
//  }
//  else if ((light>1000 && light<2000) && (temp>28 && temp<30) && (hum>70 && hum<85)){
//    result = 1;
//  }
//  else if ((light>2000 && light<3000) && (temp>30 && temp<36) && (hum>55 && hum<70)){
//    result = 2;
//  }
//  else if ((light>2000 && light<3000) && (temp>35) && (hum<55)){
//    result = 3;
//  }
//  else if ((light>1000 && light<2000) && (temp>35) && (hum<55)){
//    result = 2;
//  }
//  else if ((light>3000) && (temp>28 && temp<33) && (hum>70 && hum<85)){
//    result = 2;
//  }
//  else if ((light>3000) && (temp>32 && temp<36) && (hum>55 && hum<70)){
//    result = 3;
//  }
//  else if ((light > 3000) && (temp > 34) && (hum < 55)){
//    result = 4;
//  }
//  
//  else if ((light > 3000) && (temp > 34) && (hum > 55)){
//    result = 3;
//  }
//  else if ((light > 3000) && (temp < 34) && (hum < 55)){
//    result = 3;
//  }
//    else if ((light>3000) && (temp>32 && temp<36) && (hum>55 && hum<70)){
//    result = 3;
//  }
//    else if ((light>3000) && (temp>28 && temp<33) && (hum>70 && hum<85)){
//    result = 2;
//  }
//    else if ((light>3000) && (temp<28) && (hum>85)){
//    result = 1;
//  }
//  else if ((light<500) && (temp<28) && (hum<55)){
//    result = 1;
//  }
//  else if ((light>3000) && (temp>33) && (hum<70)){
//    result = 3;
//  }
//   else if (light<500){
//    result = 1;
//  }
//   else if (temp<28){
//    result = 1;
//  }
//  else if (hum>85){
//    result = 1;
//  }
//  else{
//    result = 2;
//  }
//}

//void sendRequest() {
//  if (WiFi.status() == WL_CONNECTED) {
//    HTTPClient http;
//
//    // Create a JSON payload with two values to send.
//    DynamicJsonDocument sendJson(512); // Adjust the size as needed.
//    sendJson["temp"] = temp;
//    sendJson["hum"] = hum;
//    sendJson["light"] = light_send;
//    sendJson["percentage"] = percentage;
//    sendJson["energy_reading"] = energyReading1;
//    sendJson["energy_reading2"] = energyReading2;
//
//    String sendPayload;
//    serializeJson(sendJson, sendPayload);
//
//    http.begin(sendUrl);
//    http.addHeader("Content-Type", "application/json");
//
//    int sendResponseCode = http.POST(sendPayload);
//
//    if (sendResponseCode > 0) {
//      String sendResponse = http.getString();
////      Serial.println("Data sent successfully.");
////      Serial.print("Server response: ");
////      Serial.println("Sent successfully.");
//    } else {
//      Serial.println((String)"Data send request failed. response code: "+sendResponseCode);
//    }
//
//    http.end();
//  }
//}
//
//void resultConditions(){
//  if (light>100 && light<1000){
//  result1 = 1;
//}
//  else if (light>1000 && light<3000){
//  result1 = 2;
//}
//  else if (light>3000){
//  result1 = 3;
//}
//  else if (light<100){
//  result1 = 0;
//}
//if (temp>28 && temp<33){
//  result2 = 1;
//}
//  else if (temp>32 && temp<36){
//  result2 = 2;
//}
//  else if (temp>35){
//  result2 = 3;
//}
//  else if (temp<28){
//  result2 = 0;
//}
//if (hum>70 && hum<85){
//  result3 = 1;
//}
//  else if (hum>55 && hum<70){
//  result3 = 2;
//}
//  else if (hum<55){
//  result3 = 3;
//}
//  else if (hum>85){
//  result3 = 0;
//}
//}


//void sendRequest() {
//  if (WiFi.status() == WL_CONNECTED) {
//    HTTPClient http;
//
//    // Create a JSON payload with two values to send.
//    DynamicJsonDocument sendJson(512); // Adjust the size as needed.
//    sendJson["result1"] = 1;
//    sendJson["result2"] = 0;
//    sendJson["result3"] = 2;
//
//    String sendPayload;
//    serializeJson(sendJson, sendPayload);
//
//    http.begin(sendUrl);
//    http.addHeader("Content-Type", "application/json");
//
//    int sendResponseCode = http.POST(sendPayload);
//
//    if (sendResponseCode > 0) {
//      String sendResponse = http.getString();
//      Serial.println("Data sent successfully.");
//      Serial.print("Server response: ");
//      Serial.println("Data received successfully.");
//
//      // Parse the JSON response with two values.
//      DynamicJsonDocument receiveJson(512); // Adjust the size as needed.
//      deserializeJson(receiveJson, sendResponse);
//
//      int Result = receiveJson["result"];
////      int receivedValue2 = receiveJson["value2"];
//
//      Serial.print("Received value1: ");
//      Serial.println(Result);
////      Serial.print("Received value2: ");
////      Serial.println(receivedValue2);
//    } else {
//      Serial.println("Data send request failed.");
//    }
//
//    http.end();
//  }
//}

//void upload(){
//  
//  // Write data to ThingSpeak
//  ThingSpeak.setField(1, data1);
//  delay(50);
//  ThingSpeak.setField(2, data2);
//   delay(50);
//  ThingSpeak.setField(3, data3);
//   delay(50);
//  ThingSpeak.setField(4, result1);
//   delay(50);
//  ThingSpeak.setField(5, result2);
//   delay(50);
//  ThingSpeak.setField(6, result3);
//   delay(50);
//  int response = ThingSpeak.writeFields(CHANNEL_NUMBER, THING_SPEAK_API_KEY);
//
//  if (response == 200) {
//    Serial.println("Data sent to ThingSpeak successfully!");
//  } else {
//    Serial.println("Failed to send data to ThingSpeak. Error code: " + String(response));
//  }
//}
