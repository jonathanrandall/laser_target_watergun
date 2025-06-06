
#ifndef wifi_cam_h
#define wifi_cam_h

#include "esp_camera.h"

#include "camera_pins.h"        // Must include this after setting
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "esp_http_server.h"
#include <ESP32Servo.h>

#include "helpers.h"

#define laser_pin 13

bool last_region_g4 = true;
bool toggle_track = true;

unsigned long squirt_tm;
bool tracking_on = false;
bool got_laser_image = false;

bool calibrate = false;
// Replace with your network credentials
// const char* ssid = "WiFi-C5BF";
// const char* password = "";

const char* ssid = "ESP32-CAM-AP";
const char* password = "123456789";

#define PART_BOUNDARY "123456789000000000000987654321"


static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t camera_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

bool configure_camera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE; //PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = MY_FRAMESIZE; //FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = MY_FRAMESIZE; //FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  return true;
}

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<html>
  <head>
    <title>ESP32-CAM Robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      table { margin-left: auto; margin-right: auto; }
      td { padding: 8 px; }
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      img {  width: auto ;
        max-width: 100% ;
        height: auto ; 
      }
    </style>
  </head>
  <body>
    <h1>ESP32-CAM Pan and Tilt</h1>
    <img src="" id="photo" >
    <table>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('up');" ontouchstart="toggleCheckbox('up');">Up</button></td></tr>
      <tr><td align="center"><button class="button" onmousedown="toggleCheckbox('left');" ontouchstart="toggleCheckbox('left');">Left</button></td><td align="center"></td><td align="center"><button class="button" onmousedown="toggleCheckbox('right');" ontouchstart="toggleCheckbox('right');">Right</button></td></tr>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('down');" ontouchstart="toggleCheckbox('down');">Down</button></td></tr>                   
    </table>
     <table>
      <tr><td align="center"><button class="button" onmousedown="toggleCheckbox('squirt');" ontouchstart="toggleCheckbox('squirt');">Squirt</button></td><td align="center"></td>
      <td align="center"><button class="button" onmousedown="toggleCheckbox('calibrate');" ontouchstart="toggleCheckbox('calibrate');">Calibrate</button></td>
      <td align="center"><button class="button" onmousedown="toggleCheckbox('reset_servos');" ontouchstart="toggleCheckbox('reset_servos');">Reset Servos</button></td>
      </tr>
     </table>
     <table>
      <tr><td align="center"><button class="button" onmousedown="toggleCheckbox('target_get');" ontouchstart="toggleCheckbox('target_get');">target_get</button></td><td align="center"></td>
      <td align="center"><button class="button" onmousedown="toggleCheckbox('laser_off');" ontouchstart="toggleCheckbox('laser_off');">laser_off</button></td>
      </tr>
     </table>
       
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
     xhr.send();
   }
   window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req){
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

static esp_err_t stream_handler(httpd_req_t *req){
  fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  float y_error = 0.0, y_error_prev = 0.0;
  float x_error = 0.0, x_error_prev = 0.0;
  float x_cummulative_error = 0.0, y_cummulative_error = 0.0;
  long unsigned int calibrate_last = 0;

  float positionx0, positiony0;

  float x_move_cummulative = 0.0, y_move_cummulative = 0.0;

  float p_mult = 0.4, i_mult = 0.02, d_mult = 0.06; //pid controller coefficients

  bool pos0 = true;
  

  while(true){
    //grab semaphor
    xSemaphoreTake(Semaphore_Controls,  portMAX_DELAY);
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      //put the clibration in here
      if (calibrate ){
        
        
        if (!got_laser_image ){ //&& (millis()-calibrate_last>1000)
          
          if (fb) {
             esp_camera_fb_return(fb);
             delay(50);
          }
          fb = esp_camera_fb_get();
          capture_still();
          update_frame();
          got_laser_image = true;
          digitalWrite(laser_pin, LOW);
          delay(100);
          Serial.println("getting laser image");
        } else if(got_laser_image) {
          Serial.println("in got laser image");
          if (fb) {
             esp_camera_fb_return(fb);
             delay(50);
          }
          fb = esp_camera_fb_get();
          delay(50);
          capture_still();
          get_diff_image();
          // print_frame(diff_frame); //only for debugging
          position = find_bright_spot_centroid();
          got_laser_image = false;
          y_error_prev = y_error;
          x_error_prev = x_error;
          
          digitalWrite(laser_pin, HIGH);
          

          if (pos0 && position.valid){
            positionx0 = position.x;
            positiony0 = position.y;
            pos0 = false;
          }

          if (position.valid){
            y_error = 2*30.0 - position.y;
            x_error = 2*40.0 - position.x;
            x_cummulative_error+=x_error;
            y_cummulative_error+=y_error;
            x_move_calibrate = p_mult*x_error + i_mult*x_cummulative_error + d_mult*(x_error - x_error_prev);
            y_move_calibrate = p_mult*y_error + i_mult*y_cummulative_error + d_mult*(y_error - y_error_prev);

          } else {
            x_move_calibrate = 0.0;
            y_move_calibrate = 0.0;
          }

          Serial.println("--------------------------------------------------");
          Serial.println(String("start positions: x = ") + String((int) positionx0) + String(", y = ") + String((int) positiony0));          
          Serial.println(String("total moves: x = ") + String((int) x_move_cummulative) + ", y = " + String((int) y_move_cummulative));
          Serial.println("current postions: x = " + String((int) position.x) + ", y = " + String((int) position.y));
          Serial.println("next move: x = " + String((int) x_move_calibrate) + ", y = " + String((int) y_move_calibrate));
          Serial.println("--------------------------------------------------");

          move_servos_inloop = true;
          x_move_cummulative+=x_move_calibrate;
          y_move_cummulative+=y_move_calibrate;
          // adjust_servos((int) x_move_calibrate, 0);//(int) -1.0*y_move_calibrate);//, 0); //(int) y_move_calibrate);
          // adjust_servos(0, (int) y_move_calibrate);//(int) -1.0*y_move_calibrate);//, 0); //(int) y_move_calibrate);
          adjust_servos((int) x_move_calibrate, (int) y_move_calibrate);

          delay(1600);

          calibrate_last = millis();

        }

        cnt++;
        if (got_laser_image==false && ((cnt >= 12) || 
          (position.valid && abs(y_error) < 5 && abs(x_error)<5))) {
          Serial.println(cnt);
          // reset_all();
          cnt = 0;
          // got_laser_image = false;
          calibrate = false;
          calibrate_last = 0;
          x_move_cummulative = 0.0;
          y_move_cummulative = 0.0;
          y_error = 0.0; y_error_prev = 0.0;
          x_error = 0.0; x_error_prev = 0.0;
          x_cummulative_error = 0.0;
          y_cummulative_error = 0.0;
          pos0 = true;
        }
      }
      
      //end calibrartion tracking and convert to jpeg for transportation
      if(fb->width > 100){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      // _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    xSemaphoreGive(Semaphore_Controls);
    delay(20); //time for the waitig task to take the semaphor
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}

static esp_err_t cmd_handler(httpd_req_t *req){
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  String reply = "put_reply_here";
  float y_error = 0.0;
  float x_error = 0.0;
  float x_mult = 0.7, y_mult = 0.7;
  
  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if(!buf){
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  sensor_t * s = esp_camera_sensor_get();
  //flip the camera vertically
  //s->set_vflip(s, 1);          // 0 = disable , 1 = enable
  // mirror effect
  //s->set_hmirror(s, 1);          // 0 = disable , 1 = enable

  int res = 0;
  
  if(!strcmp(variable, "up")) {
    if(servo1Pos <= 150) {
      servo1Pos += SERVO_STEP;
      servo1.write(servo1Pos);
    }
    Serial.println(servo1Pos);
    Serial.println("Up");
  }
  else if(!strcmp(variable, "left")) {
    if(servo2Pos <= 170) {
      servo2Pos += SERVO_STEP;
      servo2.write(servo2Pos);
    }
    Serial.println(servo2Pos);
    Serial.println("Left");
  }
  else if(!strcmp(variable, "right")) {
    if(servo2Pos >= 10) {
      servo2Pos -= SERVO_STEP;
      servo2.write(servo2Pos);
    }
    Serial.println(servo2Pos);
    Serial.println("Right");
  }
  else if(!strcmp(variable, "down")) {
    if(servo1Pos >= 30) {
      servo1Pos -= SERVO_STEP;
      servo1.write(servo1Pos);
    }
    Serial.println(servo1Pos);
    Serial.println("Down");
  }
  ////////changed here
  else if(!strcmp(variable, "squirt")) {
    digitalWrite(DRIVER_PIN, HIGH); //!digitalRead(DRIVER_PIN)); //toggle
    squirt_tm = millis();
    squirt_water = true;
    
//    Serial.println(servo1Pos);
    Serial.println("squirt");
  }
  else if(!strcmp(variable, "track")) {
    // tracking_on = true;
    
    Serial.println("track");
  }
  else if(!strcmp(variable, "calibrate")) {
    calibrate = true;
    
    Serial.println("calibrate");
  }
  else if(!strcmp(variable, "reset_servos")) {
    servo1Pos = 70;
    servo2Pos = 80;
    servo2.write(servo2Pos);
    servo1.write(servo1Pos);
    
    Serial.println("reset_servos");
  }
  else if(!strcmp(variable, "track_off")) {
    // tracking_on = false;
    Serial.println("track off");
  }
  else if(!strcmp(variable, "target_get")) {
    Serial.println("in target_get");
    // tracking_on = true;
    if (move_servos_inloop){
      reply = "currently_tracking";
    } else {
      xSemaphoreTake(Semaphore_Controls,  portMAX_DELAY);
      fb = NULL;
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        reply = "image_not_captured";
      } else {
        esp_camera_fb_return(fb);
        delay(20);
        fb = NULL;
        fb = esp_camera_fb_get();
        if (!fb) {
          Serial.println("Camera capture failed");
          reply = "image_not_captured again";
        } else {
          delay(10);
          capture_still();
          update_frame();
          reply = "next";
          got_laser_image = true;
          if (fb){
            esp_camera_fb_return(fb);
            fb = NULL;
          }
        }
      }
      xSemaphoreGive(Semaphore_Controls);
    }
  }
  else if(!strcmp(variable, "laser_off")) {
    Serial.println("in laser_off");
    if(!got_laser_image){
      reply = "get_laser_first";
    } else if(move_servos_inloop){
      reply = "currently_tracking2";
    } else {
      xSemaphoreTake(Semaphore_Controls,  portMAX_DELAY);
      fb = NULL;
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        reply = "image_not_captured";
      } else {
        esp_camera_fb_return(fb);
        delay(20);
        fb = NULL;
        fb = esp_camera_fb_get();
        delay(10);
        if (!fb) {
          Serial.println("Camera capture failed");
          reply = "image_not_captured";
        } else {
          capture_still();
          get_diff_image();
          // update_frame();
          reply = "done";
          position = find_bright_spot_centroid();
          if (position.valid){
              y_error = 2*30.0 - position.y;
              x_error = 2*40.0 - position.x;
              x_move_calibrate = x_mult*x_error;
              y_move_calibrate = y_mult*y_error;
              move_servos_inloop = true;

            } else {
              x_move_calibrate = 0.0;
              y_move_calibrate = 0.0;
              reply = "position invalid";
            }
          // tracking_on = true;
          if (fb){
            esp_camera_fb_return(fb);
            fb = NULL;
          }
        }
      }
      Serial.println("returning semaphore");

      xSemaphoreGive(Semaphore_Controls);
    }
    // tracking_on = true;
    got_laser_image = false;

    
    
  }
  else {
    res = 1;
    // reply = ""
  }

  if(res){
    return httpd_resp_send_500(req);
  }
  Serial.println(reply.c_str());
  // httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  // httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, reply.c_str(), HTTPD_RESP_USE_STRLEN);
  // const char resp[] = "URI GET Response";
  // return httpd_resp_send(req, resp,  5); //HTTPD_RESP_USE_STRLEN);
  // return httpd_resp_send(req, NULL, 0);
}

void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}


#endif
