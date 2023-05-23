 #define LED 13
 #define BUZZER 10
 #define PRZYCISK 2
 
void setup() {
  //Konfiguracja wyjsc
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //Konfiguracja wyjscia
  pinMode(PRZYCISK, INPUT_PULLUP); 
 
  //Wylaczenie diody i buzzera
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);
 
  //Testowe migniecia LED
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}
 
void loop() {
  if (digitalRead(PRZYCISK) == LOW) { //Jesli przycisk jest wcisniety...
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER, HIGH);   
    delay(500);
  } else { //Jesli przycisk nie jest wcisniety...
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER, LOW);
  }
}