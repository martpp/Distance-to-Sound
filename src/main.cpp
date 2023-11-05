#include <Arduino.h>
#include <Ultrasonic.h>
// pin definitions for 3 HC-RS04

#define LED_BUILTIN 1
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

class MovingAverageFilter {
public:
  MovingAverageFilter(int window_size)
      : windowsize(window_size), index(0), sum(0.0f) {
    inputhistory = new float[windowsize];
    for (int i = 0; i < windowsize; i++) {
      inputhistory[i] = 0.0f;
    }
  }

  ~MovingAverageFilter() { delete[] inputhistory; }

  float update(float input) {
    sum -= inputhistory[index];       // remove the oldest value from the sum
    inputhistory[index] = input;      // insert the new value
    sum += input;                     // add the new value to the sum
    index = (index + 1) % windowsize; // move to the next position
    return sum / windowsize;
  }

private:
  float *inputhistory;
  int windowsize;
  int index;
  float sum;
};

int windowsize = 4;
MovingAverageFilter filter_0(windowsize);
MovingAverageFilter filter_1(windowsize);
MovingAverageFilter filter_2(windowsize);

float fdist0, fdist1, fdist2;

void setup() {
  Serial.begin(9600);
  pinMode(ampPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  unsigned int current_time_ms = millis();
  fdist0 = filter_0.update(ultrasonic0.read());
  fdist1 = filter_1.update(ultrasonic1.read());
  fdist2 = filter_2.update(ultrasonic2.read());
  unsigned int distance = min(fdist0, min(fdist1, fdist2));

  Serial.print(">dist0:");
  Serial.println(ultrasonic0.read());
  Serial.print(">dist0 filtered:");
  Serial.println(fdist0);
  Serial.print(">dist1:");
  Serial.println(ultrasonic1.read());
  Serial.print(">dist1 filtered:");
  Serial.println(fdist1);
  Serial.print(">dist2:");
  Serial.println(ultrasonic2.read());
  Serial.print(">dist2 filtered:");
  Serial.println(fdist2);

  // map distance to a duration value between 200 and 2000ms
  unsigned int duration1 = map(distance, 10, 100, 50, 1000);

  if (current_time_ms - last_time_ms > duration1 && distance < 100 &&
      distance > 10) {
    last_time_ms = current_time_ms;
    // play a tone with the duration
    tone(ampPin, 1000, duration1 / 2);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(10);
}