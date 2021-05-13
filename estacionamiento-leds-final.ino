/*LEDS */
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define LED D4
//How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

/*KY-032*/
#define infraRed D1
int valueInfraRed;


/* NETWORK CONNECTION*/
// Load Wi-Fi library
#include <ESP8266WiFi.h>
// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PASSWORD";
//Asign Static IP for the web server
IPAddress ip(192, 168, 1, 100); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;


/*SETUP FUNCTION*/
void setup() {
  
  Serial.begin(9600);
  
  // Initialize KY-032
  pinMode(infraRed, INPUT);

  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  //End of trinket special code
  pixels.begin(); // This initializes the NeoPixel library.
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

/*LOOP FUNCTION*/
void loop(){
  
  //GET VALUE KY-032
  valueInfraRed = digitalRead(infraRed);
  delay(200);

  //LEDS 
  if(valueInfraRed == 1 ) {
    for(int i=0;i<NUMPIXELS;i++){
      //pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0,255,0)); // Green Color
      pixels.show(); // This sends the updated pixel color to the hardware.
     
    }
  }else if (valueInfraRed == 0){
    for(int i=0;i<NUMPIXELS;i++){
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(255,0,0)); // RED Color
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
  }else{
    // NO SHOW NOTHING
  }
  
  //Start Web Server
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            //Configuring Server to show JSON <-------------------
            client.println("Content-Type: text/javascript");
            client.println("Access-Control-Allow-Origin: *");
            client.println();  
            if(valueInfraRed == 1 ) {
              client.println("{");
              client.print("\"status\":");
              client.println("1");
              client.println("}");
            }else if (valueInfraRed == 0){
              client.println("{");
              client.print("\"status\":");
              client.println("0");
              client.println("}");
            }
              /*SALIDAS EN EL MONITOR SERIAL*/
              //Serial.print("Status: "); Serial.println(valueInfraRed);

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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
}
