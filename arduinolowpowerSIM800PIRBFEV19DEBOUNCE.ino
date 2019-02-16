// **** INCLUDES *****
#include "LowPower.h"
#include <SoftwareSerial.h>
//#include <TextFinder.h> 
#define DEBUG true
//#define DEBUG false
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
int RST=10;                    // Pour éteindre la carte GSM SIM800L 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
const int wakeUpPin = 2;
long debouncing_time = 2000; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

int led = 12;
int count=1;
//int ESPwifi = 8; // relié au gate du 2N7000
void wakeUp()
{
   
}

void setup()
{
  //GSM800.begin(9600);  // Démarrage du modem
  Serial.begin(9600);	// Initialisation de la communication série
    // Configure wake up pin as input.
    // This will consumes few uA of current.
    pinMode(wakeUpPin, INPUT);  
     pinMode(LED_BUILTIN, OUTPUT); 
      digitalWrite(LED_BUILTIN, LOW);
   
}

void loop() 
{
    // Allow wake up pin to trigger interrupt on low.
    //attachInterrupt(0, wakeUp, RISING);//0>PIN_2,1>PIN_3
     pinMode(RST,OUTPUT);
     digitalWrite(RST,LOW); //on éteint le GSM
      
    attachInterrupt(0, interrupt1,RISING);//0>PIN_2,1>PIN_3
    
    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    
 
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(0); 
   
    //pinMode(ESPwifi, OUTPUT);
    //digitalWrite(ESPwifi, HIGH);   // sets the ESP8266 ON
    delay(3000);                  // waits for 3 seconds
     //sendsms();
     interrupt1();	
     //pinMode(ESPwifi, INPUT);
    //digitalWrite(ESPwifi, LOW);   // sets the LED on
  Serial.println(count);
  //112 x8s-->15 min
  //75 x8s-->10 min
  //37 x8s-->5 min
 // for (int i=0; i <= 30 ; i++){
  for (int i=0; i <= 2 ; i++){
      LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

   } 
   Serial.println("READY...");
  digitalWrite(LED_BUILTIN, HIGH); 
   
  count++;
  
    // Do something here
    // Example: Read sensor, data logging, data transmission.
}


  void sendsms(){
    Serial.println("wakeup...");
     pinMode(RST,INPUT);      // On allume le GSM en mettant RST en haute impédance, attendre au moins 6s pour se connecter au réseau
      digitalWrite(LED_BUILTIN, HIGH); 
      delay(6000);
  
 
  digitalWrite(LED_BUILTIN, LOW); 
    Serial.println("Sending text message...");
   delay(1000);
  serialSIM800.begin(9600);
  delay(1000);

  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  serialSIM800.write("AT+CMGS=\"0695307298\"\r\n");
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("SIM ALERTE !");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
  delay(1000);
  pinMode(RST,OUTPUT);
  digitalWrite(RST,LOW); //on éteint le GSM
    }

  void interrupt1(){
    Serial.println((long)(micros() - last_micros));
  //delay(1000);
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    Serial.println("awake");
    Serial.println(count++);
    Serial.println("awake");
    //digitalWrite(LedPin, HIGH);
    // delay(1000);
    //digitalWrite(LedPin, LOW);
    
    
    last_micros = micros();
    delay(3000);                  // waits for 3 seconds
     sendsms();  
  }

  //digitalWrite(LedPin, HIGH);
  //Serial.println("awake");
  //Serial.println(count++);
  //Serial.println(state);
  //state=0;
    
  }


