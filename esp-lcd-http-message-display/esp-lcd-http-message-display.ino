#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <U8g2lib.h>
#include <SimpleTimer.h>

#ifndef STASSID
#define STASSID "CHANGEME"
#endif
#ifndef STAPSK
#define STAPSK  "CHANGEME"
#endif
#ifndef AUTHTOKENCODE
#define AUTHTOKENCODE "Auth-Token"
#endif
#ifndef AUTHTOKENVALUE
#define AUTHTOKENVALUE "1234567890"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* authTokenCode = AUTHTOKENCODE;
const char* authTokenValue = AUTHTOKENVALUE;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4);

ESP8266WebServer server(80);

SimpleTimer timer;

long show_message_interval = 100;
String display_message = "";
unsigned long display_message_until = 0;
u8g2_uint_t x;
u8g2_uint_t offset;
u8g2_uint_t width;

void setMessage(String message, long duration) {
  display_message = message + " ";
  display_message_until = millis() + duration;
}

void showMessage() {
  // Have we finished the current message?
  if (millis() >= display_message_until) {
    u8g2.clear();
    return;
  }

  // Display the current message
  int message_len = display_message.length() + 1; 
  char message_array[message_len];
  display_message.toCharArray(message_array, message_len);
  
  width = u8g2.getUTF8Width(message_array);
  
  u8g2.firstPage();
  do {

    x = offset;
    do {
      u8g2.drawUTF8(x, 20, message_array);
      x += width;
    } while ( x < u8g2.getDisplayWidth() );

    u8g2.setCursor(0, 64);

  } while ( u8g2.nextPage() );
  
  offset -= 1;
  if ( (u8g2_uint_t)offset < (u8g2_uint_t) - width ) {
    offset = 0;
  }
}

void handleDisplay() {
  Serial.println("Method: handleDisplay");

  // Validate the token
  String providedToken = server.header(authTokenCode);
  Serial.print("Client Provided Token of:");
  Serial.println(providedToken);
  if (providedToken != authTokenValue) {
    Serial.println("EXIT: Token Denied");
    server.send(400, "text/plain", "Sorry, token is invalid");
    return;
  }

  // Do we have a message?
  if (!server.hasArg("message")) {
    Serial.println("EXIT: Message is missing");
    server.send(400, "text/plain", "Sorry, message is missing");
    return;
  }

  // Get the message
  String message = server.arg("message");
  Serial.print("Message is:");
  Serial.println(message);

  // Get the duration (display time)
  long duration = 10000;
  if (server.hasArg("duration")) {
    duration = server.arg("duration").toInt();
  }
  Serial.print("Duration is:");
  Serial.println(duration);

  // Set the message
  setMessage(message, duration);

  // Bye
  Serial.println("EXIT: Success!");
  server.send(200, "text/plain", "Success!");
}

void handleIndex() {
  Serial.println("Method: handleIndex");
  server.send(200, "text/plain", "Hello! Welcome to the ESP message client");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
  ArduinoOTA.begin();

  // Font Setup
  u8g2.begin();
  u8g2.setFont(u8g2_font_helvR18_te);
  u8g2.clear();

  // We need to collect some headers
  const char * headerkeys[] = {authTokenCode} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );

  // Set routes
  server.on("/", HTTP_GET, handleIndex);
  server.on("/display", HTTP_POST, handleDisplay);

  // Start the server!
  server.begin();

  // Display the host IP as the initial message
  String boot_message = "Host IP: " + WiFi.localIP().toString();
  Serial.println(boot_message);
  setMessage(boot_message, 120000);

  // Timer Setup
  timer.setInterval(show_message_interval);
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  if (timer.isReady()) {
    showMessage();
  }
}
