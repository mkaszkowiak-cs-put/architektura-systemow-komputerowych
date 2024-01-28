#define L_PWM 5
#define L_DIR 4
#define R_PWM 6
#define R_DIR 9

void setup() {
  //Konfiguracja pinow od mostka H
  pinMode(L_DIR, OUTPUT);
  pinMode(R_DIR, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);
}

void loop() {
  //Obroty silnika przez sekundę do przodu z prędkością 100
  digitalWrite(R_DIR, 0); //Ustawienie kierunku
  analogWrite(R_PWM, 100); //Ustawienie predkosci
  delay(1000); //Opoznienie 1 sekunda
  //Zatrzymanie silnika na sekundę
  digitalWrite(R_DIR, 0); //Ustawienie kierunku
  analogWrite(R_PWM, 0); //Wylaczenie silnika
  delay(1000); //Opoznienie 1 sekunda
}