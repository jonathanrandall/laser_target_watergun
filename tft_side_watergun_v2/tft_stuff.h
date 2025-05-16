#ifndef TFT_STUFF
#define TFT_STUFF

#define laser_button 21 //if using cyd, use pullup resistor (so this is always on unles pressed) and set back light to -1
#define fire_button 22

#define laser 25

const char* ssid = "ESP32-CAM-AP";
const char* password = "123456789";


const char *ssid2 = "C5BF";
const char *password2 = "";

TFT_eSPI tft = TFT_eSPI();  // Initialize TFT display
// WiFiClient client;
const char* stream_url ="http://192.168.4.1:81/stream";
// const char* stream_url ="http://192.168.1.107:81/stream";// "http://192.168.4.1:81/stream";
String sendMessage(String msg);
void spray_target_with_water();

// const char* serverIP = "192.168.1.xx"; // IP of ESP32 B

void spray_target_with_water(){
  String reply = "reply_not_received";
  reply = sendMessage("target_get");
  Serial.println(reply);

  if (reply ==String("next")){
    digitalWrite(laser, 0);
    //turn off laser
    //resent message
    reply = sendMessage("laser_off");
    if ((reply == "done")){
      digitalWrite(laser, 1);
      //turn laser on
    }
  }
  else {
    Serial.println("wrong reply");
    tft.println(reply);
    delay(2000);
  }
}

String sendMessage(String msg) {
  HTTPClient http;
  // String url = "http://192.168.1.107" + String(stream_url) + "/action?go=" + String(msg);
  // String url = String("http://192.168.1.107") +  "/action?go=" + String(msg);
  String url = String("http://192.168.4.1") +  "/action?go=" + String(msg);

  http.begin(url);
  int httpCode = http.GET();
  String response = "reply_not_received";
  

  if (httpCode > 0) {
    response = http.getString();
    // Serial.println(response);
    // resp = response.c_str();
  } 
  http.end();
  return String(response);
  
}

#endif