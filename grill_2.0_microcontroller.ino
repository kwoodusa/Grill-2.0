
/*
  Author: Kevin B. Underwood
  Project: Grill 2.0
  summary: ugly drum smoker temperature controller and meat temperature monitor
*/
#include <SPI.h> //used to communicate with WIFI chip
#include <WiFi101.h>//WiFi library for winc1500

//wifi variables
int status = WL_IDLE_STATUS; //status of wifi
WiFiServer server(80);//declare server object and specify port

//pin declarations
int fan = 2;  //Digital I/O pin the fan control is connected
int led = A2; //Analog pin the LED is connected

//user defined variables
int highTemp = 225;
int lowTemp = highTemp - 3;
int grillTemp = 0;
int meatTemp = 0;
//boolean isCelcius = false;
unsigned long timeDelay = 20000;

void setup() {
  //initialize wifi pins
  // pin 8 = CS
  // pin 7 = IRQ
  // pin 4 = RST
  WiFi.setPins(8, 7, 4);

  //initializing the digital pin for the fan control
  pinMode(fan, OUTPUT);
  //initializing the analog pin as a I/O pin for LED control
  pinMode(led, OUTPUT);
  // set analog voltage reference 0 to 5 volts
  analogReference(DEFAULT);
  // starting serial communications and set baud to 9600
  //    Serial.begin(9600);
  //    while (!Serial) {
  //      ; // wait for serial port to connect. Needed for native USB port only
  //    }
  //  Serial.println("started serial");
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  //Serial.println("wifi shield present! ");
  // connect to wifi network
  while (status != WL_CONNECTED) {
    //Serial.println("attempting to connect to wifi");
    // connect to WPA/WPA2
    status = WiFi.begin(ssid, pass);
    //give network time to connect
    delay(10000);//wait 10 seconds
  }
  //Serial.println("connected to wifi");
  server.begin();
  //Serial.println("server started");
}

void loop() {

  grillTimer();// run grill controller/monitor code

  WiFiClient client = server.available(); // listen for incoming clients

  if (client) {                 // if you get a client
    String currentLine = "";    // hold incoming data from client
    // HTTP headers always start with a response code (HTTP/1.1 200 OK)
    // and content-type so the client knows what's coming, then a blank line:
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("");
    while (client.connected()) {// while connected
      grillTimer();             // run grill controller/monitor code
      unsigned long t = millis();// time in milli seconds since the program started running
      if (t > timeDelay) {
        //HTTP response body
        client.print("Grill temp = ");
        client.println(grillTemp);
        client.print("Meat temp = ");
        client.println(meatTemp);
        client.print("High temp = ");
        client.println(highTemp);
        // HTTP response end
        client.println(" ");

        timeDelay = t + 20000;//wait another 20 seconds
      }
      if (client.available()) { // if there are bytes to read
        char c = client.read(); // read the byte
        if (c == '\n') {        // if the byte is a new line character
          currentLine = "";//getting a new line, so clear contents

        } else if (c != '\r') { // if you got anything else but a carriage return character.
          currentLine += c; // add it to the end of the currentLine
        }
//        // check the client request
//        if (currentLine.endsWith("/C")) {
//          isCelcius = true;
//        }
//        if (currentLine.endsWith("/F")) {
//          isCelcius = false;
//        }
        if (currentLine.endsWith("/T")) {
          int n = currentLine.length();
          String t = (String)currentLine.charAt(n - 5) + (String)currentLine.charAt(n - 4) + (String)currentLine.charAt(n - 3);
//          if (isCelcius) {
//            highTemp = (t.toInt() * 1.8) + 32;
//          } else {
            highTemp = t.toInt();
//          }
          //          Serial.print("current line ");
          //          Serial.println(currentLine);
          //          Serial.print("temp sent is ");
          //          Serial.println(t);
          //          Serial.println(highTemp);
        }

      }
    }
  }
}

void grillTimer() {

  // analogRead() gets the temp from the ADC on the UNO
  // then turn the digital value into a readable value
  grillTemp = (int)((analogRead(A1) * 1.76) - 418);
  meatTemp = (int)((analogRead(A3) * 1.76) - 418);
  
  delay(1000);// wait a second!!

  if (grillTemp < lowTemp) {
    // turn fan on
    digitalWrite(fan, HIGH);
    // turn on green led on proto-board
    digitalWrite(led, HIGH);
  }
  if (grillTemp > highTemp) {
    // turn fan off
    digitalWrite(fan, LOW);
    // turn off green led on proto-boards
    digitalWrite(led, LOW);
  }

}



