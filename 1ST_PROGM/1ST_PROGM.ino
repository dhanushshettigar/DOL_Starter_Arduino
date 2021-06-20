#include<EEPROM.h>
void setup() {
  // put your setup code here, to run once:
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.update(11,0);  
  EEPROM.update(10,0);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int lastnr = EEPROM.read(10);
  Serial.println(lastnr);
}
