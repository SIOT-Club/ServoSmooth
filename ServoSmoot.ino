#include <Servo.h>
Servo ZaHando;
int endVal = 0, signal = 0;
const int trigPin = 9, echoPin = 10, maxDistanceToScan = 40;

void SmoothBore(Servo X, int maxInt, int increment, int& currentVal, int delayTime = 500) {
  if (maxInt < 0 || maxInt > 180) {
    Serial.println("Not valid maximum integer");
  } else if (currentVal < maxInt) {
    if (currentVal == maxInt)
      ;
    else {
      for (int i = 0; i <= maxInt; i += increment) {
        X.write(i);
        delay(delayTime);
      }
      currentVal = maxInt;
    }
  }
  else if (currentVal >= maxInt){
    if(currentVal == maxInt);
    else{
      for(int i = 0; i >= maxInt; i -= increment){
        X.write(i);
        delay(delayTime);
      }
      currentVal = maxInt;
    }
  }
}

int CalcDistance(int echo) {
  long duration;
  duration = pulseIn(echo, 1);
  return duration / 58.2;
}

int DistanceUltra(int trigger, int echo) {
  int delay = 2, delayAtt = 10;

  digitalWrite(trigger, 0);
  delayMicroseconds(delay);
  digitalWrite(trigger, 1);
  delayMicroseconds(delayAtt);
  digitalWrite(trigger, 0);

  return CalcDistance(echo);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ZaHando.attach(6);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  signal = DistanceUltra(trigPin, echoPin);
  Serial.println(endVal);
  if (signal < maxDistanceToScan) {
    SmoothBore(ZaHando, 90, 15, endVal);
  } else {
    SmoothBore(ZaHando, 0, 15, endVal);
  }
}
