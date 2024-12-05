#include <SoftwareSerial.h>       // Serial library
SoftwareSerial espSerial(6, 7);   // Connect (Arduino RX: pin 6) to (ESP-01: TX pin)

// WiFi and ThingSpeak Configuration
String mySSID = "linksys_1";
String myPWD = "";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myWriteAPI = "6UKE7N1W16R0TIOC";
String myReadAPI = "MCX66QIC4S1OR75T";
String otherChannelWriteAPI = "";
String myCHANNEL = "2749755";

// Field Names
String fieldRed = "field1";
String fieldOrange = "field2";
String fieldGreen = "field3";
String myAlertField = "field8";
String otherAlertField = "";

// Traffic Light Durations
int DELAY_RED = 30000;      // Delay for RED signal
int DELAY_ORANGE = 20000;   // Delay for ORANGE signal
int DELAY_GREEN = 30000;    // Delay for GREEN signal

String response;            // Response from ESP8266
String sendData = "";       // Data to be sent
int sendVal;                // Value to be sent

String res_feeds = "";                  // Response from ThingSpeak
int ret_Len;                            // Length of response
int pos;                                // Position of field
unsigned int delayVal = 15000;          // Delay value
String messager;                        // Message to be sent
String x01;                             // Value of alert field

int currentMillis;                      // Current time     
int previousMillis = 0;                 // Previous time
const unsigned long timer = 600000      // 10 minutes
const unsigned long outOfOrderTimer = 60000; // 1 minute 


void setup() 
{
    Serial.begin(9600);
    espSerial.begin(9600);

    Serial.println("======== [ESP8266 Setup] ========");
    
    espData("AT+RST", 1000, true);                                        // Reset ESP-01
    espData("AT+CWMODE=1", 1000, true);                                   // Set mode to client
    espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, true); // Connect to WiFi

    while (!espSerial.find("WIFI GOT IP"))  // Wait for connection
    {                            
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected!");
    
    Serial.println("=================================");
}

void loop() 
{
/*
 *  [Task A.4] ==> Set the Traffic Light on operation 
 */
    Serial.println("======== [Task A.4] ========");

    setTrafficLight("RED");
    delay(DELAY_RED);

    setTrafficLight("ORANGE");
    delay(DELAY_ORANGE);

    setTrafficLight("GREEN");
    delay(DELAY_GREEN);

    Serial.println("============================");

/*
 *  [Task B.1] ==> Set ALERT Field (Field 8) of other channel to 0 
 */
    Serial.println("======== [Task B.1] ========");
    
    setFieldValue(otherAlertField, otherChannelWriteAPI, 0);
    Serial.println("ALERT Field (Field 8) of other channel set to 0.");
    
    Serial.println("============================");

/*
 *  [Task B.2] ==> Set ALERT Field (Field 8) of my channel to 0 
 */
    Serial.println("======== [Task B.2] ========");
    
    setFieldValue(myAlertField, myWriteAPI, 0);
    Serial.println("ALERT Field (Field 8) of my channel set to 0.");
    
    Serial.println("============================");

/*
 *  [Task C.1] ==> Read ALERT Field (Field 8) of my channel and set Traffic Light to ORANGE if value is 1
 */
    Serial.println("======== [Task C.1] ========");
    
    getFieldValue(myAlertField);
    Serial.println("ALERT field value:" + x01);
    if (x01 == 1)
    {
        Serial.println("ALERT: Traffic Light out of order.");
        setTrafficLight("ORANGE");
        delay(outOfOrderTimer); // Wait for 1 minute
    }
  
    Serial.println("============================");

/*
 *  [Task C.2] ==> Set ALERT Field (Field 8) of my channel to 1 every 10 minutes
 */
    Serial.println("======== [Task C.2] ========");
    
    currentMillis = millis();
    if (currentMillis - previousMillis >= timer)
    { 
        previousMillis = currentMillis;
        setFieldValue(myAlertField, myWriteAPI, 1);
    }

    Serial.println("============================");

    ret_Len = 0; 
    x01 = "";
    pos = 0;   
    res_feeds = "";
    delay(delayVal);
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
    espData("AT+CIPMUX=1", 1000, true);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, true);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, true);
    espSerial.find(">");
    espSerial.println(sendData);
    espData("AT+CIPCLOSE=0", 1000, true);
}

// Function to get the value of a field in ThingSpeak
void getFieldValue(String field) 
{
    messager = "GET /channels/" + myCHANNEL + "/feeds.json?api_key=" + myReadAPI + "&results=1";
    espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSTO=10",1000, DEBUG);
    espData("AT+CIPSEND=0,"+ String(messager.length() + 4), 1000, DEBUG);
    espSerial.find(">"); 
    espSerial.println(messager);
    Serial.println("Starting to read...");
    espData("AT+CIPCLOSE=0", 1000, DEBUG);

    delay(1000);
    Serial.println(response);
    ret_Len = response.length();
      
    pos = response.indexOf("[");
    res_feeds = response.substring(pos, ret_Len);
    Serial.print(res_feeds);
    pos = res_feeds.indexOf(myFIELDa);

    x01 = res_feeds.substring(pos+9, pos+10);
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
        Serial.print(response);
    }

    return response;
}
