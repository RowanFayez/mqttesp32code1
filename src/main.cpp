#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "Mercuryk";  // Replace with your WiFi SSID
const char* password = "gehan 201818";  // Replace with your WiFi password

// MQTT broker settings
const char* mqttServer = "eba1575c50534835aaf7901fe9abeda5.s1.eu.hivemq.cloud";  // HiveMQ broker
const int mqttPort = 8883;  // Default MQTT port
// HiveMQ Cloud Let's Encrypt CA certificate
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

const char* mqttUser = "ESP32";  // If using authentication
const char* mqttPassword = "ESP32lol";  // If using authentication

WiFiClient espClient;
PubSubClient client(espClient);

// Topics for MQTT
const char* flameTopic = "firefighter/flame";
const char* smokeTopic = "firefighter/smoke";
const char* stateTopic = "firefighter/state";
const char* controlTopic = "firefighter/control";  // Topic to receive control commands
// Servo Initialization 
Servo myservo;
int pos = 0;
boolean fire = false;

#define IN1 17
#define IN2 5
#define IN3 18
#define IN4 19
#define flameL 36
#define flameM 39
#define flameR 34
#define buzzer 23
#define smoke 35
#define servoPin 32

void setup() {
  Serial.begin(115200);

  // Wi-Fi setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // MQTT setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);  // Set the MQTT callback function

  connectToMQTT();

  // Motor pins setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  _stop();

  // Flame sensor pins setup
  pinMode(flameL, INPUT);
  pinMode(flameM, INPUT);
  pinMode(flameR, INPUT);

  // Buzzer pin setup
  pinMode(buzzer, OUTPUT);

  // Smoke sensor setup
  pinMode(smoke, INPUT);

  // Servo setup
  myservo.attach(servoPin);
  myservo.write(90);
}

void loop() {
  // Reconnect if disconnected from MQTT
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();  // Handle incoming/outgoing MQTT messages

  // Sensor readings
  int smokeval = analogRead(smoke);
  int flameLVal = digitalRead(flameL);
  int flameMVal = digitalRead(flameM);
  int flameRVal = digitalRead(flameR);

  // Publish flame sensor readings
  char flameMsg[50];
  sprintf(flameMsg, "L:%d M:%d R:%d", flameLVal, flameMVal, flameRVal);
  client.publish(flameTopic, flameMsg);

  // Publish smoke sensor readings
  char smokeMsg[50];
  sprintf(smokeMsg, "%d", smokeval);
  client.publish(smokeTopic, smokeMsg);

  // Robot operation logic
  if (flameLVal == 1 && flameMVal == 1 && flameRVal == 1) {
    _stop();
    client.publish(stateTopic, "Idle");
  } else if (flameMVal == 0) {
    forword();
    delay(300);
    _stop();
    _buzzer();
    fire = true;
    client.publish(stateTopic, "In action - Front");
  } else if (flameLVal == 0) {
    left();
    delay(300);
    _stop();
    _buzzer();
    fire = true;
    client.publish(stateTopic, "In action - Left");
  } else if (flameRVal == 0) {
    right();
    delay(300);
    _stop();
    _buzzer();
    fire = true;
    client.publish(stateTopic, "In action - Right");
  }

  while (fire == true) {
    put_off_fire();
    fire = false;
    client.publish(stateTopic, "Fire extinguished");
  }

  delay(1000);  // Adjust based on how frequently you want to send updates
}

// MQTT connect function
void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Firefighter", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
      client.subscribe(controlTopic);  // Subscribe to control topic
    } else {
      Serial.print("Failed to connect, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Convert the payload into a string
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);

  // Control actions based on the message content
  if (String(topic) == controlTopic) {
    if (message == "start") {
      // Start the robot's operation
      Serial.println("Starting robot operation");
      // You can set a flag or call a function to start the robot
    } else if (message == "stop") {
      // Stop the robot's operation
      Serial.println("Stopping robot operation");
      _stop();
    } else if (message == "buzzer") {
      // Activate the buzzer
      Serial.println("Activating buzzer");
      _buzzer();
    }
    // Add more commands as needed
  }
}

// Existing functions (put_off_fire, _stop, movement functions, _buzzer) remain unchanged
// ... [Include all your existing functions here] ...

void put_off_fire() {
  _stop();
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(10);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(10);
  }
  myservo.write(90);
}

void _stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forword() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backword() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void _buzzer() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
  }
}