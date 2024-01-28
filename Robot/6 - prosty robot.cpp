#include <RC5.h>
 
#define L_PWM 5
#define L_DIR 4
#define R_PWM 6
#define R_DIR 9
#define PWM_MAX 165
 
#define BUZZER 10
#define LED 13
#define TSOP_PIN 3
 
RC5 rc5(TSOP_PIN); //Informacja o podłączeniu odbiornika TSOP
byte address; 
byte command;
byte toggle;
 
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
 
  Serial.begin(9600);
}
 
void loop() {
 if (rc5.read(&toggle, &address, &command)){
     switch(command) {
      case 2: //Do przodu
        leftMotor(40);
        rightMotor(40);
      break;
 
      case 8: //Do tyłu
        leftMotor(-40);
        rightMotor(-40);
      break;
 
      case 5: //STOP
        stopMotors();
      break;
 
      case 4: //Obrót w lewo
        leftMotor(-30);
        rightMotor(30);
      break;   
      
      case 6: //Obrót w prawo
        leftMotor(30);
        rightMotor(-30);
      break;       
      
      case 12:
        digitalWrite(BUZZER, 1);
        delay(500);
        digitalWrite(BUZZER, 0);
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
    digitalWrite(R_DIR, 0); //Kierunek: do przodu
    analogWrite(R_PWM, V); //Ustawienie predkosci 
  } else {
    V = abs(V); //Funkcja abs() zwroci wartosc V  bez znaku
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(R_DIR, 1); //Kierunek: do tyłu
    analogWrite(R_PWM, V); //Ustawienie predkosci    
  }
}
 
void stopMotors() {
  analogWrite(L_PWM, 0); //Wylaczenie silnika lewego
  analogWrite(R_PWM, 0); //Wylaczenie silnika prawego
}