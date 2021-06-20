#include <EEPROM.h>

#include <DS3231.h>

DS3231  rtc(SDA, SCL);

int lastnr;
int runpot;
int offpot;
int lastpresent_hr;
int lastpresent_min;
int lastdelay_time;
int magdata;
int Sthr;
int Stmin;
int Pthr;
int Ptmin;
int remain_time;
int runpot_position_time;
int offpot_position_time;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void start_time_rtc()
{
  String startTime = rtc.getTimeStr();
  String Shr = getValue(startTime, ':', 0);
  int hr = Shr.toInt();
  String Smin = getValue(startTime, ':', 1);
  Stmin = Smin.toInt();
  if (hr > 12)
  {
    Sthr = 60 * (hr - 12);
  }
  else
  {
    Sthr = 60 * hr;
  }
}

void present_time_rtc()
{
  String PresentTime = rtc.getTimeStr();
  String Phr = getValue(PresentTime, ':', 0);
  int thr = Phr.toInt();
  String Pmin = getValue(PresentTime, ':', 1);
  Ptmin  = Pmin.toInt();
  EEPROM.update(1, Ptmin);
  if (thr > 12)
  {
    Pthr = 60 * (thr - 12);
    EEPROM.update(2, (thr - 12));
  }
  else
  {
    Pthr = 60 * thr;
    EEPROM.update(2, thr);
  }
}

void runpot_check();

void rundelay() {
  EEPROM.update(3, 1);
  EEPROM.update(4, runpot_position_time);
  start_time_rtc();
  bool flag1 = true;
  while (flag1)
  {
    present_time_rtc();
    EEPROM.update(5, Ptmin - Stmin);
    Serial.println(Ptmin - Stmin);
    if ((Ptmin - Stmin) == runpot_position_time)
    {

      delay(100);
      flag1 = false;
    }
  }
}
void offpot_check();

void offdelay() {
  EEPROM.update(3, 0);
  EEPROM.update(4, offpot_position_time);
  start_time_rtc();
  bool flag1 = true;
  while (flag1)
  {
    present_time_rtc();
    EEPROM.update(5, Ptmin - Stmin);
    Serial.println(Ptmin - Stmin);
    if ((Ptmin - Stmin) == offpot_position_time)
    {
      delay(100);
      flag1 = false;
    }
  }
}

void runpump() {
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  delay(700);
  digitalWrite(5, HIGH);
  delay(700);
}

void offpump() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  delay(700);
  digitalWrite(6, HIGH);
  delay(700);
}

void eepromdelay()
{
  Serial.println("inside eepromdelay"); Serial.println("\t");
  start_time_rtc();
  bool flag1 = true;
  while (flag1)
  {
    present_time_rtc();
    EEPROM.update(5, (Pthr + Ptmin) - (Sthr + Stmin));
    if ((Pthr + Ptmin) - (Sthr + Stmin) == magdata)
    {
      delay(100);
      flag1 = false;
    }
    Serial.println((Pthr + Ptmin) - (Sthr + Stmin)); Serial.println("\t");
  }
}

void checkeeprom()
{
  Serial.println("inside checkeeprom"); Serial.println("\t");
  int state = EEPROM.read(3);
  int lastpresent_hrr = EEPROM.read(2);
  lastpresent_hr = lastpresent_hrr;
  // Serial.println(lastpresent_hr);
  lastpresent_min = EEPROM.read(1);
  // Serial.println(lastpresent_min);
  lastdelay_time = EEPROM.read(4);
  //  Serial.println(lastdelay_time);
  remain_time = EEPROM.read(5);
  // Serial.println(remain_time);
  start_time_rtc();
  //  int data = (remain_time - ((Sthr + Stmin) + (lastdelay_time - (lastpresent_hr + lastpresent_min))));
  int data = (remain_time - lastdelay_time);
  //  int data = (lastdelay_time-remain_time);
  magdata = abs(data);
  Serial.println(magdata);
  if (state == 1)
  {
    Serial.println("state=1"); Serial.println("\t");
    runpump();
    eepromdelay();
  }
  else
  {
    offpump();
    eepromdelay();
  }

}

void setup() {
  rtc.begin(); // Initialize the rtc object

  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(2, LOW);

  int nr = EEPROM.read(10);
  lastnr = nr + 1;
  EEPROM.update(10, lastnr); //upload a empty skech with only lastnr=0
  Serial.begin(115200);
}

void loop() {
  runpot = map(analogRead(A2), 0, 1023, 0, 40);
  runpot_check();
  offpot = map(analogRead(A1), 0, 1023, 0, 30);
  offpot_check();

  if (lastnr == 1)
  {
    runpump();
    Serial.println("normal run pump"); Serial.println("\t");
    rundelay();
    offpump();
    Serial.println("normal off pump"); Serial.println("\t");
    offdelay();
  }
  else
  {

    bool flg = true;
    while (flg)
    {
      Serial.println("inside while"); Serial.println("\t");
      checkeeprom();
      flg = false;
    }
    lastnr = 1;
  }
}
void runpot_check() {
  if (runpot >= 35 && runpot <= 40)
  {
    runpot_position_time = 60;
    Serial.println("1hr");
  }
  else if (runpot >= 26 && runpot <= 36)
  {
    runpot_position_time = 2;
    Serial.println("2min");
  }
  else if (runpot >= 9 && runpot <= 25)
  {
    runpot_position_time = 30;
    Serial.println("30min");
  }
  else if (runpot >= 0 && runpot <= 8)
  {
    runpot_position_time = 15;
    Serial.println("15min");
  }
  else {
    runpot_position_time = 1;
    Serial.println("1min");
  }
}

void offpot_check() {
  if (offpot >= 11 && offpot <= 22)
  {
    offpot_position_time = 30; 
    Serial.println("30min");
  }
  else if (offpot >= 23 && offpot <= 26)
  {
    offpot_position_time = 5; 
    Serial.println("5min");
  }
  else if (offpot >= 27)
  {
    offpot_position_time = 120;
    Serial.println("2hr");
  }
  else if (offpot >= 0 && offpot <= 10)
  {
    offpot_position_time = 90; 
    Serial.println("1.5hr");
  }
  else {
    offpot_position_time = 1;
    Serial.println(offpot_position_time);
  }
}
