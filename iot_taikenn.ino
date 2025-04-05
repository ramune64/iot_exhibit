#include <WiFi.h>

#include <Adafruit_NeoPixel.h>

#define PIN            27  // NeoPixelのデータピンに接続したGPIO番号
#define PIN2            22  // NeoPixelのデータピンに接続したGPIO番号
#define NUMPIXELS     63 // NeoPixelのLEDの数
#define NUMPIXELS2     65 // NeoPixelのLEDの数

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // 初期化
Adafruit_NeoPixel pixels2(NUMPIXELS2, PIN2, NEO_GRB + NEO_KHZ800);  // 初期化

const char* ssid = "rakusei-GIGA";
const char* password = "sV2B5JSb";
//const char* ssid = "ramuneESP32";
//const char* password = "ramune0141";

int red = 0;
int green = 0;
int blue = 0;
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String greenState = "off";
String redState = "off";
String blueState = "off";

// Assign output variables to GPIO pins

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


void led_change(){
  for (float i = 0; i < NUMPIXELS2 ; i++) {
    pixels2.setPixelColor(i, pixels2.Color(red, green, blue));
    
    //delay(100);     // 0.1秒待つ
  }
  for (float i = 0; i < NUMPIXELS ; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
    //delay(100);     // 0.1秒待つ
  }
  pixels2.show();  // LEDを更新
  pixels.show();  // LEDを更新
}

void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  // Set outputs to LOW

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
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
            if (header.indexOf("GET /red/on") >= 0) {
              Serial.println("red on");
              redState = "on";
              red = 150;
              led_change();
            } else if (header.indexOf("GET /red/off") >= 0) {
              Serial.println("red off");
              redState = "off";
              red = 0;
              led_change();
            } else if (header.indexOf("GET /green/on") >= 0) {
              Serial.println("green on");
              greenState = "on";
              green = 150;
              led_change();
            } else if (header.indexOf("GET /green/off") >= 0) {
              Serial.println("green off");
              greenState = "off";
              green = 0;
              led_change();
            }
            else if (header.indexOf("GET /blue/on") >= 0) {
              Serial.println("blue on");
              blueState = "on";
              blue = 150;
              led_change();
            } else if (header.indexOf("GET /blue/off") >= 0) {
              Serial.println("blue off");
              blueState = "off";
              blue = 0;
              led_change();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html{font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: gray; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;");
            client.println(".button2 {background-color: gray;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>IoT with ESP32</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            //client.println("<p>RED" + output26State + "</p>");
            client.println("<p>Red</p>");
            // If the output26State is off, it displays the ON button       
            if (redState=="on") {
              client.println("<p><a href=\"/red/off\"><button class=\"button button2\" style=\"background-color: red;\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/red/on\"><button class=\"button\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            //client.println("<p>Green" + output27State + "</p>");
            client.println("<p>Green</p>");
            // If the output27State is off, it displays the ON button       
            if (greenState=="on") {
              client.println("<p><a href=\"/green/off\"><button class=\"button button2\" style=\"background-color: green;\">ON</button></a></p>");

            } else {
              client.println("<p><a href=\"/green/on\"><button class=\"button\">OFF</button></a></p>");
            }
            // Display current state, and ON/OFF buttons for GPIO 27  
            //client.println("<p>Blue" + output27State + "</p>");
            client.println("<p>Blue</p>");
	    if (blueState=="on") {
              client.println("<p><a href=\"/blue/off\"><button class=\"button button2\" style=\"background-color: blue;\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/blue/on\"><button class=\"button\">OFF</button></a></p>");
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