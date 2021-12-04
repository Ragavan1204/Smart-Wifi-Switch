#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "RAGAVAN";
const char* password = "ragul19971998";
const char* mqtt_server = "test.mosquitto.org"; // public broker

light =0; //pin GPIO 0
fan=2;  // pin GPIO 2
char msg[50];
int timer_value = 0;
unsigned long timer;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// function works when a message arrive to the client
void control(char* topic, byte* payload, unsigned int length) {

  // Control the room light
  if ((char)payload[3] == '2') {
    digitalWrite(light, HIGH); // turn the light on
  } 
  else if ((char)payload[3] == '3'){
    digitalWrite(light, LOW); // turn the light off
  }

  // control the fan 
  if ((char)payload[3] == '1') {
    digitalWrite(fan, HIGH);   // turn the fan on
  } else if((char)payload[3] == '0') {
    digitalWrite(fan, LOW);  // Turn the fan off
    client.publish("outgoingTopic", "Timer finished"); 
  }

  // set timer for fan 
  
    int d3 = (int)payload[0]-48; // get the timer value from string
    int d2 = (int)payload[1] - 48;
    int d1 = (int)payload[2] -48;
    
    timer_value= d3*100 + d2*10 +d1;
  
}

// connect with mqqt server
void connect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("incommingTopic");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void setup() {
  pinMode(light, OUTPUT);
  pinMode(fan, OUTPUT);
  
  digitalWrite(light, HIGH);
  digitalWrite(fan, HIGH);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // configure mqtt server
  client.setCallback(control); // assign a function for callback
}

void loop() {

  if (!client.connected()) {
    connect_mqtt();
  }
  client.loop(); // check if any message recieved in the backround


  // Timer function
  if(timer_value !=0){
    pre_val = timer_value;
    timer = millis();
    snprintf (msg, 75, "Timer set to %d min", val);
    client.publish("outgoingTopic", msg);
    digitalWrite(2, HIGH); // start the fan
    
  else{
    if((millis()-timer)>(timer_value*60000)){  
      digitalWrite(fan, LOW);
      client.publish("outgoingTopic", "Timer Finished");
    }
    
  }
  }

  delay(1000);
