
#include <ESP8266WiFi.h>
#include <ESPAsyncE131.h>
#include <ArduinoOTA.h> 
#include <PubSubClient.h> 


#define UNIVERSE 1                      // First DMX Universe to listen for
#define UNIVERSE_COUNT 1                // Total number of Universes to listen for, starting at UNIVERSE

ESPAsyncE131 e131(UNIVERSE_COUNT);
WiFiClient espClient;
PubSubClient client(espClient);


//USER CONFIGURED SECTION START//
#define MQTT_CLIENT_NAME    "Novastella" //This name will be used for all MQTT Messages
const char* ssid = "YourSSID";
const char* password = "YourWiFi_PW";
const char* mqtt_server = "192.168.X.XXX"; 
const int mqtt_port = 1883;
const char *mqtt_user = "YourMQTTUserName";
const char *mqtt_pass = "YourMQTTPassword";
String effect = "MQTT"; //Set this to "E131" if you only want to control this light via xlights
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
const char *mqtt_client_name = MQTT_CLIENT_NAME; 
char charPayload[50];


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

void reconnect() 
{
  int retries = 0;
  while (!client.connected()) {
    if(retries < 5)
    {
      Serial.print("Attempting MQTT connection...");
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) 
      {
        Serial.println("connected");
        if(boot == false)
        {
          client.publish(MQTT_CLIENT_NAME "/checkIn", "Reconnected"); 
        }
        if(boot == true)
        {
          client.publish(MQTT_CLIENT_NAME "/checkIn", "Rebooted");
          boot = false;
        }
        client.subscribe(MQTT_CLIENT_NAME"/power");
        client.subscribe(MQTT_CLIENT_NAME"/color");
        client.subscribe(MQTT_CLIENT_NAME"/brightness");
        client.subscribe(MQTT_CLIENT_NAME"/white");
        client.subscribe(MQTT_CLIENT_NAME"/effect");
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        delay(5000);
      }
    }
    if(retries > 5)
    {
    ESP.restart();
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  String newTopic = topic;
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  String newPayload = String((char *)payload);
  int intPayload = newPayload.toInt();
  Serial.println(newPayload);
  Serial.println();
  newPayload.toCharArray(charPayload, newPayload.length() + 1); 
  if (newTopic == MQTT_CLIENT_NAME "/color")
  {
    client.publish(MQTT_CLIENT_NAME "/colorState", charPayload); 
    // get the position of the first and second commas
    uint8_t firstIndex = newPayload.indexOf(',');
    uint8_t lastIndex = newPayload.lastIndexOf(',');
    
    uint8_t rgb_red = newPayload.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      r = rgb_red;
      Serial.println(r);
    }
    
    uint8_t rgb_green = newPayload.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      g = rgb_green;
      Serial.println(g);
    }
    
    uint8_t rgb_blue = newPayload.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      b = rgb_blue;
      Serial.println(b);
    }
    updateLights();
  }
  if (newTopic == MQTT_CLIENT_NAME "/brightness")
  {
    client.publish(MQTT_CLIENT_NAME "/brightnessState", charPayload); 
    brightness = intPayload;
    updateLights();
  }
  if (newTopic == MQTT_CLIENT_NAME "/white")
  {
    client.publish(MQTT_CLIENT_NAME "/whiteState", charPayload); 
    w = intPayload;
    updateLights();
  }
  if (newTopic == MQTT_CLIENT_NAME "/effect")
  {
    client.publish(MQTT_CLIENT_NAME "/effectState", charPayload); 
    effect = newPayload;
    updateLights();
  }
  if (newTopic == MQTT_CLIENT_NAME "/power") 
  {
    client.publish(MQTT_CLIENT_NAME "/powerState", charPayload); 
    if (newPayload == "ON")
    {
      updateLights();
    }
    if (newPayload == "OFF")
    {
      analogWrite(4, 0);
      analogWrite(12, 0);
      analogWrite(14, 0);
      analogWrite(13, 0);
      analogWrite(5, 0);
    }
  }
}

void updateLights()
{
      r_show = (r * brightness / 255);
      g_show = (g * brightness / 255);
      b_show = (b * brightness / 255);
      w_show = (w * brightness / 255);
      analogWrite(4, r_show);
      analogWrite(12, g_show);
      analogWrite(14, b_show);
      analogWrite(13, w_show);
      analogWrite(5, w_show);
}



void setup() {
    Serial.begin(115200);
    delay(10);
    analogWriteRange(255);
    analogWriteFreq(880);
      pinMode(4, OUTPUT);
      pinMode(12, OUTPUT);
      pinMode(14, OUTPUT);
      pinMode(13, OUTPUT);
      pinMode(5, OUTPUT);
    // Make sure you're in station mode    
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    setup_wifi();
    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.print(ssid);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    ArduinoOTA.setHostname(MQTT_CLIENT_NAME);
    ArduinoOTA.begin(); 
    
    // Choose one to begin listening for E1.31 data
    if (e131.begin(E131_UNICAST))                               // Listen via Unicast
    //if (e131.begin(E131_MULTICAST, UNIVERSE, UNIVERSE_COUNT))   // Listen via Multicast
        Serial.println(F("Listening for data..."));
    else 
        Serial.println(F("*** e131.begin failed ***"));
}

void loop() {
  if(effect == "E131")
  {
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
        w_show = packet.property_values[4];
    }

      analogWrite(4, r_show);
      analogWrite(12, g_show);
      analogWrite(14, b_show);
      analogWrite(13, w_show);
      analogWrite(5, w_show);
  }
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
}
