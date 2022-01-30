#include <DS3231.h>

DS3231  rtc(SDA, SCL);

int Relay1Pin = 2;
int Relay2Pin = 3;

int LedRedPin = 4;
int LedGreenPin = 5;

int PotRunPin = A0;
int PotOffPin = A1;

Time  t;

int checkPotValue();
int checkOffPotValue();
bool OnPump = true;
int Start_hour, Start_min;
long Present_time_in_min, Time_Till_Pump_On, Time_Till_Pump_Off;
int Extra_time;

void On_the_Pump() {
  digitalWrite(Relay1Pin, LOW);
  digitalWrite(Relay2Pin, HIGH);
  digitalWrite(LedGreenPin, HIGH);
  digitalWrite(LedRedPin, LOW);
  delay(700);
  digitalWrite(Relay1Pin, HIGH);
  delay(700);
}

void Off_the_Pump() {
  digitalWrite(Relay1Pin, HIGH);
  digitalWrite(Relay2Pin, LOW);
  digitalWrite(LedGreenPin, LOW);
  digitalWrite(LedRedPin, HIGH);
  delay(700);
  digitalWrite(Relay2Pin, HIGH);
  delay(700);
}

void setup() {
  rtc.begin(); // Initialize the rtc object

  pinMode(PotRunPin, INPUT);
  pinMode(PotOffPin, INPUT);
  pinMode(Relay1Pin, OUTPUT);
  pinMode(Relay2Pin, OUTPUT);
  pinMode(LedRedPin, OUTPUT);
  pinMode(LedGreenPin, OUTPUT);

  digitalWrite(Relay1Pin, HIGH);
  digitalWrite(Relay2Pin, HIGH);
  digitalWrite(LedRedPin, HIGH);
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  t = rtc.getTime();
  Start_hour = t.hour;
  Start_min = t.min;

  int Total_on_time = checkOnPotValue();
  Serial.print("POT ON Value ");
  Serial.println(Total_on_time);

  Present_time_in_min = (Start_hour * 60) + Start_min;

  Serial.print("Present Time ");
  Serial.println(Present_time_in_min);

  Time_Till_Pump_On = Present_time_in_min + Total_on_time;

  if (Time_Till_Pump_On > 1439)
  {
    Extra_time = Time_Till_Pump_On - 1439;
    Serial.print("Extra_time:");
    Serial.println(Extra_time);
  }

  On_the_Pump();
  Serial.print("Time_Till_Pump_On:");
  Serial.println(Time_Till_Pump_On);
  while (OnPump)
  {
    t = rtc.getTime();
    Start_hour = t.hour;
    Start_min = t.min;
    Present_time_in_min = (Start_hour * 60) + Start_min;

    Serial.print("Present On Time:");
    Serial.println(Present_time_in_min);

    if (Present_time_in_min == 0)
    {
      Time_Till_Pump_On = Extra_time;
    }
    if (Time_Till_Pump_On == Present_time_in_min)
    {
      OnPump = false;
    }
    delay(10000);
  }

  int Total_off_time = checkOffPotValue();

  Serial.print("POT OFF Value ");
  Serial.println(Total_off_time);

  t = rtc.getTime();
  Start_hour = t.hour;
  Start_min = t.min;
  Present_time_in_min = (Start_hour * 60) + Start_min;

  Serial.print("Present Time ");
  Serial.println(Present_time_in_min);

  Time_Till_Pump_Off = Present_time_in_min + Total_off_time;

  Serial.print("Time_Till_Pump_Off ");
  Serial.println(Time_Till_Pump_Off);

  if (Time_Till_Pump_Off > 1439)
  {
    Extra_time = Time_Till_Pump_On - 1439;
    Serial.print("Extra_time:");
    Serial.println(Extra_time);
  }
  Serial.print("Time_Till_Pump_Off:");
  Serial.println(Time_Till_Pump_Off);
  Off_the_Pump();
  while (!OnPump)
  {
    t = rtc.getTime();
    Start_hour = t.hour;
    Start_min = t.min;
    Present_time_in_min = (Start_hour * 60) + Start_min;
    if (Present_time_in_min == 0)
    {
      Time_Till_Pump_Off = Extra_time;
    }
    if (Time_Till_Pump_Off == Present_time_in_min)
    {
      OnPump = true;
    }
    Serial.print("Present Off Time:");
    Serial.println(Present_time_in_min);
    delay(10000);
  }
}
int checkOnPotValue()
{
  int runpot = map(analogRead(PotRunPin), 0, 1023, 0, 100);
  if (runpot >= 0 && runpot <= 2)
  {
    return 2;
    //Serial.println("2min");
  }
  else if (runpot >= 3  && runpot <= 29)
  {
    return 5;
    //Serial.println("5min");
  }
  else if (runpot >= 30 && runpot <= 63)
  {

    return 15;
    //Serial.println("15min");
  }
  else if (runpot >= 64 && runpot <= 98)
  {
    return 30;
    //Serial.println("30min");
  }
  else if (runpot >= 99 && runpot <= 100)
  {
    return 60;
    //Serial.println("1Hr");
  }
  else {
    return 1;
    //Serial.println("1min");
  }
}
int checkOffPotValue()
{
  int offpot = map(analogRead(PotOffPin), 0, 1023, 0, 100);
  if (offpot >= 0 && offpot <= 2)
  {
    return 5;
    //Serial.println("5min");
  }
  else if (offpot >= 3 && offpot <= 18)
  {
    return 30;
    //Serial.println("30min");
  }
  else if (offpot >= 19 && offpot <= 35)
  {
    return 60;
    //Serial.println("1Hr");
  }
  else if (offpot >= 36 && offpot <= 72)
  {
    return 90;
    //Serial.println("1.5hr");
  }
  else if (offpot >= 80 && offpot <= 100)
  {
    return 120;
    //Serial.println("2hr");
  }
  else {
    return 1;
    //Serial.println(offpot_position_time);
  }
}
