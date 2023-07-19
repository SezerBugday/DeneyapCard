#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "Deneyap.h"

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#include <ESP32Servo.h>
 #include "deneyap.h"

#include "hcsr04ESP32.h"      //sensör kütüphanesini import ediyoruz

#define trigPin D8      //sensörün trig ve echo pinlerinin bağlı olduğu pinler tanımlanır
#define echoPin D9

#define LedGaraj D12     
/****************************/
#define Oda1_red D0
#define Oda1_green D1
#define Oda1_blue D4
#define servoPin D13

#define Fan_pin D14

// Insert your network credentials
#define WIFI_SSID "*******"
#define WIFI_PASSWORD "*******"

// Insert Firebase project API Key
#define API_KEY "****************" // Firebase web api key

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "**************" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long zaman = 0;
unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;
Servo myservo;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);  

void setup() {
  pinMode(LedGaraj, OUTPUT);  
  myservo.attach(servoPin);
    pinMode(Oda1_red,OUTPUT);
    pinMode(Oda1_green,OUTPUT);
    pinMode(Oda1_blue,OUTPUT);           

     pinMode(Fan_pin,OUTPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  config.api_key = API_KEY;


  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

 
  config.token_status_callback = tokenStatusCallback; 

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {


      if (millis() - zaman > 1000) 
      {
          zaman = millis();
          float mesafe = distanceSensor.measureDistanceCm();
          Serial.println(mesafe);      
    
      if (mesafe < 0 ){
      
        }
      else{                               
   
            Serial.println(mesafe);

              if(20<=mesafe){
              
                digitalWrite(LedGaraj,1);

              }
              else{
                digitalWrite(LedGaraj,0);
              }
            
          }
         
    }

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) 
    {
      sendDataPrevMillis = millis();

    
        Firebase.RTDB.getInt(&fbdo, "/oda1");
        char oda_bir = fbdo.intData();
        if(oda_bir==0)
        {
          digitalWrite(Oda1_red,0);
          digitalWrite(Oda1_green,0);
          digitalWrite(Oda1_blue,0);
        }
        if(oda_bir==1)
        {
          digitalWrite(Oda1_red,1);
          digitalWrite(Oda1_green,0);
          digitalWrite(Oda1_blue,0);
        }
      if(oda_bir==2)
        {
          digitalWrite(Oda1_red,0);
          digitalWrite(Oda1_green,1);
          digitalWrite(Oda1_blue,0);
        }
        
        if(oda_bir==3)
        {
            digitalWrite(Oda1_red,0);
            digitalWrite(Oda1_green,0);
            digitalWrite(Oda1_blue,1);
        }
        

        if(oda_bir==10)
        {
               myservo.write(0);
            delay(15); 
        }
        if (oda_bir==11)
        {
          myservo.write(180); 
          delay(15); 

        }

        
        if(oda_bir==15)
        {
           digitalWrite(Fan_pin,1);
           Serial.println(Fan_pin);
        }
        if(oda_bir==16)
        {
          digitalWrite(Fan_pin,0);
          Serial.println(Fan_pin);
        }           


    
    }


  
 }
