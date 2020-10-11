// Libraries
#include <ArduinoBlue.h>
#include <Servo.h>
#include "config.h"

ArduinoBlue phone(Serial1); //pass reference of bluetooth object to ArduinoBlue constructor.

Servo servo;
int angle = 35;

void setup() {    
  Serial.begin(BAUDRATE);
  Serial1.begin(BAUDRATE);  
  delay(100); // delay just in case bluetooth module needs time to "get ready".
  //Serial.println("setup complete");
  pinMode(VELOCITY_L, OUTPUT);
  pinMode(ROTATIONDIR_L1, OUTPUT);
  pinMode(ROTATIONDIR_L2, OUTPUT);
  pinMode(ROTATIONDIR_R1, OUTPUT);
  pinMode(ROTATIONDIR_R2, OUTPUT);
  pinMode(VELOCITY_R, OUTPUT);
  
  // Ultrasonic
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);

  // Servo
  servo.attach(27);
  servo.write(angle);  
}


void loop() {  
  throttle = phone.getThrottle(); // get bluetooth data
  steering = phone.getSteering(); 
  button = phone.getButton(); 
 
  sliderId = phone.getSliderId(); // ID of the slider moved. In this timegap sliderVal accessible.
  sliderVal = phone.getSliderVal(); // Slider value goes from 0 to 200.

  switch (sliderId) {  // changing the drive velocity depending on throttle/steering slider position
    case 1:
      velo_frontal = map(sliderVal, 0, 200, 80, 255);
      break;
    case 0:
      velo_spin = map(sliderVal, 0, 200, 80, 150);
      break;     
  }
 
  if (button != -1) {  // changing between steering and auto mode
    select_drivemode = button;
  }

  switch (select_drivemode) {
    case 3:
      drivemode(); // steering mode 
    case 2:
      selfdrivemode();  // auto mode 
  }
  
 

}

void findDistance() {
  int duration;
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);

  duration = pulseIn(ECHO,HIGH);
  distance = duration / 2 / 7.6;
  delay(60); //metro.h

}


void selfdrivemode() {  
  for (angle=35; angle <= 70; angle += 10){
    servo.write(angle);
    findDistance();
    Serial.println(distance); //value 10 equals 1cm obstacle distance
    if (distance <= 70) {
      analogWrite(VELOCITY_L, 100);
      analogWrite(VELOCITY_R, 100);
      right();
      delay(800);
      stopengine();
    }
    delay(15);
  }
  
  servo.write(35);
  
  for (angle=35; angle >= 0; angle -= 10){
    servo.write(angle);
    findDistance();
    Serial.println(distance); //value 10 equals 1cm obstacle distance
    if (distance <= 70) {
      analogWrite(VELOCITY_L, 100);
      analogWrite(VELOCITY_R, 100);
      left();
      delay(800);
      stopengine();
    }
    delay(15);
  }
  
  servo.write(35);
  
  findDistance();
  Serial.println(distance); //value 10 equals 1cm obstacle distance
  if (distance > 70) {
    analogWrite(VELOCITY_L, 100);
    analogWrite(VELOCITY_R, 100);
    forward();
    delay(400);
    stopengine();
  }
}

void drivemode() {
  findDistance();
  Serial.println(distance); //value 10 equals 1cm obstacle distance
  if (distance >= 100) {
    drivesetup();
  }
  else {
    analogWrite(VELOCITY_L, 90);
    analogWrite(VELOCITY_R, 90);
    left();
    delay(1000);
  }          
        
}

void drivesetup() {  
  if (throttle > 60) { //switch ..get rid of if
    if (steering > 40 && steering < 60) {
      analogWrite(VELOCITY_L, velo_frontal);
      analogWrite(VELOCITY_R, velo_frontal);
      forward();
    }
    if (steering <= 40 && steering >= 10) {
      velo_slowturn = map(steering, 10, 40, 0, velo_frontal);
      analogWrite(VELOCITY_L, velo_slowturn); //decrease
      analogWrite(VELOCITY_R, velo_frontal); //hold
      forward();
    }
    if (steering >= 60 && steering <= 90) {
      velo_slowturn = map(steering, 60, 90, velo_frontal, 0);      
      analogWrite(VELOCITY_L, velo_frontal); //hold
      analogWrite(VELOCITY_R, velo_slowturn); //decrease
      forward();
    }
    if (steering < 10) {
      analogWrite(VELOCITY_L, velo_spin);
      analogWrite(VELOCITY_R, velo_spin);
      left(); 
    }
    if (steering > 90) {
      analogWrite(VELOCITY_L, velo_spin);
      analogWrite(VELOCITY_R, velo_spin);
      right(); 
    }
  }    
  if (throttle < 40) {
    if (steering > 40 && steering < 60) {
      analogWrite(VELOCITY_L, velo_frontal);
      analogWrite(VELOCITY_R, velo_frontal);
      back();
    }
    if (steering <= 40 && steering >= 25) {
      velo_slowturn = map(steering, 25, 40, 0, velo_frontal);
      analogWrite(VELOCITY_L, velo_slowturn); //decrease
      analogWrite(VELOCITY_R, velo_frontal); //hold
      back();
    }
    if (steering >= 60 && steering <= 75) {
      velo_slowturn = map(steering, 60, 75, velo_frontal, 0);
      analogWrite(VELOCITY_L, velo_frontal); //hold
      analogWrite(VELOCITY_R, velo_slowturn); //decrease
      back();
    }
    if (steering < 10) {
      analogWrite(VELOCITY_L, velo_spin);
      analogWrite(VELOCITY_R, velo_spin);
      left(); 
    }
    if (steering > 90) {
      analogWrite(VELOCITY_L, velo_spin);
      analogWrite(VELOCITY_R, velo_spin);
      right(); 
    }
  }
   
  if (throttle == 49) {
    stopengine();
  }
}

void forward() {
  digitalWrite(ROTATIONDIR_L1, HIGH);
  digitalWrite(ROTATIONDIR_L2, LOW);
  digitalWrite(ROTATIONDIR_R1, HIGH);
  digitalWrite(ROTATIONDIR_R2, LOW);
}

void back() {
  digitalWrite(ROTATIONDIR_L1, LOW);
  digitalWrite(ROTATIONDIR_L2, HIGH);
  digitalWrite(ROTATIONDIR_R1, LOW);
  digitalWrite(ROTATIONDIR_R2, HIGH);
}

void left() {
  digitalWrite(ROTATIONDIR_L1, LOW);
  digitalWrite(ROTATIONDIR_L2, HIGH);
  digitalWrite(ROTATIONDIR_R1, HIGH);
  digitalWrite(ROTATIONDIR_R2, LOW);
}

void right() {
  digitalWrite(ROTATIONDIR_L1, HIGH);
  digitalWrite(ROTATIONDIR_L2, LOW);
  digitalWrite(ROTATIONDIR_R1, LOW);
  digitalWrite(ROTATIONDIR_R2, HIGH);
}

void stopengine() {
  digitalWrite(ROTATIONDIR_L1, LOW);
  digitalWrite(ROTATIONDIR_L2, LOW);
  digitalWrite(ROTATIONDIR_R1, LOW);
  digitalWrite(ROTATIONDIR_R2, LOW);
}
