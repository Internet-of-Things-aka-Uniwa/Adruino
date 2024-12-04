#include <SoftwareSerial.h>       // Serial library
SoftwareSerial espSerial(6, 7);   // Connect (Arduino RX: pin 6) to (ESP-01: TX pin)
                                  // and (Arduino TX: pin 7) to (ESP-01: RX pin)
#define DEBUG true
String mySSID = "linksys_1";            // WiFi SSID
String myPWD = "";                	    // WiFi Password
String myHOST = "api.thingspeak.com";   // ThingSpeak Host
String myPORT = "80";                   // ThingSpeak Port
String myCHANNEL = "2749755";           // Channel ID
String myWriteAPI = "6UKE7N1W16R0TIOC"; // Write API Key
//String otherUserWriteAPI = "";

String fieldRed = "field1";             // Field for RED signal
String fieldOrange = "field2";          // Field for ORANGE signal
String fieldGreen = "field3";           // Field for GREEN signal
String alertField = "field8";           // Field for ALERT signal
//String otherUserAlertField = "";

int DELAY_RED = 30000;                  // Delay for RED signal
int DELAY_ORANGE = 20000;               // Delay for ORANGE signal
int DELAY_GREEN = 30000;                // Delay for GREEN signal
String tr_light = "";                   // Traffic light status

int sendVal;                            // Value to be sent
String sendData = "";                   // Data to be sent

String res_feeds = "";                  // Response from ThingSpeak
unsigned int delayVal = 15000;          // Delay value
String messager;                        // Message to be sent
String x01;                             // Value of alert field

int currentMillis;                      // Current time     
int previousMillis = 0;                 // Previous time
const unsigned long timer = 600000      // 10 minutes

void setup()
{
    Serial.begin(9600);
    espSerial.begin(9600);                                               // IN CASE OF ERROR, change espSerial to 9600
 
    Serial.println("======== [ESP8266 Setup] ========");
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
    Serial.println("=================================");
    delay(1000);
}

void loop() 
{
    Serial.println("======== [Task A.4] ========");
    
    // RED Signal
    sendVal = 1;
    tr_light="RED";                         
    sendData = "GET /update?api_key=" + myWriteAPI + "&" + fieldRed + "=" + String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);   
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
        
    espData("AT+CIPCLOSE=0",10000,DEBUG);
    Serial.println(tr_light);
    delay(DELAY_RED); // 30 sec

    // ORANGE Signal
    sendVal = 2;
    tr_light="ORANGE";    						         
    sendData = "GET /update?api_key="+ myWriteAPI + "&" + fieldOrange + "=" + String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);     
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG); 
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
        
    espData("AT+CIPCLOSE=0",10000,DEBUG);
    Serial.println(tr_light);
    delay(DELAY_ORANGE); // 20 sec 

    // GREEN Signal
    sendVal = 3;    
    tr_light="GREEN";                                   
    espData("AT+CIPMUX=1", 1000, DEBUG);              
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
        
    espData("AT+CIPCLOSE=0",10000,DEBUG);
    Serial.println(tr_light);
    delay(DELAY_GREEN); // 30 sec

    Serial.println("============================");

  /*
    Serial.println("======== [Task B.1] ========");

    sendVal = 0;
    tr_light = "ALERT";                            
    sendData = "GET /update?api_key="+ otherUserWriteAPI + "&" + otherUserAlertField + "=" + String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);            
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);

    espData("AT+CIPCLOSE=0", 10000, DEBUG);
    Serial.println(tr_light);

    Serial.println("============================"); 
*/

    Serial.println("======== [Task B.2] ========");

    sendVal = 0;
    tr_light = "ALERT";                            
    sendData = "GET /update?api_key="+ myWriteAPI + "&" + alertField + "=" + String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);            
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);  
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);

    espData("AT+CIPCLOSE=0", 10000, DEBUG);
    Serial.println(tr_light);

    Serial.println("============================");
}

String espData(String command, const int timeout, boolean debug)
{
    Serial.print("AT Command ==> ");
    Serial.print(command);
    Serial.println("     ");
 
    String response = "";
    espSerial.println(command);
    long int time = millis();
    while((time + timeout) > millis())
    {
        while(espSerial.available())
        {
            char c = espSerial.read();
            response += c;
        }
    }
    if (debug)
    {
        Serial.print(response);
    }
    
    return response;
}