#include <SoftwareSerial.h>       // Serial library
SoftwareSerial espSerial(6, 7);   // Connect (Arduino RX: pin 6) to (ESP-01: TX pin)
                                  // and (Arduino TX: pin 7) to (ESP-01: RX pin)
#define DEBUG true
String mySSID = "linksys_1";               // WiFi SSID
String myPWD = "";                // WiFi Password
String myAPI = "7X90Y7VEBTKVZVBO";
String otherUserApi = "";

String myReadApi = "DTD07IEHUEAZD9BH";             // Read API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myCHANNEL = "1923877";

String response = "";

String field = "field2";
String alertField = "field8";
 
int sendVal;
String sendData;
String sendDataToOtherUser;
String res_feeds = "";
unsigned int delayVal = 15000;
String messager;
String x01;

int currentMillis;
int previousMillis = 0;
const unsigned long timer = 600000 // 10 minutes

void setup()
{
  Serial.begin(9600);
  espSerial.begin(9600);                                              // IN CASE OF ERROR, change espSerial to 9600
 
  espData("AT+RST", 1000, DEBUG);                                      // ESP-01 Reset
  espData("AT+CWMODE=1", 1000, DEBUG);                                 // Mode=1 => client
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   // Connect to WiFi network
  while(!espSerial.find("WIFI GOT IP"))                                // Wait for connection
  {          
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
  }
  Serial.println("Connected!");
  delay(1000);
 
}

void loop() {
  // put your main code here, to run repeatedly:
 
  // Zitoumeno A.4

  // turn on red
  sendVal = 0;                            // 0 for red
  sendData = "GET /update?api_key="+ myAPI +"&"+ field +"="+String(sendVal);
  espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal);
     
  espData("AT+CIPCLOSE=0",10000,DEBUG);
  delay(3000); // 30 sec

  // turn on orange
  sendVal = 1;    // 1 for orange
  sendData = "GET /update?api_key="+ myAPI +"&"+ field+"="+String(sendVal);
  espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  ε
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal);
     
  espData("AT+CIPCLOSE=0",10000,DEBUG);
  delay(2000); // 20 sec 

  // turn on green
  sendVal = 2;                            // 2 for green
  espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal);
     
  espData("AT+CIPCLOSE=0",10000,DEBUG);
  delay(3000); // 30 sec

  // Anti gia 0 vazoume 3 kai anti 1 vazoume 4

  // zitoumeno b.1
  sendVal = 3;                            
  sendData = "GET /update?api_key="+ otherUserApi +"&"+ alertField +"="+String(sendVal);
  espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal);

  // zitoumeno b.2
  sendVal = 3;                            
  sendData = "GET /update?api_key="+ myAPI +"&"+ alertField +"="+String(sendVal);
  espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
  espSerial.find(">");
  espSerial.println(sendData);
  Serial.print("Value to be sent: ");
  Serial.println(sendVal);

  // zitoumeno g

  // after ten minutes field 8 take the value 4
  currentMillis = millis(); // update with every loop
  if (currentMillis - previousMillis >= timer ); { 
    previousMillis = currentMillis;
    // send the 4 value to field 8
    sendVal = 4;                            
    sendData = "GET /update?api_key="+ myAPI +"&"+ alertField +"="+String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
  }

  // read the value from field 8 if 4 turn on the orange light 
  messager = "GET /channels/"+myCHANNEL+"/feeds.json?api_key="+myReadApi+"&results=60";
  espData("AT+CIPMUX=1", 1000, DEBUG);
  espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  espData("AT+CIPSTO=10",1000, DEBUG);
  espData("AT+CIPSEND=0,"+String(messager.length()+4), 1000, DEBUG);
  espSerial.find(">");
  espSerial.println(messager);
  Serial.println("Starting to read...");
  espData("AT+CIPCLOSE=0",1000,DEBUG);

  delay(1000);
  Serial.println(response);
  int ret_Len=response.length();
     
  int pos = response.indexOf("[");
  res_feeds = response.substring(pos, ret_Len);
  Serial.print(res_feeds);
  pos = res_feeds.indexOf(alertField);

  x01 = res_feeds.substring(pos+9, pos+10);
  Serial.println("ALERT  ld value:" +x01);

  if(x01 == 4) { // out of order for 1 minute
      // turn on orange
    sendVal = 1;    // 1 for orange
    sendData = "GET /update?api_key="+ myAPI +"&"+ field+"="+String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);

    delay(60000) // keep the warning sign for 1 minute
    
    // to turn off the warning sing
    sendVal = 3;                            
    sendData = "GET /update?api_key="+ myAPI +"&"+ alertField +"="+String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);               // Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
  }

  ret_Len=0;
  x01="";
  pos=0;  
  res_feeds = "";
 
}

 String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
 
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    //Serial.print(response);
  }
  return response;
}