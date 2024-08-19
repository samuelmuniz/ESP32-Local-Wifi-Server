// EEPROM size allocated 
#define EEPROM_SIZE 80
//EEPROM address for each one of the 4 variables
#define OUTPUT26STATE_ADDR 0
#define OUTPUT27STATE_ADDR 20
#define SSDR_ADDR 40
#define PASSWORD_ADDR 60


//Load libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP.h>
#include <EEPROM.h>

struct config{
  char ssid[30];
  char password[30];
} user_wifi = {};

const int LED_BUILTIN = 2;
const int TOMADA_1 = 22;
const int TOMADA_2 = 23;
const char* ssid = "ESPWifi";
const char* password = "1234";
AsyncWebServer server(80);



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

void testLocalCredentials(){

  server.on("/ssid", HTTP_POST, [](AsyncWebServerRequest *request){
    String ssid_value;
    String password_value;
    //If the there are both "ssid" and "password" values
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      ssid_value = request->getParam("ssid", true)->value();
      password_value = request->getParam("password", true)->value();
      Serial.print("SSID: ");
      Serial.println(ssid_value);
      Serial.print("Password: ");
      Serial.println(password_value);
    }
    request->send(200, "text/html", "SSID: " + ssid_value + "<br>Password: " + password_value);
  });

}
// Auxiliary functions ############################################################################


// HTML/CSS Scripts ############################################################################

String credentials_html_page = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Wi-Fi Credentials</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            padding-top: 50px;
            background-color: #1a1a5e;  /* Dark blue background */
            color: white;
        }
        form {
            display: inline-block;
            text-align: left;
            background-color: white;  /* White background for the form */
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        label {
            font-size: 1.2em;
            margin-bottom: 10px;
            display: block;
            color: #1a1a5e;  /* Dark blue text color for labels */
        }
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 10px;
            margin: 10px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input[type="submit"] {
            background-color: #e60000;  /* Red background for the submit button */
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 1.2em;
        }
        input[type="submit"]:hover {
            background-color: #cc0000;  /* Slightly darker red when hovering */
        }
    </style>
</head>
<body>
    <h2>Enter Your Wi-Fi Credentials</h2>
    <form action="/submit" method="POST">
        <label for="name">Name:</label>
        <input type="text" id="name" name="name" required>

        <label for="password">Password:</label>
        <input type="password" id="password" name="password" required>

        <input type="submit" value="Submit">
    </form>
</body>
</html>
)=====";


String main_menu_html_page = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>ESP32 Controle das Tomadas</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 20px;
      text-align: center;
      background-color: #1a1a5e;
      color: #ffffff;
    }
    h1 {
      color: #ffffff;
    }
    h2 {
      color: #ffffff;
    }
    .toggle-btn {
      padding: 10px 20px;
      font-size: 16px;
      background-color: #007bff;
      color: #fff;
      border: none;
      border-radius: 5px;
      margin-bottom: 15px;
      cursor: pointer;
    }
    .toggle-btn.on {
      background-color: #28a745; /* Green when on */
    }
    .toggle-btn.off {
      background-color: #dc3545; /* Red when off */
    }
    .toggle-btn:hover {
      opacity: 0.9;
    }
  </style>
</head>
<body>
  <h1>ESP32 Controle de Tomadas</h1>
  
  <h2>Tomada 1</h2>
  <button id="toggle-btn-1" class="toggle-btn off" onclick="toggleLamp(1)">Ligar</button>
  
  <h2>Tomada 2</h2>
  <button id="toggle-btn-2" class="toggle-btn off" onclick="toggleLamp(2)">Ligar</button>

  <script>
    // Track the state of each outlet
    let lampStates = [false, false]; // false means "off", true means "on"

    function toggleLamp(lampNumber) {
      const button = document.getElementById(`toggle-btn-${lampNumber}`);
      lampStates[lampNumber - 1] = !lampStates[lampNumber - 1]; // Toggle the state

      if (lampStates[lampNumber - 1]) {
        button.textContent = 'Desligar';
        button.classList.remove('off');
        button.classList.add('on');
        // Send a request to turn the lamp on
        // Example: fetch(`/lamp${lampNumber}/on`);
      } else {
        button.textContent = 'Ligar';
        button.classList.remove('on');
        button.classList.add('off');
        // Send a request to turn the lamp off
        // Example: fetch(`/lamp${lampNumber}/off`);
      }
    }
  </script>
</body>
</html>
)=====";

String change_credentials_html_script;


// HTML/CSS Scripts ############################################################################
//standart IP = 192.168.4.1



void setup() {
  EEPROM.begin(sizeof(struct config));
  EEPROM.get(0, user_wifi);

  // Initialize the output variables as outputs
  String retrievedState = readStringFromEEPROM(OUTPUT26STATE_ADDR);
  Serial.println("Retrieved Retrieved OUTPUT 27 STATE: " + retrievedState);
  pinMode(TOMADA_1, OUTPUT);
  setGpio(TOMADA_1,retrievedState);


  retrievedState = readStringFromEEPROM(OUTPUT27STATE_ADDR);
  Serial.println("Retrieved OUTPUT 27 STATE: " + retrievedState);
  pinMode(TOMADA_2, OUTPUT);
  setGpio(TOMADA_2,retrievedState);

  WiFi.mode(WIFI_STA);
  //Erase flash memory
  Serial.begin(115200);
  // set the built-in LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT); 
  //delete old credentials
  WiFi.disconnect(true);
  // Start Access Point mode with defined name and password
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  //print SSID name
  Serial.println(WiFi.softAPSSID());
  // Print the IP address of the Access Point
  Serial.print("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();

  


  // Serve the HTML form on the root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //String html_page = credentials_html_page;
    request->send(200, "text/html", credentials_html_page);
  });

  // Handle the form submission and store the SSID and password in variables
  server.on("/ssid", HTTP_POST, [](AsyncWebServerRequest *request){
    String ssid_value;
    String password_value;
    //If the there are both "ssid" and "password" values
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      ssid_value = request->getParam("ssid", true)->value();
      password_value = request->getParam("password", true)->value();
      if (ssid_value == ssid && password_value == password){
        String html_page = main_menu_html_page;
        request->send(200, "text/html", html_page);
      }
      
    }

  });

}

void loop() {
  
  // turn the LED on
  digitalWrite(LED_BUILTIN, HIGH); 
  // wait for 0.5 second
  delay(500); 
  // turn the LED off
  digitalWrite(LED_BUILTIN, LOW); 
  // wait for 0.5 second
  delay(500);

}
