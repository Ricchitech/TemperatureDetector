//-----------------------------------------------
// Author: BHARATHRAJ
// Email: brg2289@gmail.com
// Description: This code for demonstration of send data from ESP8266 into Google Spreadsheet 
// update ssid, password and GAS_ID(deployed script MACRO id)
// https://script.google.com/macros/s/AKfycbymWSSX2il-aUSc0ZCa59Jk8o_yEEZPUvdFI-OkiZELSzY71viH/exec?temperature=98.1&status=Normal&RFIDhexcode=67AB5A1190
// GAS_id =  AKfycbymWSSX2il-aUSc0ZCa59Jk8o_yEEZPUvdFI-OkiZELSzY71viH
// https://script.google.com/macros/s/AKfycbymWSSX2il-aUSc0ZCa59Jk8o_yEEZPUvdFI-OkiZELSzY71viH/exec
//-----------------------------------------------

// Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

// Defining Data
#define BUZ         D8
#define RST_PIN         D3    //9       // Configurable, see typical pin layout above
#define SS_PIN          D4    //10      // Configurable, see typical pin layout above
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define RED 0xF800

byte block;
byte len;
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;

//Creating Sensors Objects
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 object
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)


const char* ssid = "Rajeev";    // name of your wifi network
const char* pass = "qwerty2#";     // wifi pasword
const char* host = "script.google.com";
const int httpsPort = 443;


// Use WiFiclient class to create TLS connection
//WiFiClient client;
WiFiClientSecure client;


// SHA1 fingerprint of the certificate, don't care with your GAS service
//const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbymWSSX2il-aUSc0ZCa59Jk8o_yEEZPUvdFI-OkiZELSzY71viH";   // Replace by your GAS service id



String stat;
int readidsuccess;
int readnamesuccess;
byte readcard[4];
char str[32] = "";
char strn[32] = "";
String StrUID;
String StrName;


//setup code run once:
void setup() {

 // display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
  //display.clearDisplay();
  //display.display();                                         // Initialize serial communications with the PC
  
  Serial.begin(115200);  
  pinMode(BUZ, OUTPUT);
  SPI.begin();                                                  // Init SPI bus
  Serial.println("Welcome To Reva University\n");
  mfrc522.PCD_Init();  
  mlx.begin();                                            // Init MFRC522 card
  delay(10);

  Serial.println("Connecting to SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
    
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\nReady to go\n");

}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;
  

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;
   String tempvar="";
 

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  for (uint8_t i = 14; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
     // Serial.write(buffer1[i]);
      tempvar.concat(buffer1[i]);
    }
  }
  Serial.print(" ");
   Serial.print(tempvar);

  Serial.println(F("\n**End Reading**\n"));

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  String cur_stat="N";
  String cardname=tempvar;
  int it=100;
sendData(it, cur_stat,cardname );

}

// Function for Send data into Google Spreadsheet
void sendData(int tem, String stats, String hexcodes)
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);


  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }


  String temper =  String(tem, DEC); 
  String temp_stat =  String(stats); 
  String hexacodes =  String(hexcodes); 
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + temper + "&status=" + temp_stat + "&RFIDhexcode=" + hexacodes;
  Serial.print("requesting URL: ");
  Serial.println(url);
 client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}


//void loop() {
//
//     
//
//  readidsuccess = getid();
//  if(readidsuccess){
//     Serial.print(tempvar);
//   
//    float t=mlx.readObjectTempF()/20;
//     if(t<98.4)
//   {
//    stat = "Normal";
//   }
//  else
//   {
//  tone(BUZ, 1000); // Send 1KHz sound signal...
//  delay(1000);        // ...for 1 sec
//  noTone(BUZ);     // Stop sound...
//    stat = "AbNormal";
//   }
//
//   String a="Temp :";
//   String b=" *F";
//   String temperaturedata=a+t+b;
//    //Serial.println(StrUID);
//      Serial.print(tempvar);
//    //Serial.println();
//    //Serial.println(StrName);
//    String UIDresultSend, postData;
//    UIDresultSend = tempvar;
//
//   // Serial.println("Temp: ");
//     //Serial.println(t);
//    // Serial.println(" F");
//    Serial.println(temperaturedata);
//    Serial.println("Status is: "+stat);
//
//  float it = t;
//  String cur_stat = stat;
// // String hexc;
//  sendData(it, cur_stat, UIDresultSend);
//  delay(2000);
//      
//  }
//
//}
//
//
//// Function for Send data into Google Spreadsheet
//void sendData(float tem, String stats, String hexcodes)
//{
//  client.setInsecure();
//  Serial.print("connecting to ");
//  Serial.println(host);
//
//
//  if (!client.connect(host, httpsPort)) {
//    Serial.println("connection failed");
//    return;
//  }
//  tem = (tem*1000)/1000;
//
//  String temper =  String(tem, DEC); 
//  String temp_stat =  String(stats); 
//  String hexacodes =  String(hexcodes); 
//  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + temper + "&status=" + temp_stat + "&RFIDhexcode=" + hexacodes;
//  Serial.print("requesting URL: ");
//  Serial.println(url);
// client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
//  Serial.println("request sent");
//  while (client.connected()) {
//  String line = client.readStringUntil('\n');
//  if (line == "\r") {
//    Serial.println("headers received");
//    break;
//  }
//  }
//  String line = client.readStringUntil('\n');
//  if (line.startsWith("{\"state\":\"success\"")) {
//  Serial.println("Failed!!!!!!!!");
//  } else {
//  Serial.println("successfully updated!");
//  }
//  Serial.println("reply was:");
//  Serial.println("==========");
//  Serial.println(line);
//  Serial.println("==========");
//  Serial.println("closing connection");
//
//} 



// --------------------------------------------------------------------
void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
