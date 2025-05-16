/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-projects-ebook/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include "wifi_cam.h"


unsigned long st = 200;

void do_squirt(unsigned long st_ = 200){
  st = st_;
  digitalWrite(DRIVER_PIN, HIGH);
  squirt_water = true;
  squirt_tm = millis();
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  
  Serial.begin(115200);
  // Serial.println("here");
  ////////////////////////////////////////////////////////////
  //changed here
  squirt_tm = millis();
  pinMode(DRIVER_PIN, OUTPUT);
  digitalWrite(DRIVER_PIN, LOW); //make sure motor is off
  ///////////////////////////////////////////////////////
  
  servo1.setPeriodHertz(50);    // standard 50 hz servo
  servo2.setPeriodHertz(50);    // standard 50 hz servo
  servoN1.attach(2, 1000, 2000);
  servoN2.attach(13, 1000, 2000);
  
  servo1.attach(SERVO_1, 1000, 2000);
  servo2.attach(SERVO_2, 1000, 2000);
  
  servo1Pos = 70; //top
  servo2Pos = 80;
  servo1.write(servo1Pos);
  servo2.write(servo2Pos);

  
  
  
  Serial.setDebugOutput(false);

  configure_camera();
  

  // Wi-Fi connection
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  /*
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  */
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.println(WiFi.localIP());
  Semaphore_Controls = xSemaphoreCreateMutex();
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, HIGH);
  pinMode(DRIVER_PIN, OUTPUT);
  digitalWrite(DRIVER_PIN, LOW);
  // Start streaming web server
  startCameraServer();
  servo1.write(servo1Pos);
  servo2.write(servo2Pos);
}

void loop() {

  if (move_servos_inloop){
    Serial.println("in loop");
    Serial.println(x_move_calibrate);
    Serial.println(y_move_calibrate);
    adjust_servos((int) x_move_calibrate, (int) y_move_calibrate);
    delay(500);
    digitalWrite(DRIVER_PIN, HIGH);
    delay(400);
    digitalWrite(DRIVER_PIN, LOW);
    move_servos_inloop = false;
    // do_squirt();
  }

  if(squirt_water && (millis()-squirt_tm)>st){
    digitalWrite(DRIVER_PIN, LOW);
    squirt_water = false;
  }

 delay(50);
}
