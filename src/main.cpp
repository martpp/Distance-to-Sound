#include <Arduino.h>
#include <Ultrasonic.h>
// pin definitions for 3 HC-RS04
#define trigPin1 2
#define echoPin1 3
#define trigPin2 4
#define echoPin2 5
#define trigPin3 6
#define echoPin3 7
// define pin for lm386-amp
#define ampPin 9
// define the ultrasonic sensor object
Ultrasonic ultrasonic0(trigPin1, echoPin1);
Ultrasonic ultrasonic1(trigPin2, echoPin2);
Ultrasonic ultrasonic2(trigPin3, echoPin3);

unsigned int duration_ms;
unsigned int last_time_ms;

void setup()
{
  Serial.begin(9600);
  pinMode(ampPin, OUTPUT);
}

void loop()
{
  unsigned int current_time_ms = millis();
  unsigned int distance1 = ultrasonic1.read();
  // map distance to a duration value between 200 and 2000ms
  unsigned int duration1 = map(distance1, 10, 100, 50, 1000);


  if(current_time_ms - last_time_ms > duration1 && distance1 < 100 && distance1 > 10)
  {
    last_time_ms = current_time_ms;
    Serial.print("Distance1: ");
    Serial.print(distance1);
    Serial.print(" Duration1: ");
    Serial.println(duration1);
    // play a tone with the duration
    tone(ampPin, 1000, duration1 / 2);
  }
  delay(10);
}