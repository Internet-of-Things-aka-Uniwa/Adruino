#include <SoftwareSerial.h>       // Βιβλιοθήκη για σειριακή επικοινωνία
SoftwareSerial espSerial(6, 7);   // Σύνδεση (Arduino RX: pin 6) με (ESP-01: TX pin)

#define DEBUG true

// Ρυθμίσεις WiFi και ThingSpeak
String mySSID = "linksys_1";
String myPWD = "";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myWriteAPI = "6UKE7N1W16R0TIOC";
String myReadAPI = "MCX66QIC4S1OR75T"; // [ΝΕΟ 1.] Read API του καναλιού μου
String myCHANNEL = "2749755";

// Ονόματα πεδίων
String fieldRed = "field1";
String fieldOrange = "field2";
String fieldGreen = "field3";
String fieldAlert = "field8";

// Διάρκειες φωτεινού σηματοδότη
int DELAY_RED = 30000;      // Καθυστέρηση για το κόκκινο σήμα
int DELAY_ORANGE = 20000;   // Καθυστέρηση για το πορτοκαλί σήμα
int DELAY_GREEN = 30000;    // Καθυστέρηση για το πράσινο σήμα

String response;            // Απόκριση από το ESP8266
String sendData = "";       // Δεδομένα προς αποστολή
int sendVal;                // Τιμή προς αποστολή

String res_feeds = "";                  // [ΝΕΟ 2.] Απόκριση από το ThingSpeak
int ret_Len;                            // [ΝΕΟ 3.] Μήκος της απόκρισης
int pos;                                // [ΝΕΟ 4.] Θέση του πεδίου
unsigned int delayVal = 15000;          // [ΝΕΟ 5.] Τιμή καθυστέρησης
String messager;                        // [ΝΕΟ 6.] Μήνυμα προς αποστολή
String x01;                             // [ΝΕΟ 7.] Τιμή του πεδίου ειδοποίησης (alert)

void setup() 
{    
    Serial.begin(9600);
    espSerial.begin(9600);                                               // ΣΕ ΠΕΡΙΠΤΩΣΗ ΣΦΑΛΜΑΤΟΣ, αλλάξτε το espSerial σε 9600

    Serial.println("************ Ρύθμιση ESP-01 ************");
  
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
    
    Serial.println("Συνδέθηκε!");
    delay(1000);
    
    Serial.println("***************************************");
}

void loop() 
{
/*
 *  Task A.4: Θέστε τον φωτεινό σηματοδότη σε λειτουργία
 */
    Serial.println("************ Εργασία A.4 ************");

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

/*
 *  Task B: Θέστε το πεδίο ALERT σε 0
 */
    Serial.println("************ Εργασία B ************");
    
    setFieldValue(fieldAlert, myWriteAPI, 0); // Ρύθμιση του πεδίου ALERT σε 0
    Serial.println("Το πεδίο ALERT ρυθμίστηκε σε 0.");
    
    Serial.println("***************************************");

/*
 *  Task C.1: Διαβάστε το πεδίο ALERT και θέστε τον φωτεινό σηματοδότη σε ΠΟΡΤΟΚΑΛΙ αν η τιμή είναι 1
 */
    Serial.println("************ Εργασία C.1 ************");
        
    x01 = getFieldValue(fieldAlert); // Ανάγνωση τιμής του πεδίου ALERT
    Serial.println("Τιμή πεδίου ALERT:" + x01);

    if (x01 == 1)
    {
        Serial.println("ΕΙΔΟΠΟΙΗΣΗ: Ο φωτεινός σηματοδότης δεν λειτουργεί σωστά.");
        setTrafficLight("ORANGE");
    }
  
    Serial.println("***************************************");

    ret_Len = 0; 
    x01 = "";
    pos = 0;   
    res_feeds = "";
    delay(delayVal);
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
    Serial.println("Ο φωτεινός σηματοδότης ρυθμίστηκε σε " + color);
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
    Serial.println("Τιμή προς αποστολή: ");
    Serial.println(value);

    espData("AT+CIPCLOSE=0", 1000, true);
    delay(10000);
}

// [ΝΕΟ 11.] Συνάρτηση για ανάγνωση της τιμής ενός πεδίου στο ThingSpeak
String getFieldValue(String field) 
{
    messager = "GET /channels/" + myCHANNEL + "/feeds.json?api_key=" + myReadAPI + "&results=1";
    espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSTO=10", 1000, DEBUG);
    espData("AT+CIPSEND=0,"+ String(messager.length() + 4), 1000, DEBUG);
    espSerial.find(">"); 
    espSerial.println(messager);
    Serial.println("Ξεκινά ανάγνωση...");
    espData("AT+CIPCLOSE=0", 1000, DEBUG);

    delay(1000);
    Serial.println(response);
    ret_Len = response.length();
      
    pos = response.indexOf("[");
    res_feeds = response.substring(pos, ret_Len);
    Serial.print(res_feeds);
    pos = res_feeds.indexOf(field);

    return res_feeds.substring(pos+9, pos+10);
}

// Συνάρτηση για αποστολή εντολών AT στο ESP-01
String espData(String command, const int timeout, boolean debug) 
{
    Serial.print("Εντολή AT ==> ");
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
