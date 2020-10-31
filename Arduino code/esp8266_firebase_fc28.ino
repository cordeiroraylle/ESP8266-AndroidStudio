#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#define FIREBASE_HOST "esp8266-fc28-firebase-1e097.firebaseio.com" //--> URL address of your Firebase Realtime Database.
#define FIREBASE_AUTH "W8CtKtsq1ejvV2GsN93ZBy50Dn8119ZmNeeHtlLU" //--> Your firebase database secret code.

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router


//   SSID and Password of your WiFi router.
const char* ssid = "brisa-1143938"; //--> Your wifi name or SSID.
const char* password = "tx4vrpdz"; //--> Your wifi password.

const int Pin = A0;          //  The pin used for the soil sensor wich is analog
const int PinBomba = D7;     // The pin used for the water pump

const int bomba = 0;         // variable to storage the pump state

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(PinBomba, OUTPUT);
  delay(500);
  
  WiFi.begin(ssid, password); //Connect to your WiFi router
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT);
  digitalWrite(ON_Board_LED, HIGH); 
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
 
  digitalWrite(ON_Board_LED, HIGH);
 
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  // Firebase Realtime Database Configuration.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  

  delay(1000);
  
 // Create pump data in the Firebase Realtime Database.
  Firebase.setString("bomba","off");

  // Conditions for handling errors.
  if (Firebase.failed()) {
      Serial.print("Failed to create Bomb Stat");
      Serial.println(Firebase.error());  
  }
}

void loop() {
  int leitura = analogRead(Pin); // Read humidity.
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(leitura)) {
    Serial.println("Failed to load sensor !");
    delay(1000);
    return;
  }

 // Show the reads on serial monitor
  Serial.print(F("Sensor: "));
  Serial.println(leitura);

// To send the data to firebase
  //Send Humidity data to the Firebase Realtime Database as interger data type
  Firebase.setInt("leitura", leitura);

  // Conditions to determinate de soil status (dry, moderate or soaked) and send them as string

  // dry
  if (leitura < 350) {
  Firebase.setString("status", "seco"); 
  delay(1000); 
 }

  // moderate
  else if (leitura >= 350 && leitura < 600) {
  Firebase.setString("status", "moderado"); 
  delay(1000); 
 }

  // soaked
 else if (leitura >=600){
  Firebase.setString("status", "encharcado"); 
  delay(1000); 
 }
 
  // Conditions for handling errors.
 if (Firebase.failed()) { 
      Serial.print("setting sensor failed :");
      Serial.println(Firebase.error());  
      delay(500);
      return;
  } 
  
// Reading the data from Database
 String getData = Firebase.getString("bomba");

  // Conditions for handling errors.
  if (Firebase.failed()) {
      Serial.print("Getting /LEDStat failed :");
      Serial.println(Firebase.error()); 
      delay(500); 
      return;
  }

   // Conditions for turning off or turning on the water pump.
  if (getData == "on") {
    digitalWrite(PinBomba, HIGH);
    Serial.println("Bomb On");
    Serial.println();
  }

  if (getData == "off") {
    digitalWrite(PinBomba, LOW);
    Serial.println("Bomb off");
    Serial.println();
  }
  
  delay(1000);
}
