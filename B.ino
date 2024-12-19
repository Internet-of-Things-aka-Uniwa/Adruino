#include <SoftwareSerial.h>       // Serial library
SoftwareSerial espSerial(6, 7);   // Connect (Arduino RX: pin 6) to (ESP-01: TX pin)

#define DEBUG true

// WiFi and ThingSpeak Configuration
String mySSID = "linksys_1";
String myPWD = "";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myWriteAPI = "6UKE7N1W16R0TIOC";
String myCHANNEL = "2749755";

// Field Names
String fieldRed = "field1";
String fieldOrange = "field2";
String fieldGreen = "field3";
String fieldAlert = "field8"; // [NEW 1.] Add alert field

// Traffic Light Durations
int DELAY_RED = 30000;      // Delay for RED signal
int DELAY_ORANGE = 20000;   // Delay for ORANGE signal
int DELAY_GREEN = 30000;    // Delay for GREEN signal

String response;            // Response from ESP8266
String sendData = "";       // Data to be sent
int sendVal;                // Value to be sent

void setup() 
{
    Serial.println("************ ESP-01 Setup ************");
    
    Serial.begin(9600);
    espSerial.begin(9600);                                               // IN CASE OF ERROR, change espSerial to 9600
  
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
    
    Serial.println("***************************************");
}

void loop() 
{
/*
 *  Task A.4 : Set the Traffic Light on operation 
 */
    Serial.println("************ Task A.4 ************");

    setTrafficLight("RED");
    delay(DELAY_RED);

    setTrafficLight("ORANGE");
    delay(DELAY_ORANGE);

    setTrafficLight("GREEN");
    delay(DELAY_GREEN);

    Serial.println("***************************************");

/*
 *  Task B : Set ALERT Field (Field 8) of my channel to 0 
 */
    Serial.println("************ Task B ************");
    
    setFieldValue(fieldAlert, myWriteAPI, 0);
    Serial.println("ALERT Field (Field 8) of my channel set to 0.");
    
    Serial.println("***************************************");
}

// Function to set the traffic light to a specific color
void setTrafficLight(String color) 
{
    String field;
    if (color == "RED") 
    {
        field = fieldRed;
        sendVal = 1;
    }
    else if (color == "ORANGE") 
    {
        field = fieldOrange;
        sendVal = 2;
    }
    else if (color == "GREEN")
    {
        field = fieldGreen;
        sendVal = 3;
    }
    else return;

    Serial.println("Traffic Light: " + color);
    setFieldValue(field, myWriteAPI, sendVal); // Update the appropriate field in ThingSpeak
}

// Function to set a field value on ThingSpeak
void setFieldValue(String field, String writeAPI, int value) 
{
    sendData = "GET /update?api_key=" + writeAPI + "&" + field + "=" + String(value);
    espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.println("Value to be sent: ");
    Serial.println(value);

    espData("AT+CIPCLOSE=0", 1000, true);
    delay(10000);
}

// Function to send AT commands to ESP-01
String espData(String command, const int timeout, boolean debug) 
{
    Serial.print("AT Command ==> ");
    Serial.println(command);

    response = "";
    espSerial.println(command);
    long int time = millis();
    while ((time + timeout) > millis()) 
    {
        while (espSerial.available()) 
        {
            char c = espSerial.read();
            response += c;
        }
    }
    if (debug) 
    {
       // Serial.print(response);
    }

    return response;
}
