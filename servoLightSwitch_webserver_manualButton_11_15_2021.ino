/* Michael Brewer
 *  EPS as a Web server to control light switch
 *  threw the buttons on the web site and a manual button on the unit
 * 
 * 11_15_2021
 * 
 */

#include "Arduino.h"
#include <WiFi.h>
#include <ESP32Servo.h>

// Global variables
//#define buttonPin 17
#define LED_BUILTIN 2
int servoPin = 18;

// sercice interrupt
struct Button {
  const uint8_t PIN;
     bool pressed;
};
Button button1 = {17, false};
boolean motorOn = false;

// wifi/webserver
const char* WIFI_NAME= "Scytale"; 
const char* WIFI_PASSWORD = ""; 
WiFiServer server(80);

String header;

 String LED_ONE_STATE = "off";
 String LED_TWO_STATE = "off";
 String LED_THREE_STATE = "off";
 String LED_FOUR_STATE = "off";

//boolean lastButton = HIGH;
//boolean currentButton = HIGH;
//boolean servOn = true;


//prototype function
//boolean debounce(boolean last);

void IRAM_ATTR isr() {
    button1.pressed = true;
}


Servo myservo;  // create servo object to control a servo


void taskLED( void * pvParameters );
void taskWifi( void * pvParameters );

TaskHandle_t Task1;
TaskHandle_t Task2;


void setup() {

Serial.begin(115200);
 pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button1.PIN, INPUT_PULLDOWN);
  attachInterrupt(button1.PIN, isr, RISING);

// Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep


Serial.print("Connecting to ");
Serial.println(WIFI_NAME);
WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.print("Trying to connect to Wifi Network");
}
Serial.println("");
Serial.println("Successfully connected to WiFi network");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
server.begin();

 xTaskCreatePinnedToCore(
    taskLED,          /* Task function. */
    "taskLED",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    &Task1,           /* Task handle. */
    0);               /*pin task to core 0 */
delay(500);

  xTaskCreatePinnedToCore(
    taskWifi,          /* Task function. */
    "taskWifi",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    &Task2,           /* Task handle. */
    1);               /*pin task to core 1 */
delay(500);
}//end of setup


/////////////////function definitions

/* boolean debounce(boolean last)
{
  boolean current = digitalRead(buttonPin);
  if (last != current)
  {
    delay(5);
    current = digitalRead(buttonPin);
    //Serial.print(digitalRead(buttonPin));
  } //end of if statment
 return current;
} //end of debounce function */

void lightSW_on(){
   myservo.write(1);  
}
void lightSW_off(){
  myservo.write(70); 
}



///////////end of function definitions


uint8_t modeRGB = 6;
void taskLED( void * parameter )
{
/*
currentButton = debounce(lastButton);

if (lastButton == HIGH && currentButton == LOW)
  {
    motorOn = !motorOn;
  }
 lastButton = currentButton;
 Serial.print(motorOn);
 if (motorOn){

 modeRGB = 0;  
              
 }
if (!motorOn){

modeRGB = 4;  
      
 } 
 
 
 
 
 */

 
    

 
  
  while (1) {

    if (button1.pressed) {
      Serial.printf("Button 1 has been pressed %u times\n");
      motorOn = !motorOn;
      button1.pressed = false;
if (motorOn){

modeRGB = 0; 
              
 }
if (!motorOn){

modeRGB = 4;
      
 }
      
  }
 

 

 
    switch (modeRGB) {
      case 0:
        Serial.println("LED0 is ON");
        lightSW_on();
        //modeRGB = 6;
        break;
      case 1:
        Serial.println("LED1 is ON");
        lightSW_off();
        break;
      case 2:
        Serial.println("LED2 is ON");
        //special_display_2();
        break;
      case 3:
        Serial.println("BuiltIn LED is ON");
        digitalWrite(LED_BUILTIN, HIGH);
        break;
      case 4:
        Serial.println("LED'S ARE OFF O yeah!!!!");
        lightSW_off();
       // modeRGB = 6;
        break;
      case 5:
        Serial.println("BuiltIn LED is OFF");
        digitalWrite(LED_BUILTIN, LOW);
        break;
      default:
        Serial.println("LED'S ARE OFF 12345");
        //work_lighting_off();
        break;
    } //end of switch case
    
  } //end of while loop

  Serial.println("Ending task LED");
  vTaskDelete( NULL );
}//end of task LED




void loop(){
  while (1) {
    delay(1000);
  }//end of while loop
 }// end of void loop

void taskWifi( void * parameter ) {

while (1) {
WiFiClient client = server.available(); 
if (client) { 
Serial.println("New Client is requesting web page"); 
String current_data_line = ""; 
while (client.connected()) { 
if (client.available()) { 
char new_byte = client.read(); 
Serial.write(new_byte); 
header += new_byte;
if (new_byte == '\n') { 
         
if (current_data_line.length() == 0) 
{
            
client.println("HTTP/1.1 200 OK");
client.println("Content-type:text/html");
client.println("Connection: close");
client.println();
            
if (header.indexOf("LED0=ON") != -1) 
{
Serial.println("GPIO23 LED is ON");
LED_ONE_STATE = "on";
modeRGB = 0; 
} 

if (header.indexOf("LED0=OFF") != -1) 
{
Serial.println("GPIO23 LED is OFF");
LED_ONE_STATE = "off";
modeRGB = 4;
} 

if (header.indexOf("LED1=ON") != -1)
{
Serial.println("GPIO23 LED is ON");
LED_TWO_STATE = "on";
 modeRGB = 1;
}

if (header.indexOf("LED1=OFF") != -1) 
{
Serial.println("GPIO23 LED is OFF");
LED_TWO_STATE = "off";
 modeRGB = 4;  
}

if (header.indexOf("LED2=ON") != -1) 
{
Serial.println("GPIO15 LED is ON");
LED_THREE_STATE = "on";
modeRGB = 2;
 
}

if(header.indexOf("LED2=OFF") != -1)
{
Serial.println("GPIO15 LED is OFF");
LED_THREE_STATE = "off";
modeRGB = 4;
}

if (header.indexOf("LED3=ON") != -1) 
{
Serial.println("BUILT IN LED is ON");
LED_FOUR_STATE = "on";
modeRGB = 3;
 
}

if(header.indexOf("LED3=OFF") != -1)
{
Serial.println("BUILT IN LED is OFF");
LED_FOUR_STATE = "off";
modeRGB = 5;
}
            
client.println("<!DOCTYPE html><html>");
client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
client.println("<link rel=\"icon\" href=\"data:,\">");
client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
client.println(".button { background-color: #4CAF50; border: 2px solid #4CAF50;; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }");
client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"); 
// Web Page Heading
client.println("</style></head>");
client.println("<body><center><h1>ESP32 Web server to control LED strip lights over hardwear supplies</h1></center>");
client.println("<center><h2></h2></center>" );
client.println("<center><h2>Press on button to turn on LEDS and off button to turn off LEDS</h3></center>");
client.println("<form><center>");
client.println("<p> LED one is " + LED_ONE_STATE + "</p>");
// If the PIN_NUMBER_22State is off, it displays the ON button 
client.println("<center> <button class=\"button\" name=\"LED0\" value=\"ON\" type=\"submit\">LED0 ON</button>") ;
client.println("<button class=\"button\" name=\"LED0\" value=\"OFF\" type=\"submit\">LED0 OFF</button><br><br>");
client.println("<p>LED two is " + LED_TWO_STATE + "</p>");
client.println("<button class=\"button\" name=\"LED1\" value=\"ON\" type=\"submit\">LED1 ON</button>");
client.println("<button class=\"button\" name=\"LED1\" value=\"OFF\" type=\"submit\">LED1 OFF</button> <br><br>");
client.println("<p>LED three is " + LED_THREE_STATE + "</p>");
client.println ("<button class=\"button\" name=\"LED2\" value=\"ON\" type=\"submit\">LED2 ON</button>");
client.println ("<button class=\"button\" name=\"LED2\" value=\"OFF\" type=\"submit\">LED2 OFF</button></center>");
client.println("<p>ESP BUILT IN LED is " + LED_FOUR_STATE + "</p>");
client.println ("<button class=\"button\" name=\"LED3\" value=\"ON\" type=\"submit\">LED3 ON</button>");
client.println ("<button class=\"button\" name=\"LED3\" value=\"OFF\" type=\"submit\">LED3 OFF</button></center>");
client.println("</center></form></body></html>");
client.println();
break;
} 
else 
{ 
current_data_line = "";
}
} 
else if (new_byte != '\r') 
{ 
current_data_line += new_byte; 
}
}
}
// Clear the header variable
header = "";
// Close the connection
client.stop();
Serial.println("Client disconnected.");
Serial.println("");
}
}//end of while loop
}//end of task WIFI
