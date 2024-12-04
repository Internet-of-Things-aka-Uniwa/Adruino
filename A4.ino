#include <SoftwareSerial.h>       // Serial library
SoftwareSerial espSerial(6, 7);   // Connect (Arduino RX: pin 6) to (ESP-01: TX pin)

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
String alertField = "field8";

// Traffic Light Durations
int DELAY_RED = 30000;      // Delay for RED signal
int DELAY_ORANGE = 20000;   // Delay for ORANGE signal
int DELAY_GREEN = 30000;    // Delay for GREEN signal

void setup() 
{
    Serial.begin(9600);
    espSerial.begin(9600);

    Serial.println("======== [ESP8266 Setup] ========");
    
    espData("AT+RST", 1000, true);                                      // Reset ESP-01
    espData("AT+CWMODE=1", 1000, true);                                 // Set mode to client
    espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, true); // Connect to WiFi

    while (!espSerial.find("WIFI GOT IP"))  // Wait for connection
    {                            
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected!");
    
    Serial.println("=================================");
}

void loop() {
    Serial.println("======== [Task A.4] ========");

    // Set RED signal
    setTrafficLight("RED");
    delay(DELAY_RED);

    // Set ORANGE signal
    setTrafficLight("ORANGE");
    delay(DELAY_ORANGE);

    // Set GREEN signal
    setTrafficLight("GREEN");
    delay(DELAY_GREEN);

    Serial.println("============================");
}

// Function to set the traffic light to a specific color
void setTrafficLight(String color) 
{
    String field;
    int sendVal;
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

    Serial.println("Traffic Light: " + color.toUpperCase());
    setFieldValue(field, myWriteAPI, sendVal); // Update the appropriate field in ThingSpeak
}

// Function to set a field value on ThingSpeak
void setFieldValue(String field, String writeAPI, int value) 
{
    String sendData = "GET /update?api_key=" + writeAPI + "&" + field + "=" + String(value);
    espData("AT+CIPMUX=1", 1000, true);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, true);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, true);
    espSerial.find(">");
    espSerial.println(sendData);
    espData("AT+CIPCLOSE=0", 1000, true);
}

// Function to send AT commands to ESP-01
String espData(String command, const int timeout, boolean debug) 
{
    Serial.print("AT Command ==> ");
    Serial.println(command);

    String response = "";
    espSerial.println(command);
    long int time = millis();
    while ((time + timeout) > millis()) {
        while (espSerial.available()) {
            char c = espSerial.read();
            response += c;
        }
    }
    if (debug) {
        Serial.print(response);
    }

    return response;
}
