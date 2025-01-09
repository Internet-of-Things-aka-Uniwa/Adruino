#include <SoftwareSerial.h>       // Βιβλιοθήκη για σειριακή επικοινωνία
SoftwareSerial espSerial(6, 7);   // Σύνδεση (Arduino RX: pin 6) με (ESP-01: TX pin)

#define DEBUG true

// Ρυθμίσεις WiFi και ThingSpeak
String mySSID = "linksys_1";
String myPWD = "";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myWriteAPI = "6UKE7N1W16R0TIOC";
String myCHANNEL = "2749755";

// Ονόματα πεδίων
String fieldRed = "field1";
String fieldOrange = "field2";
String fieldGreen = "field3";

// Διάρκειες φωτεινού σηματοδότη
int DELAY_RED = 30000;      // Καθυστέρηση για το κόκκινο σήμα
int DELAY_ORANGE = 20000;   // Καθυστέρηση για το πορτοκαλί σήμα
int DELAY_GREEN = 30000;    // Καθυστέρηση για το πράσινο σήμα

String response;            // Απόκριση από το ESP8266
String sendData = "";       // Δεδομένα προς αποστολή
int sendVal;                // Τιμή προς αποστολή

void setup() 
{    
    Serial.begin(9600);
    espSerial.begin(9600);                                               // ΣΕ ΠΕΡΙΠΤΩΣΗ ΣΦΑΛΜΑΤΟΣ, αλλάξτε το espSerial σε 9600

    Serial.println("************ ESP-01 Setup ************");
  
    espData("AT+RST", 1000, DEBUG);                                      // Επαναφορά ESP-01
    espData("AT+CWMODE=1", 1000, DEBUG);                                 // Mode=1 => client
    espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   // Σύνδεση στο δίκτυο WiFi
    
    while(!espSerial.find("WIFI GOT IP"))                                // Αναμονή για σύνδεση
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
 *  Task A.4 : Ενεργοποίηση φωτεινού σηματοδότη 
 */
    Serial.println("************ Task A.4 ************");

    setTrafficLight("RED"); 
    delay(DELAY_RED);
    setFieldValue(fieldRed, myWriteAPI, 0); // Απενεργοποίηση του κόκκινου φωτός

    setTrafficLight("GREEN");
    delay(DELAY_GREEN);
    setFieldValue(fieldGreen, myWriteAPI, 0); // Απενεργοποίηση του πράσινου φωτός

    setTrafficLight("ORANGE");
    delay(DELAY_ORANGE);
    setFieldValue(fieldOrange, myWriteAPI, 0); // Απενεργοποίηση του πορτοκαλί φωτός

    Serial.println("***************************************");
}

// Συνάρτηση για ρύθμιση του φωτεινού σηματοδότη σε συγκεκριμένο χρώμα
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

    setFieldValue(field, myWriteAPI, sendVal); // Ενημέρωση του αντίστοιχου πεδίου στο ThingSpeak
    Serial.println("Traffic Light is set to " + color);
}

// Συνάρτηση για ρύθμιση τιμής σε ένα πεδίο στο ThingSpeak
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

// Συνάρτηση για αποστολή εντολών AT στο ESP-01
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
