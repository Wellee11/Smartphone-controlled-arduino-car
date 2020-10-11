// Libraries
#include <ArduinoBlue.h>
#include <Servo.h>

// Ultrasonic Sensor
#define TRIGGER 23
#define ECHO 25
int distance;

// Bluetooth data
int prevThrottle = 49;
int prevSteering = 49;
int throttle, steering, sliderVal, sliderId, button;
int automotive = 3; // starting in steering mode

ArduinoBlue phone(Serial1); // pass reference of bluetooth object to ArduinoBlue constructor.

// Engines
int ENA = 3;
int IN1 = 2;
int IN2 = 4;
int IN3 = 7; 
int IN4 = 8; 
int ENB = 9;

// Velocities
int acc = 80 + 175/2;
int spin = 115;
int turn;

// Servo
Servo servo;
int angle = 35;

void setup() {    
  Serial.begin(9600);
  Serial1.begin(9600);
  // delay just in case bluetooth module needs time to "get ready".
  delay(100);
  Serial.println("setup complete");
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  // Ultrasonic
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);

  // Servo
  servo.attach(27);
  servo.write(angle);
  
}


void loop() {
  
  // get bluetooth data
  throttle = phone.getThrottle();
  steering = phone.getSteering();
  button = phone.getButton(); 
  // ID of the slider moved. In this timegap sliderVal accessible.
  sliderId = phone.getSliderId();
  // Slider value goes from 0 to 200.
  sliderVal = phone.getSliderVal();

  // changing the velocity depending on throttle slider position
  if (sliderId == 1) {
    Serial.print("Slider ID: ");
    Serial.print(sliderId);
    Serial.print("\tValue: ");
    Serial.println(sliderVal);
    acc = map(sliderVal, 0, 200, 80, 255);
  }
  // changing between steering and auto mode
  if (button != -1) {
    automotive = button;
  }
  // changing turn speed depending on steering slider position
  if (sliderId == 0) {
    Serial.print("Slider ID: ");
    Serial.print(sliderId);
    Serial.print("\tValue: ");
    Serial.println(sliderVal);
    spin = map(sliderVal, 0, 200, 80, 150);
  }
  
  // Display throttle and steering data if steering or throttle value is changed (only if usb is attached)
  if (prevThrottle != throttle || prevSteering != steering) {
    Serial.print("Throttle: "); Serial.print(throttle); Serial.print("\tSteering: "); Serial.println(steering);
    prevThrottle = throttle;
    prevSteering = steering;
  }

  if (automotive == 3) {
    drivemode(); // steering mode
  }
  if (automotive == 2) {
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
      analogWrite(ENA, 100);
      analogWrite(ENB, 100);
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
      analogWrite(ENA, 100);
      analogWrite(ENB, 100);
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
    analogWrite(ENA, 100);
    analogWrite(ENB, 100);
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
    analogWrite(ENA, 90);
    analogWrite(ENB, 90);
    left();
    delay(1000);
  }          
        
}

void drivesetup() {  
  if (throttle > 60) { //switch ..get rid of if
    if (steering > 40 && steering < 60) {
      analogWrite(ENA, acc);
      analogWrite(ENB, acc);
      forward();
    }
    if (steering <= 40 && steering >= 10) {
      turn = map(steering, 10, 40, 0, acc);
      analogWrite(ENA, turn); //decrease
      analogWrite(ENB, acc); //hold
      forward();
    }
    if (steering >= 60 && steering <= 90) {
      turn = map(steering, 60, 90, acc, 0);      
      analogWrite(ENA, acc); //hold
      analogWrite(ENB, turn); //decrease
      forward();
    }
    if (steering < 10) {
      analogWrite(ENA, spin);
      analogWrite(ENB, spin);
      left(); 
    }
    if (steering > 90) {
      analogWrite(ENA, spin);
      analogWrite(ENB, spin);
      right(); 
    }
  }    
  if (throttle < 40) {
    if (steering > 40 && steering < 60) {
      analogWrite(ENA, acc);
      analogWrite(ENB, acc);
      back();
    }
    if (steering <= 40 && steering >= 25) {
      turn = map(steering, 25, 40, 0, acc);
      analogWrite(ENA, turn); //decrease
      analogWrite(ENB, acc); //hold
      back();
    }
    if (steering >= 60 && steering <= 75) {
      turn = map(steering, 60, 75, acc, 0);
      analogWrite(ENA, acc); //hold
      analogWrite(ENB, turn); //decrease
      back();
    }
    if (steering < 10) {
      analogWrite(ENA, spin);
      analogWrite(ENB, spin);
      left(); 
    }
    if (steering > 90) {
      analogWrite(ENA, spin);
      analogWrite(ENB, spin);
      right(); 
    }
  }
   
  if (throttle == 49) {
    stopengine();
  }
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void back() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopengine() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
