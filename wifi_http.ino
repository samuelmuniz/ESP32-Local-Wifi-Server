
// EEPROM size allocated 
#define EEPROM_SIZE 80
//EEPROM address for each one of the 4 variables
#define OUTPUT26STATE_ADDR 0
#define OUTPUT27STATE_ADDR 20
#define SSDR_ADDR 40
#define PASSWORD_ADDR 60


// Load Wi-Fi library
#include <WiFi.h>
#include <EEPROM.h>

// Auxiliary functions ############################################################################
void writeStringToEEPROM(int addr, String data) {
    int len = data.length();
    for (int i = 0; i < len; i++) {
        EEPROM.write(addr + i, data[i]);
    }
    EEPROM.write(addr + len, '\0'); // Null-terminate the string
    EEPROM.commit(); // Save changes to EEPROM
}

String readStringFromEEPROM(int addr) {
    String data = "";
    char c = EEPROM.read(addr);
    int i = 0;
    while (c != '\0' && i < EEPROM_SIZE) {
        data += c;
        i++;
        c = EEPROM.read(addr + i);
    }
    return data;
}

void writeIntToEEPROM(int addr, int value) {
    byte *ptr = (byte *)&value; // Get the address of the int value
    for (int i = 0; i < sizeof(value); i++) {
        EEPROM.write(addr + i, ptr[i]); // Write each byte of the int to EEPROM
    }
    EEPROM.commit(); // Save changes to EEPROM
}

int readIntFromEEPROM(int addr) {
    int value = 0;
    byte *ptr = (byte *)&value; // Get the address of the int value
    for (int i = 0; i < sizeof(value); i++) {
        ptr[i] = EEPROM.read(addr + i); // Read each byte of the int from EEPROM
    }
    return value;
}

void setGpio(int gpio, String state ) {

  state.toUpperCase();
  if (state == "LOW"){
    digitalWrite(gpio,LOW);
  }
  else if (state == "HIGH"){
    digitalWrite(gpio,HIGH);    
  }
  else{
    Serial.println("Invalid input ...");
    digitalWrite(gpio,LOW);
  }
  

}
// Auxiliary functions ############################################################################



// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;



void setup() {


  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  /*
  String myString = "Hello, ESP32!";
  writeStringToEEPROM(0, myString);

  String retrievedString = readStringFromEEPROM(0);
  Serial.println("Retrieved String: " + retrievedString);
  */

  // Auxiliar variables to store the current output state
  writeStringToEEPROM(OUTPUT26STATE_ADDR, output26State);
  writeStringToEEPROM(OUTPUT27STATE_ADDR, output27State);

  // Initialize the output variables as outputs
  String retrievedState = readStringFromEEPROM(OUTPUT26STATE_ADDR);
  Serial.println("Retrieved Retrieved OUTPUT 27 STATE: " + retrievedState);
  pinMode(output26, OUTPUT);
  setGpio(output26,retrievedState);


  retrievedState = readStringFromEEPROM(OUTPUT27STATE_ADDR);
  Serial.println("Retrieved OUTPUT 27 STATE: " + retrievedState);
  pinMode(output27, OUTPUT);
  setGpio(output26,retrievedState);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        // DEBUG
        Serial.println("HEADER:" + header);
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              writeStringToEEPROM(OUTPUT26STATE_ADDR , output26State);
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              writeStringToEEPROM(OUTPUT26STATE_ADDR , output26State);
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              writeStringToEEPROM(OUTPUT27STATE_ADDR , output27State);
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              writeStringToEEPROM(OUTPUT27STATE_ADDR , output27State);
              digitalWrite(output27, LOW);
            } else if ((header.indexOf("GET /senha") >= 0)){
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;}");
              client.println(".form {margin: 50px auto; max-width: 500px; padding: 20px; background-color: #fff; border-radius: 5px; box-shadow: 0 0 10px rgba(0,0,0,0.2);}");
              client.println(".label { display: block; font-size: 18px;margin-bottom: 10px;}");
              client.println("input[type=\"text\"], input[type=\"password\"] {display: block; width: 100%; padding: 10px; margin-bottom: 20px; font-size: 18px; border-radius: 5px;border: 1px solid #ccc;box-sizing: border-box;}");
              client.println("input[type=\"submit\"] {background-color: #4CAF50; color: #fff; border: none; border-radius: 5px; padding: 10px 20px; font-size: 18px;cursor: pointer;}</style></head>");

              client.println("<body><h1>ESP32 Wi-Fi Configuration</h1>");
              client.println("<form method=\"post\" action=\"/ssid\">");
              client.println("<label for=\"ssid\">SSID:</label>");
              client.println("<input type=\"text\" id=\"ssid\" name=\"ssid\">");
              client.println("<label for=\"password\">Password:</label>");
              client.println("<input type=\"password\" id=\"password\" name=\"password\">");
              client.println("<input type=\"submit\" value=\"Submit\"></form></body></html>");
              client.println();

            }

            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}");
            client.println(".senha {background-color: #FFFF00;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("<p><a href=\"/senha\"><button class=\"button button2\">PASSWORD</button></a></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
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
