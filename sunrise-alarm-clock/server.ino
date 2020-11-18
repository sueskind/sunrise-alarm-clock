#include <WiFi.h>
#include <WebServer.h>

WebServer server(80); // port

int start;
int duration;
int stay;
int alive = 0;


#define IN_PIN 2
#define OUT_PIN 15
#define DIMM_TIME_DARK 7500
#define DIMM_TIME_BRIGHT 2000

volatile byte zero_cross = 0;
int dimm_time;


// If zero crossing is detected
void zero_cross_detect() {
  zero_cross = 1;
}


// Utils

void setTime(int secondsOfDay){
  struct timeval now = { .tv_sec = secondsOfDay };
  settimeofday(&now, NULL);
}


// HTTP Endpoints

void root(){
  server.send(200, "text/html", indexHtml);
  Serial.println("HTTP 200 GET /");
}

void set(){
  if(server.args() == 3 && server.hasArg("in") && server.hasArg("for") && server.hasArg("stay")){
    start = server.arg("in").toInt();
    duration = server.arg("for").toInt();
    stay = server.arg("stay").toInt();

    if(start < 0 || duration < 0 || stay < 0){
      server.send(400, "text/plain", "Bad request");
      Serial.println("HTTP 400 GET /set");
      return;
    }
    
    setTime(0);
    alive = 1;
    
    attachInterrupt(IN_PIN, zero_cross_detect, CHANGE);
  
    server.send(204);
    Serial.print("HTTP 204 GET /set in=");
    Serial.print(start);
    Serial.print(" for=");
    Serial.print(duration);
    Serial.print(" stay=");
    Serial.println(stay);
    
  } else{
    server.send(400, "text/plain", "Bad request");
    Serial.println("HTTP 400 GET /set");
  }
}

void off(){
  detachInterrupt(IN_PIN);
  digitalWrite(OUT_PIN, LOW);
  alive = 0;
  server.send(204);
  Serial.println("HTTP 204 GET /off");
}

void notFound(){
  server.send(404, "text/plain", "Not found");
  Serial.println("HTTP 404 GET ?");
}



void setup() {
  pinMode(OUT_PIN, OUTPUT);
  
  Serial.begin(115200);
  while(!Serial){;}
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  byte blinking = 1;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(OUT_PIN, blinking);
    delay(500);
    Serial.print(".");
    blinking = (blinking + 1) % 2;
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, root);
  server.on("/set", HTTP_GET, set);
  server.on("/off", HTTP_GET, off);
  server.onNotFound(notFound);
  
  server.begin();
  Serial.println("HTTP server started\n");
}


int lastUpdate = 0;
void loop() {
  if(millis() - lastUpdate > 200) {
    server.handleClient();
    lastUpdate = millis();
  }

  int t = time(NULL);
  if(alive && t > start) {
    
    if(t < start + duration + stay) {
      
      if(zero_cross) {
        dimm_time = map(t - start, 0, duration, DIMM_TIME_DARK, DIMM_TIME_BRIGHT);
        dimm_time = constrain(dimm_time, DIMM_TIME_BRIGHT, DIMM_TIME_DARK);

        // experimental values
        delayMicroseconds(dimm_time);
        digitalWrite(OUT_PIN, HIGH);
        delayMicroseconds(200);
        digitalWrite(OUT_PIN, LOW);
        
        zero_cross = 0;
      }
      
    } else {
      alive = 0;
    }
    
  } else {
    delay(500);
  }

  
}
