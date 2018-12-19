//requires ESPAsyncUDP: https://github.com/me-no-dev/ESPAsyncUDP
#include <ESP8266WiFi.h> //If you get an error here you need to install the ESP8266 board manager
#include <ESPAsyncE131.h> //https://github.com/forkineye/ESPAsyncE131
#include <my92xx.h> //https://github.com/xoseperez/my92xx
#include <ArduinoOTA.h> //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient

#define MY92XX_MODEL    MY92XX_MODEL_MY9291     // The MY9291 is a 4-channel driver, usually for RGBW lights
#define MY92XX_CHIPS    1                       // No daisy-chain
#define MY92XX_DI_PIN   13                      // DI GPIO
#define MY92XX_DCKI_PIN 15                      // DCKI GPIO
#define UNIVERSE 1                              // First DMX Universe to listen for
#define UNIVERSE_COUNT 1                        // Total number of Universes to listen for, starting at UNIVERSE

my92xx _my92xx = my92xx(MY92XX_MODEL, MY92XX_CHIPS, MY92XX_DI_PIN, MY92XX_DCKI_PIN, MY92XX_COMMAND_DEFAULT);
ESPAsyncE131 e131(UNIVERSE_COUNT);
WiFiClient espClient;
PubSubClient client(espClient);

//USER CONFIGURED SECTION START//
#define OTA_CLIENT_NAME    "B1_2" //This name will be used for arduinoOTA only
const char* ssid = "YourSSID";
const char* password = "YourWiFi_PW";
//USER CONFIGURED SECTION END//

int r = 0;
int g = 0;
int b = 0;
int w = 0;
uint8_t r_show = 0;
uint8_t g_show = 0;
uint8_t b_show = 0;
uint8_t w_show = 0;
int brightness = 255;
bool boot = true;
String effect = "MQTT";
char charPayload[50];
const char *mqtt_client_name = MQTT_CLIENT_NAME; 


void setup_wifi() 
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    delay(10);
    
    // Make sure you're in station mode    
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    setup_wifi();
    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.print(ssid);
    ArduinoOTA.setHostname(OTA_CLIENT_NAME);
    ArduinoOTA.begin(); 
    
    // Choose one to begin listening for E1.31 data
    if (e131.begin(E131_UNICAST))                               // Listen via Unicast
    //if (e131.begin(E131_MULTICAST, UNIVERSE, UNIVERSE_COUNT))   // Listen via Multicast
        Serial.println(F("Listening for data..."));
    else 
        Serial.println(F("*** e131.begin failed ***"));

}

void loop() {
  if (!e131.isEmpty()) {
      e131_packet_t packet;
      e131.pull(&packet);     // Pull packet from ring buffer
      
      Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
              htons(packet.universe),                 // The Universe for this packet
              htons(packet.property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
              e131.stats.num_packets,                 // Packet counter
              e131.stats.packet_errors,               // Packet error counter
              packet.property_values[1]);             // Dimmer data for Channel 1
      r_show = packet.property_values[1];
      g_show = packet.property_values[2];
      b_show = packet.property_values[3];
  }

  _my92xx.setChannel(0, r_show); 
  _my92xx.setChannel(1, g_show); 
  _my92xx.setChannel(2, b_show); 
  _my92xx.setChannel(3, 0); 
  _my92xx.setState(true);
  _my92xx.update();
  ArduinoOTA.handle();
}
