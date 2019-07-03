/*
 Save and load from serial to eeprom
 - Connect to default if there's no data in eeprom
 - Load eeprom and connect wifi from that data
 modified 3 juli 2019
 by Hudzaifah.
 -----
 ESP8266 Web Server example from
 https://randomnerdtutorials.com/esp8266-web-server-with-arduino-ide/
 by Rui Santos
*/
#include <ESP8266WiFi.h>
#include <EEPROM.h>

//-----------------------------
void clearMem(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    Serial.print(".");
  }
  EEPROM.commit();
  Serial.println("Clear done!");
}

void saveString(char add,String data){
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}

String loadString(char add){
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}
//---------------------------------
String bufClient;
WiFiServer server(80);
String header;
String output5State = "off";
String output4State = "off";
const int output5 = LED_BUILTIN;
const int output4 = 4;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
  String rxData;
  rxData = loadString(10);
  String ssid = "Redmi1";
  String pwd = "apa aja boleh";
  if (rxData != ""){
    int len = rxData.length();
    int koma = rxData.indexOf(",");
    String ssid = rxData.substring(0, koma);
    String pwd = rxData.substring(koma + 1, len);      
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pwd);
    
  } else {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pwd);
  }
  
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

void loop(){
  if (Serial.available()) {
    bufClient = "";
    while (Serial.available()) {
      bufClient += Serial.readString();       
    }
  }
  if (bufClient != ""){
    int startVal = bufClient.indexOf("(");
    int endVal = bufClient.indexOf(")");
    String cmd = bufClient.substring(0,startVal);
    String val = bufClient.substring(startVal + 1,endVal);
    if (cmd == "save"){
      Serial.print("Saving WiFi: @[address 10]");
      Serial.println(val);
      saveString(10, val);  //Address 10 and String type data
    }
    else if(cmd == "hapusWifi"){
      clearMem();
    }
    bufClient = "";
    cmd = "";
    val = "";
  }
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
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
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
