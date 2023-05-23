#include <RC5.h>
 
#define L_PWM 5
#define L_DIR 4
#define R_PWM 6
#define R_DIR 9
#define PWM_MAX 165
 
#define BUZZER 10
#define LED 13
#define TSOP_PIN 3

const int trigPin = 8;
const int echoPin = 7;
 
RC5 rc5(TSOP_PIN); //Informacja o podłączeniu odbiornika TSOP
byte address; 
byte command;
byte toggle;

unsigned long time = 0;
int waiting_for_response = 0;
 
void setup() {
  //Konfiguracja pinow od mostka H
  pinMode(L_DIR, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);
 
  //Konfiguracja pozostalych elementow
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, 0); //Wylaczenie buzzera  
  pinMode(LED, OUTPUT); 
  digitalWrite(LED, 0); //Wylaczenie diody

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 
  Serial.begin(9600);
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  
}


void goForward() {
  leftMotor(40);
  rightMotor(40);
}

void goBack() {
  leftMotor(-40);
  rightMotor(-40);
}

void goLeft() {
  leftMotor(30);
  rightMotor(-30);
}

void goRight() {
  leftMotor(-30);
  rightMotor(30);
}

// stopMotors();

int distance_threshold = 15;
int delay_ms = 400;
int backward_ms = 200;

int current_mode = 1;

void loop() {
  if (current_mode == 1) {
    loop_remote();
  } else if (current_mode == 2) {
    loop_obstacles();
  } else if (current_mode == 3) {
    loop_distance();
  } else if (current_mode == 4) {
    loop_delay();
  } else if (current_mode == 5) {
    loop_backward();
  }
}

void loop_delay() {
  if (rc5.read(&toggle, &address, &command)){
     switch(command) {
      case 1:
        delay_ms = 50;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;

      case 2: //Do przodu
        delay_ms = 75;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;
 
      case 3: //Do tyłu
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        distance_threshold = 100 + (command - 3) * 50;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;      
      
      case 12:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        
        current_mode = 1;
        waiting_for_response = 0;
        digitalWrite(LED, 0);
        stopMotors();
      break;

      case 13:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 5;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 16:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 3;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 32:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 4;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;
    }
  }
}

void loop_backward() {
  if (rc5.read(&toggle, &address, &command)){
     switch(command) {
      case 1:
        backward_ms = 50;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;

      case 2: //Do przodu
        backward_ms = 75;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;
 
      case 3: //Do tyłu
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        backward_ms = 100 + (command - 3) * 50;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;      
      
      case 12:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        
        current_mode = 1;
        waiting_for_response = 0;
        digitalWrite(LED, 0);
        stopMotors();
      break;

      case 13:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 5;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 16:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 3;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 32:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 4;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;
    }
  }
}
void loop_distance() {
  if (rc5.read(&toggle, &address, &command)){
     switch(command) {
      case 1:
        distance_threshold = 10;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;

      case 2: //Do przodu
        distance_threshold = 12;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;
 
      case 3: //Do tyłu
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        distance_threshold = 15 + (command - 3) * 3;
        digitalWrite(BUZZER, 1);
        delay(command * 50);
        digitalWrite(BUZZER, 0);
      break;      
      
      case 12:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);

        current_mode = 1;
        waiting_for_response = 0;
        digitalWrite(LED, 0);
        stopMotors();
      break;

      case 13:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 5;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 16:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 3;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 32:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 4;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;
    }
  }
}

void loop_obstacles() {
  if (waiting_for_response == 0) {
    getDistance();
    waiting_for_response = 1;
  } else if (waiting_for_response == 1) {
    if (digitalRead(echoPin)) {
      waiting_for_response = 2;
      time = micros();
    }
  } else if (waiting_for_response == 2) {
    if (!digitalRead(echoPin)) {
      waiting_for_response = 0;
      long duration = micros() - time;
      // Calculating the distance
      int distance = duration * 0.034 / 2;

      if (distance < distance_threshold) {
        //digitalWrite(BUZZER, 1);
        //delay(200);
        //digitalWrite(BUZZER, 0);
        goBack();
        delay(backward_ms);
        goLeft();
        delay(delay_ms);
      } else {
        goForward();
      }
    }
  }
  

  if (rc5.read(&toggle, &address, &command)){
     switch(command) {

      case 12:
        digitalWrite(BUZZER, 1);
        delay(500);
        digitalWrite(BUZZER, 0);
        current_mode = 1;
        waiting_for_response = 0;
        digitalWrite(LED, 0);
        stopMotors();
      break;

      case 13:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 5;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 16:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 3;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 32:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 4;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;
    }
  }
}

void loop_remote() {
 if (rc5.read(&toggle, &address, &command)){
     switch(command) {
      case 2: //Do przodu
        goForward();
      break;
 
      case 8: //Do tyłu
        goBack();
      break;
 
      case 5: //STOP
        stopMotors();
      break;
 
      case 4: //Obrót w lewo
        goLeft();        
      break;   
      
      case 6: //Obrót w prawo
        goRight();
      break;       
      
      case 12:
        digitalWrite(BUZZER, 1);
        delay(500);
        digitalWrite(BUZZER, 0);
        current_mode = 2;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 13:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 5;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 16:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 3;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;

      case 32:
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(200);
        digitalWrite(BUZZER, 0);
        current_mode = 4;
        waiting_for_response = 0;
        digitalWrite(LED, 1);
        stopMotors();
      break;
    }
  }
}
 
void leftMotor(int V) {
  if (V > 0) { //Jesli predkosc jest wieksza od 0 (dodatnia)
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(L_DIR, 0); //Kierunek: do przodu
    analogWrite(L_PWM, V); //Ustawienie predkosci 
  } else {
    V = abs(V); //Funkcja abs() zwroci wartosc V  bez znaku
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(L_DIR, 1); //Kierunek: do tyłu
    analogWrite(L_PWM, V); //Ustawienie predkosci    
  }
}
 
void rightMotor(int V) {
  if (V > 0) { //Jesli predkosc jest wieksza od 0 (dodatnia)
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(R_DIR, 1); //Kierunek: do przodu
    analogWrite(R_PWM, V); //Ustawienie predkosci 
  } else {
    V = abs(V); //Funkcja abs() zwroci wartosc V  bez znaku
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(R_DIR, 0); //Kierunek: do tyłu
    analogWrite(R_PWM, V); //Ustawienie predkosci    
  }
}
 
void stopMotors() {
  analogWrite(L_PWM, 0); //Wylaczenie silnika lewego
  analogWrite(R_PWM, 0); //Wylaczenie silnika prawego
}