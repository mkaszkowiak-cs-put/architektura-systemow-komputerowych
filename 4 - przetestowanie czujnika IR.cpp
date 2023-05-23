#define TSOP_PIN 3
 
#include <RC5.h>
RC5 rc5(TSOP_PIN); //Informacja o podłączeniu odbiornika TSOP
 
//Zmienne dla RC5
byte address; 
byte command;
byte toggle;
 
void setup() {                
  Serial.begin(9600);
}
 
void loop() {
  //Jeśli odebrano komendę
  if (rc5.read(&toggle, &address, &command))
  {
    Serial.print("A:");
    Serial.print(address);
    Serial.print(" K:");
    Serial.print(command);
    Serial.print(" T:");
    Serial.println(toggle);
  }
}