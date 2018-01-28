#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <time.h>
int outputpin= A0;

//Time config variable
int timezone = 6 * 3600;
int dst = 0;

void setup() {
 
  Serial.begin(115200);                                  //Serial connection
  WiFi.begin("DataSoft_WiFi", "support123");   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }

  //Get DateTime from Server
  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK");   
 
}
 
void loop() {
 //Time from server 
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  String day = String(p_tm->tm_mday);
  String mon = String(p_tm->tm_mon+1);
  String year = String(p_tm->tm_year + 1900);
  String hour = String(p_tm->tm_hour);
  String mint = String(p_tm->tm_min);
  String sec = String(p_tm->tm_sec);
  String curTime = hour+":"+mint+":"+sec;
  String daymon = day+"/"+mon+"/"+year+"  "+curTime;
  Serial.print(daymon);
  
   if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
    JSONencoder["sensorType"] = "Temperature";
    JSONencoder["value"] = analogRead(outputpin);
    //time_t utc = now();  //current time from the Time Library
    //eastern = usEastern.toLocal(utc);
    JSONencoder["timestamp"] = daymon;
 
    
 
   
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

 
   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://192.168.4.64:5000/sensor");      //Specify request destination
   http.addHeader("Content-Type", "application/json");  //Specify content-type header
 
   int httpCode = http.POST(JSONmessageBuffer);   //Send the request
   String payload = http.getString();                  //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(30000);  //Send a request every 30 seconds
 
}

