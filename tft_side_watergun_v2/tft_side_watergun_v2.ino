#include <WiFi.h>
#include <TFT_eSPI.h>  // TFT library (make sure to include the correct library for your TFT display)
#include <HTTPClient.h>
#include <TJpg_Decoder.h>
#include "tft_stuff.h"



// const char *ssid = "ESP32-CAM-AP";  // SSID of the ESP32-CAM AP
// const char *password = "123456789";  // Password for the AP


// const char* stream_url ="http://jonny-ubuntu.local:81/";// "http://192.168.4.1:81/stream";
// const char* stream_url ="http://192.168.4.1:81/stream";
// const char* stream_url = "http://rpizero.local:7123/stream11.mjpg";



// const char* ssid = "ESP32_CAM_AP";
// const char* password = "12345678";



#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length ";//%u\r\n\r\n";

bool my_touch_read(uint16_t *x, uint16_t *y){
  bool touched = tft.getTouch(x,y);
  return touched;
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  if ( y >= tft.height() ) return 0;
  // tft.pushImage(x, y, w, h, bitmap);
  tft.pushImage(x, y, h, w, bitmap);
  return 1;
}

void processImage(uint8_t* image_data, size_t length) {
  // Serial.println("Processing full image...");
  TJpgDec.drawJpg(0, 0, (const uint8_t*)image_data, length);
  // Display or process the complete image here (e.g., render it on the TFT)
}

void drawJpeg(uint8_t* image_data, size_t length) {
  // Serial.println("Processing full image...");
  TJpgDec.drawJpg(0, 0, (const uint8_t*)image_data, length);
  // Display or process the complete image here (e.g., render it on the TFT)
}

void init_wifi_old()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    return;

    
}

void init_wifi()
{
  WiFi.mode(WIFI_STA);
  
  // Attempt first SSID
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.println("Connecting to primary WiFi...");
  }

  // If not connected after 5 seconds, try second SSID
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to primary WiFi. Trying backup...");
    // WiFi.disconnect(true);
    // delay(80);
    WiFi.begin(ssid2, password2);
    startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
      delay(500);
      Serial.println("Connecting to backup WiFi...");
    }
  }

  // Final connection result
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
  } else {
    Serial.println("Failed to connect to any WiFi network.");
  }
}


bool fire_done;
int unsigned long last_read_l;
int unsigned long last_read_f;
int unsigned long last_touch;

uint16_t px=0, py=0;
bool l_button_pushed, prev_l_button_pushed;
bool f_button_pushed, prev_f_button_pushed;
bool laser_on;
// HTTPClient http;
// WiFiClient* stream = nullptr;
void setup() {
  Serial.begin(115200);
  
  tft.init();
  tft.fillScreen(TFT_BLACK);  // Clear screen
  

  init_wifi();

    // Initialize the TFT screen
  tft.init();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  tft.fillScreen(TFT_BLACK);
  // tft.invertDisplay(true); //original and aliexpress but not elecros
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_GREEN);
  delay(200);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(4);
  tft.writecommand(ILI9341_GAMMASET); //Gamma curve selected for original one only but no aliexpress or elecrow.
  tft.writedata(2);
  delay(120);
  // tft.writecommand(ILI9341_GAMMASET); //Gamma curve selected
  tft.writedata(1);
  // analogWrite(21,64);

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);

  pinMode(laser_button, INPUT_PULLUP);
  pinMode(22, INPUT);
  pinMode(25, OUTPUT);

  xTaskCreatePinnedToCore(
       image_stream, /* Function to implement the task */
       "is",    /* Name of the task */
       1024*15,            /* Stack size in words */
       NULL,            /* Task input parameter */
       1,               /* Priority of the task */
       NULL,     /* Task handle. */
       1);              /* Core where the task should run */
 
//  xTaskCreatePinnedToCore(
//        laser_controls, /* Function to implement the task */
//        "lc",    /* Name of the task */
//        1024*5,            /* Stack size in words */
//        NULL,            /* Task input parameter */
//        1,               /* Priority of the task */
//        NULL,     /* Task handle. */
//        1);              /* Core where the task should run */
 

  // http.begin(stream_url);
  // int httpCode = http.GET();

  // if (httpCode == HTTP_CODE_OK) {
  //   stream = http.getStreamPtr();
  //   } else {
  //   Serial.printf("Failed to connect to stream, HTTP code: %d\n", httpCode);
  // }
  
  // Start the image stream
  // startStream();
  fire_done = false;
  last_read_l = millis();
  last_read_f = millis();
  last_touch = millis();

  prev_l_button_pushed = !digitalRead(laser_button); // digitalRead(laser_button);
  prev_f_button_pushed = !digitalRead(fire_button); //digitalRead(f_button);
  Serial.println("prev_l_button");
  Serial.println(prev_l_button_pushed);
  delay(1000);
  
  
}

// uint8_t *downloadBuffer = NULL;
//   int downloadBufferLength = 0;

//   uint8_t *mCurrentFrameBuffer = NULL;
//     size_t mCurrentFrameLength = 0;
//     size_t mCurrentFrameBufferLength = 0;
 void laser_controls(void *args){
  int unsigned long last_read_l;
  int unsigned long last_read_f;
  int unsigned long last_touch;
  last_read_l = millis();
  last_read_f = millis();
  last_touch = millis();
  uint16_t px=0, py=0;
  bool l_button_pushed, prev_l_button_pushed;
  bool f_button_pushed, prev_f_button_pushed;
  bool laser_on;
  prev_l_button_pushed = !digitalRead(laser_button); // digitalRead(laser_button);
  prev_f_button_pushed = !digitalRead(fire_button); //digitalRead(f_button);
  while (true){
    l_button_pushed = !digitalRead(laser_button); //high is off
    f_button_pushed = !digitalRead(fire_button); //high is off
    laser_on = digitalRead(laser);
    // Serial.print("prev l button");
    // Serial.println(prev_f_button_pushed);
    // Serial.print("l button");
    // Serial.println(l_button_pushed);
    if (millis() - last_read_l > 100 && !l_button_pushed && l_button_pushed!=prev_l_button_pushed){
      digitalWrite(laser, !laser_on); //toggle laser
      Serial.println("laser toggled");
      last_read_l = millis();
    }

    if (millis() - last_read_f > 1000 && laser_on && !f_button_pushed && f_button_pushed!=prev_f_button_pushed){
      Serial.println("target gotten");
      spray_target_with_water();
      last_read_f = millis();
      
      // sendMessage("target_get");
    }

    //squirt action

    if (!laser_on && f_button_pushed && !f_button_pushed!=prev_f_button_pushed){
      Serial.println("squirt");
      sendMessage("squirt");
      last_read_f = millis();
      
      // sendMessage("target_get");
    }

    if (millis() - last_touch > 2000 && my_touch_read(&px, &py)){
      sendMessage("reset_servos");
      last_touch = millis();
      Serial.println("reset_servos");
    }
    prev_l_button_pushed = l_button_pushed;
    prev_f_button_pushed = f_button_pushed;
    delay(50);

  }
  
 }



void loop() {
  // vTaskDelete(NULL);
    l_button_pushed = !digitalRead(laser_button); //high is off
    f_button_pushed = !digitalRead(fire_button); //high is off
    laser_on = digitalRead(laser);
    // Serial.print("prev l button");
    // Serial.println(prev_f_button_pushed);
    // Serial.print("l button");
    // Serial.println(l_button_pushed);

    if (l_button_pushed){
      Serial.println("l button pushed");
    }
    
    if (millis() - last_read_l > 100 && l_button_pushed && l_button_pushed!=prev_l_button_pushed){
      digitalWrite(laser, !laser_on); //toggle laser
      Serial.println("laser toggled");
      last_read_l = millis();
    }
    
    

    if (millis() - last_read_f > 1000 && laser_on && f_button_pushed && f_button_pushed!=prev_f_button_pushed){
      Serial.println("target gotten");
      spray_target_with_water();
      last_read_f = millis();
      
      // sendMessage("target_get");
    }
    

    //squirt action

    if (!laser_on && f_button_pushed && f_button_pushed!=prev_f_button_pushed){
      Serial.println("squirt");
      String msg = sendMessage("squirt");
      Serial.println(msg);
      // last_read_f = millis();
    }
    
    /*
    //the my_touch_read needs a semaphore: TODO
    if (millis() - last_touch > 200 && my_touch_read(&px, &py)){
      sendMessage("reset_servos");
      last_touch = millis();
      Serial.println("reset_servos");
    }
    */
    prev_l_button_pushed = l_button_pushed;
    prev_f_button_pushed = f_button_pushed;
    
    delay(50);

  //button stuff
  // if (millis() - last_read > 1000 && digitalRead(laser_button)){
  //   digitalWrite(laser, !digitalRead(laser));
  //   last_read = millis();
  // }

  // if (!fire_done && digitalRead(fire_button)){
  //   spray_target_with_water();
  //   // sendMessage("target_get");
  // }
  

  // if (stream && stream->connected()) {
  //   static bool in_image = false;
  //   static size_t content_length = 0;
  //   static uint8_t* imageBuffer = nullptr;

  //   if (stream->available()) {
  //     String line = stream->readStringUntil('\n');

  //     if (line.startsWith("--")) {
  //       in_image = false;
  //       if (imageBuffer) {
  //         drawJpeg(imageBuffer, content_length);
  //         free(imageBuffer);
  //         imageBuffer = nullptr;
  //       }
  //       content_length = 0;
  //     }

  //     if (line.startsWith("Content-Length: ")) {
  //       content_length = line.substring(15).toInt();
  //       imageBuffer = (uint8_t*)malloc(content_length);
  //       in_image = true;
  //     }

  //     if (in_image && imageBuffer) {
  //       size_t remaining = content_length;
  //       size_t bytes_read = 0;

  //       while (remaining > 0 && stream->available()) {
  //         bytes_read += stream->readBytes(imageBuffer + bytes_read, remaining);
  //         remaining = content_length - bytes_read;
  //       }

  //       if (remaining == 0) {
  //         in_image = false;
  //       }
  //     }
  //   }
  // } else {
  //   Serial.println("Stream disconnected!");
  //   http.end();
  //   http.begin(stream_url);
  //   int httpCode = http.GET();
  //   if (httpCode == HTTP_CODE_OK) {
  //     WiFiClient * stream = http.getStreamPtr();
  //   }
  // }

  // delay(10);

  ///////////////////////////////////////
  // delay(1000);
  //  if (WiFi.status() == WL_CONNECTED) {
  //   HTTPClient http;
    
  //   // Start the HTTP connection to the ESP32-CAM stream
  //   http.begin(stream_url);
  //   int httpCode = http.GET();

  //   if (httpCode == HTTP_CODE_OK) {
  //     WiFiClient * stream = http.getStreamPtr();
  //     bool in_image = false;
  //     size_t content_length = 0;
  //     uint8_t *imageBuffer = nullptr;

  //     while (stream->connected()) {
  //       if (stream->available()) {
  //         String line = stream->readStringUntil('\n');

  //         // Check for boundary marker
  //         if (line.startsWith("--")) {
  //           in_image = false;
  //           if (imageBuffer) {
  //             // Display the received image on the TFT
  //             drawJpeg(imageBuffer, content_length);
  //             free(imageBuffer);
  //             imageBuffer = nullptr;
  //           }
  //           content_length = 0;  // Reset the content length for the next image
  //         }

  //         // Check if the line is the content length header
  //         if (line.startsWith("Content-Length: ")) {
  //           content_length = line.substring(15).toInt();  // Use 16 when ends with a semi-colon
  //           Serial.printf("Content-Length: %d\n", content_length);
  //           imageBuffer = (uint8_t *)malloc(content_length);  // Allocate memory for the image
  //           in_image = true;  // Indicate that the image is coming
  //         }

  //         // If we're in the image data, read it into the buffer
  //         if (in_image && imageBuffer) {
  //           size_t remaining = content_length;
  //           size_t bytes_read = 0;

  //           while (remaining > 0 && stream->available()) {
  //             bytes_read += stream->readBytes(imageBuffer + bytes_read, remaining);
  //             remaining = content_length - bytes_read;
  //           }

  //           if (remaining == 0) {
  //             //Serial.println("Image received!");
  //             in_image = false;  // Image is fully received
  //           }
  //         }
  //       }
  //     }
  //   } else {
  //     Serial.printf("HTTP GET failed, error: %d\n", httpCode);
  //   }

  //   http.end(); // Close the HTTP connection
  // }

  // // delay(5000); // Adjust the delay as necessary for the next request
  // delay(20);
}



// void displayImage(uint8_t *imgData, int length) {
//   // Assuming imgData is in JPEG format and your TFT library can handle it
//   // You may need to implement a function that converts and displays the JPEG data
//   tft.setRotation(0);
//   tft.pushImage(0, 0, 320, 240, imgData);  // Adjust dimensions as needed
//   Serial.println("Image displayed on TFT");
// }

void image_stream(void *args){
  HTTPClient http;
  WiFiClient* stream = nullptr;

  http.begin(stream_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    stream = http.getStreamPtr();
    } else {
    Serial.printf("Failed to connect to stream, HTTP code: %d\n", httpCode);
  }
  
  while (true){
    if (stream && stream->connected()) {
    static bool in_image = false;
    static size_t content_length = 0;
    static uint8_t* imageBuffer = nullptr;

    if (stream->available()) {
      String line = stream->readStringUntil('\n');

      if (line.startsWith("--")) {
        in_image = false;
        if (imageBuffer) {
          drawJpeg(imageBuffer, content_length);
          free(imageBuffer);
          imageBuffer = nullptr;
        }
        content_length = 0;
      }

      if (line.startsWith("Content-Length: ")) {
        content_length = line.substring(15).toInt();
        imageBuffer = (uint8_t*)malloc(content_length);
        in_image = true;
      }

      if (in_image && imageBuffer) {
        size_t remaining = content_length;
        size_t bytes_read = 0;

        while (remaining > 0 && stream->available()) {
          bytes_read += stream->readBytes(imageBuffer + bytes_read, remaining);
          remaining = content_length - bytes_read;
        }

        if (remaining == 0) {
          in_image = false;
        }
      }
    }
  } else {
    Serial.println("Stream disconnected!");
    http.end();
    http.begin(stream_url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      stream = http.getStreamPtr();
    }
  }

  delay(10);
  } //while true

}