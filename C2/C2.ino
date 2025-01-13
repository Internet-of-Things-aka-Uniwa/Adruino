#include <SoftwareSerial.h>       // Βιβλιοθήκη για τη σειριακή επικοινωνία
SoftwareSerial espSerial(6, 7);   // Σύνδεση (Arduino RX: pin 6) με (ESP-01: TX pin)

#define DEBUG true

// Ρυθμίσεις WiFi και ThingSpeak
String mySSID = "linksys_1";
String myPWD = "";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myWriteAPI = "6UKE7N1W16R0TIOC";
String myReadAPI = "MCX66QIC4S1OR75T"; 
String myCHANNEL = "2749755";

// Ονόματα πεδίων
String fieldRed = "field1";
String fieldOrange = "field2";
String fieldGreen = "field3";
String fieldAlert = "field8";

// Διάρκεια φαναριών
int DELAY_RED = 30000;      // Καθυστέρηση για το κόκκινο φανάρι
int DELAY_ORANGE = 20000;   // Καθυστέρηση για το πορτοκαλί φανάρι
int DELAY_GREEN = 30000;    // Καθυστέρηση για το πράσινο φανάρι

String response;            // Απάντηση από το ESP8266
String sendData = "";       // Δεδομένα που θα σταλούν
int sendVal;                // Τιμή που θα σταλεί

String res_feeds = "";                  // Απάντηση από το ThingSpeak
int ret_Len;                            // Μήκος της απάντησης
int pos;                                // Θέση του πεδίου
unsigned int delayVal = 15000;          // Τιμή καθυστέρησης
String messager;                        // Μήνυμα που θα σταλεί
String x01;                             // Τιμή του πεδίου alert

int currentMillis;                      // [ΝΕΟ 1.] Τρέχων χρόνος     
int previousMillis = 0;                 // [ΝΕΟ 2.] Προηγούμενος χρόνος
const unsigned long timer = 600000;     // [ΝΕΟ 3.] Κάθε 10 λεπτά το φανάρι τίθεται εκτός λειτουργίας
const unsigned long duration = 60000;   // [ΝΕΟ 4.] Για 1 λεπτό το φανάρι τίθεται εκτός λειτουργίας

void setup() 
{    
    Serial.begin(9600);
    espSerial.begin(9600);                                                // ΣΕ ΠΕΡΙΠΤΩΣΗ ΣΦΑΛΜΑΤΟΣ, αλλάξτε το espSerial σε 9600

    Serial.println("************ ESP-01 Setup ************");
   
    espData("AT+RST", 1000, DEBUG);                                        // Επαναφορά ESP-01
    espData("AT+CWMODE=1", 1000, DEBUG);                                   // Mode=1 => client
    espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, DEBUG); // Σύνδεση στο δίκτυο WiFi
    
    while(!espSerial.find("WIFI GOT IP"))                                  // Αναμονή για σύνδεση
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
 *  Task Α.4 : Ενεργοποίηση φωτεινού σηματοδότη 
 */
    Serial.println("************ Task A.4 ************");

    setTrafficLight("RED"); 
    delay(DELAY_RED);
    setFieldValue(fieldRed, myWriteAPI, 0); // Απενεργοποίηση του κόκκινου σήματος

    setTrafficLight("GREEN");
    delay(DELAY_GREEN);
    setFieldValue(fieldGreen, myWriteAPI, 0); // Απενεργοποίηση του πράσινου σήματο

    setTrafficLight("ORANGE");
    delay(DELAY_ORANGE);
    setFieldValue(fieldOrange, myWriteAPI, 0); // Απενεργοποίηση του πορτοκαλί σήματος

    Serial.println("***************************************");

/*
 *  Task Β : Αποστολή της τιμής 0 στο κανάλι field8 που ελέγχει την λειτουργία του σηματοδότη
 */
    Serial.println("************ Task B ************");
    
    setFieldValue(fieldAlert, myWriteAPI, 0);
    Serial.println("ALERT Field set to 0.");
    
    Serial.println("***************************************");

/*
 *  Task Γ.1: Ανάγνωση της τιμής που εμπεριέχεται στο κανάλι field8 που ελέγχει την λειτουργία του σηματοδότη 
 */
    Serial.println("************ Task C.1 ************");
        
    x01 = getFieldValue(fieldAlert); // Ανάγνωση τιμής του πεδίου field8
    Serial.println("Value of ALERT field:" +x01);

    if (x01.equals("1"))
    {
        Serial.println("ALERT: Traffic Light out of order, setting the ORANGE signal.");
        setTrafficLight("ORANGE");
    }
  
    Serial.println("***************************************");

/*
 *  Task Γ.2 : Η ρύθμιση που ενημερώνει το field8 με την τιμή 1 (ΑΚΥΡΗΣ λειτουργίας) κάθε 10 λεπτά και με διάρκεια 1 λεπτού
 */
    Serial.println("************ Task C.2 ************");
    
    currentMillis = millis();
    if (currentMillis - previousMillis >= timer)
    { 
		// Θέτουμε την τιμή 1 στο πεδίο field8
        previousMillis = currentMillis;
        Serial.println("Setting the ALERT field to 1.");
        setFieldValue(fieldAlert, myWriteAPI, 1);
		
		// Θέτουμε το φανάρι σε κατάσταση εκτός λειτουργίας
		Serial.println("ALERT: Traffic Light out of order, setting the ORANGE signal.");
        setTrafficLight("ORANGE");
		
		// Για 1 λεπτό το φανάρι τίθεται εκτός λειτουργίας
		delay(duration);
		
		// Το φανάρι τίθεται ξανά σε λειτουργία
		setFieldValue(fieldAlert, myWriteAPI, 0);
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
    Serial.println("Traffic Light is set to " +color);
}

// Συνάρτηση για εγγραφή τιμής σε ένα πεδίο στο ThingSpeak
void setFieldValue(String field, String writeAPI, int value) 
{
	// HTTP GET request για εγγραφή τιμής σε κάποιο πεδίο
    sendData = "GET /update?api_key=" + writeAPI + "&" + field + "=" + String(value);
    
	// Έναρξη TCP σύνδεσης
	espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," + String(sendData.length() + 4), 1000, DEBUG);
    espSerial.find(">");
    espSerial.println(sendData);
    Serial.println("Value to be sent: ");
    Serial.println(value);

	// Τερματισμός TCP σύνδεσης
    espData("AT+CIPCLOSE=0", 1000, true);
    delay(10000);
}

// Συνάρτηση για ανάγνωση της τιμής ενός πεδίου στο ThingSpeak
String getFieldValue(String field) 
{
	// HTTP GET request για ανάγνωση του feed του καναλιού 
    messager = "GET /channels/" + myCHANNEL + "/feeds.json?api_key=" + myReadAPI + "&results=1";
    
	// Έναρξη TCP σύνδεσης
	espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    espData("AT+CIPSTO=10", 1000, DEBUG);
    espData("AT+CIPSEND=0,"+ String(messager.length() + 4), 1000, DEBUG);
    espSerial.find(">"); 
    espSerial.println(messager);
    Serial.println("Starting to read...");
	
	// Τερματισμός TCP σύνδεσης
    espData("AT+CIPCLOSE=0", 1000, DEBUG);
    delay(1000);
	
	// Ανάγνωση της απόκρισης του GET request, ώστε να εξάγουμε την τιμή του field8 από το feeds.JSON του καναλιού 
    Serial.println(response);
    ret_Len = response.length();
      
    pos = response.indexOf("[");
    res_feeds = response.substring(pos, ret_Len);
    Serial.print(res_feeds);
    pos = res_feeds.indexOf(field);

    return res_feeds.substring(pos+9, pos+10);
}

// Λειτουργία για την αποστολή εντολών AT στο ESP-01
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