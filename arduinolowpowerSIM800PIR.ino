// **** INCLUDES *****
#include "LowPower.h"
#include <SoftwareSerial.h>
//#include <TextFinder.h> 
#define DEBUG true
//#define DEBUG false
SoftwareSerial GSM800(7,8); // RX,TX // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
    // This means that you need to connect the TX line from the esp to the Arduino's pin 2
    // and the RX line from the esp to the Arduino's pin 3
// Use pin 4 as wake up pin
const int wakeUpPin = 2;

int led = 12;
int count=1;
int ESPwifi = 8; // relié au gate du 2N7000
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
   
}

void loop() 
{
    // Allow wake up pin to trigger interrupt on low.
    attachInterrupt(0, wakeUp, RISING);//0>PIN_2,1>PIN_3
    
    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    
  
    // Disable external pin interrupt on wake up pin.
    detachInterrupt(0); 
    pinMode(ESPwifi, OUTPUT);
    digitalWrite(ESPwifi, HIGH);   // sets the ESP8266 ON
    delay(3000);                  // waits for 3 seconds
     sendsms();	
     pinMode(ESPwifi, INPUT);
    //digitalWrite(ESPwifi, LOW);   // sets the LED on
  Serial.println(count);
  //112 x8s-->15 min
  //75 x8s-->10 min
  //37 x8s-->5 min
  for (int i=0; i <= 30 ; i++){
      LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

   } 
   digitalWrite(13, LOW); 
  count++;
  
    // Do something here
    // Example: Read sensor, data logging, data transmission.
}

 /*
    * Name: sendData
    * Description: Function used to send data to ESP8266.
    * Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
    * Returns: The response from the esp8266 (if there is a reponse)
    */
  void sendsms(){
    Serial.println("wakeup...");
  digitalWrite(13, HIGH); 
  delay(1000);
  digitalWrite(13, LOW); 
    //Serial.println("Sending text message...");
   delay(1000);
   GSM800.begin(9600);
  delay(1000);
    GSM800.print("AT+CMGF=1\r");  // Lance le mode SMS
    delay(1000);
    GSM800.print("AT+CMGS=\"+3369530****\"\r");
  delay(1000);
    GSM800.print("SIM  ALERT !\r");     // Le texte du message
delay(1000);
  GSM800.print(char(26));  // Equivalent du Ctrl+Z (fin de texte du SMS) 
delay(1000);
  GSM800.println();
   Serial.println("OK...");
  delay(1000);
    }
