const unsigned int MAX_MESSAGE_LENGTH = 12;  // ορισμός μέγιστου πλήθους χαρακτήρων μηνύματος

// variables
int DELAY_GREEN = 2000;
int DELAY_YELLOW = 500;
int DELAY_RED = 2000;
String tr_light = "";
char path[100];
int i = 0;

void setup() {
 Serial.begin(9600);
 Serial.println("ROAD A");
}
void loop() {
  tr_light="RED";
  Serial.println(tr_light);
  delay(DELAY_RED);
  tr_light="YELLOW";
  Serial.println(tr_light);
  delay(DELAY_YELLOW);
  tr_light="GREEN";
  Serial.println(tr_light);
  delay(DELAY_GREEN);
 while(1){
    if (Serial.available() > 0) 
    { // Checks whether data is comming from the serial port
      path[i] = Serial.read(); // Reads the data from the serial port
      Serial.print(path[i]);
      Serial.print('\t');
    }
    
    if (path[i]=='e'){
      Serial.print("loop exit");
      break;
    }
}
}